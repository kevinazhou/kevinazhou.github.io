#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Collider.h"
#include "SecretBlock.h"
#include "Door.h"
#include "Sword.h"
#include "EnemyComponent.h"


//#include "Math.h"

enum class PlayerDirection
{
	Left,
	Right,
	Up,
	Down
};

class PlayerMove : MoveComponent
{
public:
	PlayerMove::PlayerMove(class Actor* a)
		:MoveComponent(a)
	{
		g = mOwner->GetGame();
		mSpacePressed = false;

		ded = false;
		enumDirection = PlayerDirection::Up;
		pushingSBlock = false;
		sword = new Sword(g);
		attackTimer = 0.0f;
		lastDmgTaken = 0;
	}

	void ProcessInput(const Uint8* keyState)
	{
		AnimatedSprite* sprite = mOwner->GetComponent<AnimatedSprite>();
		sprite->SetIsPaused(false);
		if (keyState[SDL_SCANCODE_LEFT])
		{
			direction = Vector2(-1, 0);
			if (pushingSBlock)
			{
				SetForwardSpeed(50.0f);
			}
			else
			{
				SetForwardSpeed(130.0f);
			}
			
			sprite->SetAnimation("walkLeft");
			enumDirection = PlayerDirection::Left;
		}
		else if (keyState[SDL_SCANCODE_RIGHT])
		{
			direction = Vector2(1, 0);
			if (pushingSBlock)
			{
				SetForwardSpeed(50.0f);
			}
			else
			{
				SetForwardSpeed(130.0f);
			}
			sprite->SetAnimation("walkRight");
			enumDirection = PlayerDirection::Right;
		}
		else if (keyState[SDL_SCANCODE_DOWN])
		{
			direction = Vector2(0, 1);
			if (pushingSBlock)
			{
				SetForwardSpeed(50.0f);
			}
			else
			{
				SetForwardSpeed(130.0f);
			}
			sprite->SetAnimation("walkDown");
			enumDirection = PlayerDirection::Down;
		}
		else if (keyState[SDL_SCANCODE_UP])
		{
			direction = Vector2(0, -1);
			if (pushingSBlock)
			{
				SetForwardSpeed(50.0f);
			}
			else
			{
				SetForwardSpeed(130.0f);
			}
			sprite->SetAnimation("walkUp");
			enumDirection = PlayerDirection::Up;
		}
		else
		{
			SetForwardSpeed(0.0f);
			if (attackTimer <= 0.0f)
			{
				sprite->SetIsPaused(true);
			}
			
		}

		pushingSBlock = false;

		if (keyState[SDL_SCANCODE_SPACE])
		{
			if (!mSpacePressed)//when this frame is the leading edge
			{
				attackTimer = 0.25f;
				UpdateSword();
				mSpacePressed = true;
				Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/SwordSlash.wav"), 0);
			}

		}
		else
		{
			if (mSpacePressed)//trailing edge
			{

			}
			mSpacePressed = false;//when space is not pressed this fram
		}

		if (attackTimer > 0)//still attacking
		{
			AnimatedSprite* sprite = mOwner->GetComponent<AnimatedSprite>();
			if (enumDirection == PlayerDirection::Up)
			{
				sprite->SetAnimation("attackUp", true);//need to reset animation for some reason
			}
			else if (enumDirection == PlayerDirection::Down)
			{
				sprite->SetAnimation("attackDown", true);
			}
			else if (enumDirection == PlayerDirection::Right)
			{
				sprite->SetAnimation("attackRight", true);
			}
			else if (enumDirection == PlayerDirection::Left)
			{
				sprite->SetAnimation("attackLeft", true);
			}
		}
		else
		{
			if (enumDirection == PlayerDirection::Up)
			{
				sprite->SetAnimation("walkUp");
			}
			else if (enumDirection == PlayerDirection::Down)
			{
				sprite->SetAnimation("walkDown");
			}
			else if (enumDirection == PlayerDirection::Left)
			{
				sprite->SetAnimation("walkLeft");
			}
			else if (enumDirection == PlayerDirection::Right)
			{
				sprite->SetAnimation("walkRight");
			}
		}
	}

