#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "Goomba.h"
//#include "Math.h"

class PlayerMove : MoveComponent
{
public:
	PlayerMove::PlayerMove(class Actor* a)
		:MoveComponent(a)
	{
		g = mOwner->GetGame();
		mYSpeed = 0.0f;
		mSpacePressed = false;
		mInAir = false;
		NotPastStart = true;
		ded = false;
	}

	void ProcessInput(const Uint8* keyState)
	{
		if (keyState[SDL_SCANCODE_LEFT])
		{
			SetForwardSpeed(-300.0f);
		}
		else if (keyState[SDL_SCANCODE_RIGHT])
		{
			SetForwardSpeed(300.0f);
		}
		else
		{
			SetForwardSpeed(0.0f);
		}

		if (keyState[SDL_SCANCODE_SPACE])
		{
			if (!mSpacePressed)//when this frame is the leading edge
			{
				if (!mInAir)
				{
					mYSpeed = -700.0f;
					mSpacePressed = true;
					mInAir = true;
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Jump.wav"), 0);
				}

			}

		}
		else
		{
			if (mSpacePressed)//trailing edge
			{

			}
			mSpacePressed = false;//when space is not pressed this fram
		}
	}

	void Update(float deltaTime)
	{
		Vector2 pos = mOwner->GetPosition();
		Vector2 c = g->GetCamera();
		Vector2 change = Vector2(GetForwardSpeed()*deltaTime, mYSpeed * deltaTime);
		pos = pos + change;
		if (pos.x < c.x + 14)
		{
			pos.x = c.x + 14;
		}
		mOwner->SetPosition(pos);
		
		

		//if (pos.x < c.x)//not working
		//{
		//	mOwner->SetPosition(Vector2(c.x, pos.y));
		//}
		if (pos.x > 6368.0f)
		{
			Mix_HaltChannel(g->GetChannel());
			Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/StageClear.wav"), 0);
			mOwner->SetState(ActorState::Paused);
			return;
		}

		if (pos.y > 466.0f)
		{

			ded = true;
			mOwner->SetState(ActorState::Paused);
		}

		CollisionComponent* marioCollision = mOwner->GetComponent<CollisionComponent>();

		bool Colliding = false;

		Vector2 offset = Vector2::Zero;
		for (size_t i = 0; i < g->BlockVector.size(); i++)
		{
			CollSide side = marioCollision->GetMinOverlap(g->BlockVector[i]->collision, offset);
			if (side != CollSide::None)
			{
				if (side == CollSide::Top && mYSpeed > 0.0f)
				{
					mYSpeed = 0.0f;
					mOwner->SetPosition(pos + offset);
					mInAir = false;
					Colliding = true;
				}
				else if (side == CollSide::Bottom && mYSpeed < 0.0f)
				{
					mYSpeed = 0.0f;
					mOwner->SetPosition(pos + offset);
					//Colliding = true;
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Bump.wav"), 0);
				}

				else if (side == CollSide::Left)
				{
					if (offset.x != 0)
					{
						SetForwardSpeed(0.0f);
						pos.x += offset.x;
						mOwner->SetPosition(pos);

						//mOwner->SetPosition(pos + offset); this doesnt work ???? was working fine for collisions in the air
						//Colliding = true;
					}
				}
				else if (side == CollSide::Right)
				{
					if (offset.x != 0)
					{
						SetForwardSpeed(0.0f);
						pos.x += offset.x;
						mOwner->SetPosition(pos);

						//mOwner->SetPosition(pos + offset);
						//Colliding = true;
					}

				}
			}
			offset = Vector2::Zero;
		}

		if (!Colliding)//if not colliding, then in the air
		{
			mInAir = true;
		}
		else
		{
			mInAir = false;
		}

		for (size_t i = 0; i < g->GoombaVector.size(); i++)
		{
			CollSide side = marioCollision->GetMinOverlap(g->GoombaVector[i]->collision, offset);
			if (side != CollSide::None)
			{
				bool stomped = false;
				if (side == CollSide::Top)
				{
					g->GoombaVector[i]->SetStomped(true);
					g->GoombaVector[i]->StompTime = SDL_GetTicks();
					mYSpeed = -350.0f;
					mInAir = true;
					stomped = true;
				}
				else if (side == CollSide::Left && mInAir)//left side of the block
				{
					g->GoombaVector[i]->SetStomped(true);
					g->GoombaVector[i]->StompTime = SDL_GetTicks();
					mYSpeed = -350.0f;
					mInAir = true;
					stomped = true;
				}
				else if (side == CollSide::Right && mInAir)
				{
					g->GoombaVector[i]->SetStomped(true);
					g->GoombaVector[i]->StompTime = SDL_GetTicks();
					mYSpeed = -350.0f;
					mInAir = true;
					stomped = true;
				}
				else//mario dies
				{
					ded = true;
					mOwner->SetState(ActorState::Paused);
				}

				if (stomped)
				{
					Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Stomp.wav"), 0);
				}
			}
		}

		mYSpeed += 2000.0f * deltaTime;

		if (!ded)//if not dead
		{
			if (!mInAir)//on the ground
			{
				if (GetForwardSpeed() > 0.0f)
				{
					ChooseAnimation("runRight");
				}
				else if (GetForwardSpeed() < 0.0f)
				{
					ChooseAnimation("runLeft");
				}
				else
				{
					ChooseAnimation("idle");
				}
			}
			else//in the air
			{
				if (GetForwardSpeed() > 0.0f)
				{
					ChooseAnimation("jumpRight");
				}
				else if (GetForwardSpeed() < 0.0f)
				{
					ChooseAnimation("jumpLeft");
				}
				else
				{
					std::string name = mOwner->GetComponent<AnimatedSprite>()->GetAnimName();
					if (name == "runRight" || name == "idle" || name == "jumpRight")
					{
						ChooseAnimation("jumpRight");
					}
					else
					{
						ChooseAnimation("jumpLeft");
					}
				}
			}
		}
		else
		{
			ChooseAnimation("dead");
			Mix_HaltChannel(g->GetChannel());
			Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Dead.wav"), 0);
		}
		

		if (mOwner->GetPosition().x < 300 && NotPastStart)//starting camera
		{
			g->SetCamera(Vector2(0, c.y));
		}
		else
		{
			NotPastStart = false;
			if (!(mOwner->GetPosition().x - 300 < c.x))//don't decrease camera.y
			{
				g->SetCamera(Vector2(mOwner->GetPosition().x - 300, c.y));
			}

		}

	}

	void ChooseAnimation(std::string s)
	{
		mOwner->GetComponent<AnimatedSprite>()->SetAnimation(s);
	}

private:
	bool ded;
	bool NotPastStart;
	float mYSpeed;
	bool mSpacePressed;
	bool mInAir;
	Game* g;

};