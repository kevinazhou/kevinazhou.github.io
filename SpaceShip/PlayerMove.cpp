#include "PlayerMove.h"
#include "Player.h"
#include "Renderer.h"
#include "SideBlock.h"
#include "Random.h"
#include "Bullet.h"
#include "Block.h"
//#include "CollisionComponent.h"


PlayerMove::PlayerMove(class Actor* owner)
	:MoveComponent(owner)
{
	g = mOwner->GetGame();
	velocity = Vector3(400, 0, 0);

	spacePressed = false;
	speedMultiplier = 1.0f;
	EveryTwentySec = SDL_GetTicks();

	for (int i = 0; i * 500 < mOwner->GetPosition().x + 3000; i++)
	{
		SideBlock* side1 = new SideBlock(g);
		side1->SetPosition(Vector3(i * 500, 500, 0));
		SideBlock* side2 = new SideBlock(g);
		side2->SetPosition(Vector3(i * 500, -500, 0));

		
		if (i % 4 == 0 || i % 4 == 2)
		{
			side1->SetTexIndex(2);
			side2->SetTexIndex(2);
		}
		else if (i % 4 == 1)
		{
			side1->SetTexIndex(3);
			side2->SetTexIndex(3);
		}
		else if (i % 4 == 3)
		{
			side1->SetTexIndex(4);
			side2->SetTexIndex(4);
		}

		lastSideBlockX = i * 500;
		sidesCreated = i + 1;
	}

	for (int i = 1; i * 1000 < mOwner->GetPosition().x + 3000; i++)
	{
		std::string file = ".txt";
		file.insert(0, std::to_string(i));
		file.insert(0, "Assets/Blocks/");
		g->LoadBlocks(file, i * 1000.0f);

		lastBlockX = i * 1000;
		blocksCreated = i + 1;
	}

}

void PlayerMove::Update(float deltaTime)
{
	trueVelocity = velocity * speedMultiplier;
	Vector3	change = trueVelocity * deltaTime;
	change += input * deltaTime;

	Vector3 newPos = mOwner->GetPosition() + change;
	if (newPos.y < -195)
	{
		newPos.y = -195;
	}
	else if (newPos.y > 195)
	{
		newPos.y = 195;
	}

	if (newPos.z < -225)
	{
		newPos.z = -225;
	}
	if (newPos.z > 225)
	{
		newPos.z = 225;
	}
	mOwner->SetPosition(newPos);

	Vector3 cameraPos = mOwner->GetPosition() - (mOwner->GetForward() * HDist)
		+ (Vector3::UnitZ * VDist);

	Vector3 targetPos = mOwner->GetPosition() + (mOwner->GetForward() * TargetDist);

	Matrix4 lookMatrix = Matrix4::CreateLookAt(cameraPos, targetPos, Vector3::UnitZ);

	mOwner->GetGame()->GetRenderer()->SetViewMatrix(lookMatrix);

	CollisionComponent* col = mOwner->GetComponent<CollisionComponent>();

	std::vector<Block*> blocks = mOwner->GetGame()->GetBlockVec();
	for (size_t i = 0; i < blocks.size(); i++)
	{
		if (col->Intersect(blocks[i]->GetComponent<CollisionComponent>()))
		{
			mOwner->SetState(ActorState::Destroy);
			Mix_HaltChannel(g->shipSound);
			Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/ShipDie.wav"), 0);
		}
	}

	//spawning sideblocks
	if (lastSideBlockX - mOwner->GetPosition().x < 2500)
	{
		SideBlock* side1 = new SideBlock(g);
		side1->SetPosition(Vector3(lastSideBlockX + 500, 500, 0));
		SideBlock* side2 = new SideBlock(g);
		side2->SetPosition(Vector3(lastSideBlockX + 500, -500, 0));
		
		if (sidesCreated % 10 == 0)
		{//blue sides every 10 sides
			side1->SetTexIndex(1);
			side2->SetTexIndex(1);
		}
		else if (sidesCreated % 4 == 0 || sidesCreated % 4 == 2)
		{
			side1->SetTexIndex(2);
			side2->SetTexIndex(2);
		}
		else if (sidesCreated % 4 == 1)
		{
			side1->SetTexIndex(3);
			side2->SetTexIndex(3);
		}
		else if (sidesCreated % 4 == 3)
		{
			side1->SetTexIndex(4);
			side2->SetTexIndex(4);
		}
		
		lastSideBlockX += 500;
		sidesCreated++;
	}

	if (lastBlockX - mOwner->GetPosition().x < 2000)
	{
		std::string file = ".txt";
		if (blocksCreated <= 20)
		{
			file.insert(0, std::to_string(blocksCreated));
		}
		else
		{//randomly choose block configurations after 20 of them
			int fileNum = Random::GetIntRange(1, 20);
			file.insert(00, std::to_string(fileNum));
		}
		file.insert(0, "Assets/Blocks/");

		g->LoadBlocks(file, lastBlockX + 1000.0f);

		lastBlockX += 1000.0f;
		blocksCreated++;
	}


	if (SDL_GetTicks() - EveryTwentySec > 20000)
	{//speed up every 20 sec
		speedMultiplier += 0.15f;
		EveryTwentySec = SDL_GetTicks();
	}
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE])
	{
		if (!spacePressed)
		{
			//create a bullet
			Bullet* bullet = new Bullet(mOwner->GetGame());
			bullet->SetPosition(mOwner->GetPosition());
			Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Shoot.wav"), 0);
		}
		spacePressed = true;
	}
	else
	{
		spacePressed = false;
	}

	input = Vector3::Zero;
	if (keyState[SDL_SCANCODE_W] && !keyState[SDL_SCANCODE_S])
	{
		input.z += (300 * speedMultiplier);
	}
	else if (keyState[SDL_SCANCODE_S] && !keyState[SDL_SCANCODE_W])
	{
		input.z -= (300 * speedMultiplier);
	}

	if (keyState[SDL_SCANCODE_D] && !keyState[SDL_SCANCODE_A])
	{
		input.y += (300 * speedMultiplier);
	}
	else if (keyState[SDL_SCANCODE_A] && !keyState[SDL_SCANCODE_D])
	{
		input.y -= (300 * speedMultiplier);
	}
}