	void Update(float deltaTime)
	{
		Vector2 pos = mOwner->GetPosition();
		Vector2 c = g->GetCamera();
		Vector2 change = direction * GetForwardSpeed() * deltaTime;
		//Vector2 change = Vector2(GetForwardSpeed()*deltaTime, mYSpeed * deltaTime);
		pos = pos + change;

		if (attackTimer <= 0.0f)
		{
			mOwner->SetPosition(pos);
		}
		else
		{
			attackTimer -= deltaTime;
		}
		

		

		//collision stuff
		CollisionComponent* linkCollision = mOwner->GetComponent<CollisionComponent>();

		std::vector<Actor* > enemyVect = g->GetActorsInARoom(g->currentRoom);
		CollisionComponent* swordColl = sword->GetComponent<CollisionComponent>();

		for (size_t i = 0; i < enemyVect.size(); i++)
		{
			bool successfulHit = false;
			if (attackTimer > 0.0f)//if attacking
			{
				Vector2 offset = Vector2::Zero;
				CollisionComponent* enemyColl = enemyVect[i]->GetComponent<CollisionComponent>();
				CollSide side = swordColl->GetMinOverlap(enemyColl, offset);
				if (side != CollSide::None)//hit
				{
					EnemyComponent* e = enemyVect[i]->GetComponent<EnemyComponent>();
					int g = 5;
					e->TakeDamage(1);
					successfulHit = true;
				}
			}
			if (attackTimer <= 0.0f || successfulHit == false)
			{
				Vector2 offset = Vector2::Zero;
				CollisionComponent* enemyColl = enemyVect[i]->GetComponent<CollisionComponent>();
				CollSide side = linkCollision->GetMinOverlap(enemyColl, offset);
				if (side != CollSide::None)
				{
					Vector2 position = mOwner->GetPosition();
					position += offset;
					position = position + (direction * -32);
					mOwner->SetPosition(position);
					
					unsigned int curr = SDL_GetTicks();
					if (curr - lastDmgTaken > 500)
					{
						EnemyComponent* e = enemyVect[i]->GetComponent<EnemyComponent>();
						g->PlayerTakeDmg(e->collideDmg);
						lastDmgTaken = curr;
					}
					

					
				}
			}
		}


		std::vector<SecretBlock* > SBlocks = g->GetSBlockVector();

		for (size_t i = 0; i < SBlocks.size(); i++)
		{
			Vector2 offset = Vector2::Zero;
			CollisionComponent* blockColl = SBlocks[i]->GetComponent<CollisionComponent>();
			CollSide side = linkCollision->GetMinOverlap(blockColl, offset);
			if (side != CollSide::None)
			{
				if (side == SBlocks[i]->direction && SBlocks[i]->fullyPushed == false)//then you can push it
				{
					if (SBlocks[i]->initiated == false)
					{
						SBlocks[i]->initialPos = SBlocks[i]->GetPosition();
						SBlocks[i]->initiated = true;
					}
					
					pushingSBlock = true;
					Vector2 blockPos = SBlocks[i]->GetPosition();
					Vector2 finalPos = blockPos - offset;
					SBlocks[i]->SetPosition(finalPos);

					Vector2 diff = finalPos - SBlocks[i]->initialPos;
					if (abs(diff.x) >= 32 || abs(diff.y) >= 32)
					{
						SBlocks[i]->fullyPushed = true;
						std::vector<Door* > dors = g->GetDoorsInARoom(g->currentRoom);
						for (size_t k = 0; k < dors.size(); k++)
						{
							if (dors[k]->GetDoorState() == DoorState::Closed)//opening all the doors
							{
								dors[k]->SetDoorState(DoorState::Open);
								Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/DoorOpen.wav"), 0);
								Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Secret.wav"), 0);
							}
						}
					}
				}
				else//otherwise collide like usual
				{
					Vector2 position = mOwner->GetPosition();
					SetForwardSpeed(0.0f);
					mOwner->SetPosition(position + offset);
				}
			}

		}

		std::vector<Door* > doooors = g->GetDoorsInARoom(g->currentRoom);

		for (size_t i = 0; i < doooors.size(); i++)
		{
			Vector2 offset = Vector2::Zero;
			CollisionComponent* dorColl = doooors[i]->GetComponent<CollisionComponent>();
			CollSide side = linkCollision->GetMinOverlap(dorColl, offset);
			if (side != CollSide::None)
			{
				if (enumDirection == PlayerDirection::Left && doooors[i]->GetDoorDirection() == DoorDirection::Left &&
					doooors[i]->GetDoorState() == DoorState::Locked && g->keyCount > 0)
				{
					doooors[i]->SetDoorState(DoorState::Open);
					g->keyCount--;
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/DoorOpen.wav"), 0);
				}
				else if (enumDirection == PlayerDirection::Right && doooors[i]->GetDoorDirection() == DoorDirection::Right &&
					doooors[i]->GetDoorState() == DoorState::Locked && g->keyCount > 0)
				{
					doooors[i]->SetDoorState(DoorState::Open);
					g->keyCount--;
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/DoorOpen.wav"), 0);
				}
				else if (enumDirection == PlayerDirection::Down && doooors[i]->GetDoorDirection() == DoorDirection::Down &&
					doooors[i]->GetDoorState() == DoorState::Locked && g->keyCount > 0)
				{
					doooors[i]->SetDoorState(DoorState::Open);
					g->keyCount--;
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/DoorOpen.wav"), 0);
				}
				else if (enumDirection == PlayerDirection::Up && doooors[i]->GetDoorDirection() == DoorDirection::Up &&
					doooors[i]->GetDoorState() == DoorState::Locked && g->keyCount > 0)
				{
					doooors[i]->SetDoorState(DoorState::Open);
					g->keyCount--;
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/DoorOpen.wav"), 0);
				}
				
			}
		}

		for (size_t i = 0; i < doooors.size(); i++)
		{
			Vector2 offset = Vector2::Zero;
			CollisionComponent* dorColl = doooors[i]->GetComponent<CollisionComponent>();
			CollSide side = linkCollision->GetMinOverlap(dorColl, offset);
			if (side != CollSide::None)//if colliding with a door
			{
				if (enumDirection == PlayerDirection::Left && doooors[i]->GetDoorDirection() == DoorDirection::Left &&
					doooors[i]->GetDoorState() == DoorState::Open)
				{
					Vector2 position = mOwner->GetPosition();
					Vector2 teleport = Vector2(-125, 0);
					mOwner->SetPosition(position + teleport);

					std::vector<Actor* > vect = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect.size(); p++)
					{
						vect[p]->SetState(ActorState::Paused);
					}
					g->currentRoom = doooors[i]->GetDestination();

					std::vector<Actor* > vect2 = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect2.size(); p++)
					{
						vect2[p]->SetState(ActorState::Active);
					}
				}
				else if (enumDirection == PlayerDirection::Right && doooors[i]->GetDoorDirection() == DoorDirection::Right &&
					doooors[i]->GetDoorState() == DoorState::Open)
				{
					Vector2 position = mOwner->GetPosition();
					Vector2 teleport = Vector2(125, 0);
					mOwner->SetPosition(position + teleport);

					std::vector<Actor* > vect = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect.size(); p++)
					{
						vect[p]->SetState(ActorState::Paused);
					}
					g->currentRoom = doooors[i]->GetDestination();

					std::vector<Actor* > vect2 = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect2.size(); p++)
					{
						vect2[p]->SetState(ActorState::Active);
					}
				}
				else if (enumDirection == PlayerDirection::Up && doooors[i]->GetDoorDirection() == DoorDirection::Up &&
					doooors[i]->GetDoorState() == DoorState::Open)
				{
					Vector2 position = mOwner->GetPosition();
					Vector2 teleport = Vector2(0, -125);
					mOwner->SetPosition(position + teleport);

					std::vector<Actor* > vect = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect.size(); p++)
					{
						vect[p]->SetState(ActorState::Paused);
					}
					g->currentRoom = doooors[i]->GetDestination();

					std::vector<Actor* > vect2 = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect2.size(); p++)
					{
						vect2[p]->SetState(ActorState::Active);
					}
				}
				else if (enumDirection == PlayerDirection::Down && doooors[i]->GetDoorDirection() == DoorDirection::Down &&
					doooors[i]->GetDoorState() == DoorState::Open)
				{
					Vector2 position = mOwner->GetPosition();
					Vector2 teleport = Vector2(0, 125);
					mOwner->SetPosition(position + teleport);
					//pause all enemies in previous
					std::vector<Actor* > vect = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect.size(); p++)
					{
						vect[p]->SetState(ActorState::Paused);
					}
					g->currentRoom = doooors[i]->GetDestination();

					std::vector<Actor* > vect2 = g->GetActorsInARoom(g->currentRoom);
					for (size_t p = 0; p < vect2.size(); p++)
					{
						vect2[p]->SetState(ActorState::Active);
					}
				}
			}
		}


		std::vector<Collider* > colliderVec = g->GetColliderVector();

		for (size_t i = 0; i < colliderVec.size(); i++)
		{
			Vector2 offset = Vector2::Zero;
			CollSide side = linkCollision->GetMinOverlap(colliderVec[i]->GetCollisionComponent(), offset);
			if (side != CollSide::None)
			{
				Vector2 position = mOwner->GetPosition();
				SetForwardSpeed(0.0f);
				mOwner->SetPosition(position + offset);
				
			}
			
		}
		pos = mOwner->GetPosition();
		//was having issues with floating point precision where camera would jitter by 1 pixel
		//but it stopped happening, idk why
		//float yPos = pos.y - g->GetGameHeight() / 2;//camera position
		//yPos -= 0.1f;
		//char buffer[64];
		//int ret = snprintf(buffer, sizeof buffer, "%f", pos.y - yPos);
		//SDL_Log(buffer);
		g->SetCamera(Vector2(pos.x - g->GetGameWidth() / 2, pos.y - g->GetGameHeight() / 2));

		
		if ( visited[g->currentRoom] != g->currentRoom)//if first time going into that room
		{
			visited.insert({ g->currentRoom, g->currentRoom });
			g->TriggerSpawners(g->currentRoom);
		}

		UpdateSword();
		
	}

	void ChooseAnimation(std::string s)
	{
		mOwner->GetComponent<AnimatedSprite>()->SetAnimation(s);
	}

	void UpdateSword()
	{
		SpriteComponent* swordSprite = sword->GetComponent<SpriteComponent>();
		if (attackTimer <= 0.0f)
		{
			
			swordSprite->SetIsVisible(false);//set to invisible if not attacking
		}
		else
		{
			swordSprite->SetIsVisible(true);
			Vector2 playerPos = mOwner->GetPosition();
			if (enumDirection == PlayerDirection::Up)
			{
				swordSprite->SetTexture(g->GetTexture("Assets/Sword/Up.png"));
				sword->SetPosition(playerPos + Vector2(-8, -16));
			}
			else if (enumDirection == PlayerDirection::Down)
			{
				swordSprite->SetTexture(g->GetTexture("Assets/Sword/Down.png"));
				sword->SetPosition(playerPos + Vector2(8, 16));
			}
			if (enumDirection == PlayerDirection::Left)
			{
				swordSprite->SetTexture(g->GetTexture("Assets/Sword/Left.png"));
				sword->SetPosition(playerPos + Vector2(-16, 4));
			}
			if (enumDirection == PlayerDirection::Right)
			{
				swordSprite->SetTexture(g->GetTexture("Assets/Sword/Right.png"));
				sword->SetPosition(playerPos + Vector2(16, 4));
			}
		}
	}

	int keyCount;

private:
	bool ded;

	bool mSpacePressed;

	bool pushingSBlock;

	unsigned int lastDmgTaken;

	Vector2 direction;
	Game* g;

	std::unordered_map<std::string, std::string> visited;

	PlayerDirection enumDirection;

	Sword* sword;
	float attackTimer;
	
};