#include "Bullet.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"
#include "Block.h"
#include "Player.h"
#include "PlayerMove.h"


Bullet::Bullet(class Game* game)
	:Actor(game)
{
	mScale = 0.5f;

	mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));

	move = new MoveComponent(this);

	PlayerMove* playMove = mGame->GetPlayer()->GetComponent<PlayerMove>();
	move->SetForwardSpeed(playMove->trueVelocity.x + 500);

	collision = new CollisionComponent(this);
	collision->SetSize(10, 10, 10);

	creationTime = SDL_GetTicks();
}

void Bullet::OnUpdate(float deltaTime)
{
	if (SDL_GetTicks() - creationTime > 1000)
	{
		SetState(ActorState::Destroy);
	}

	std::vector<Block*> blockVec = mGame->GetBlockVec();
	for (size_t i = 0; i < blockVec.size(); i++)
	{
		if (collision->Intersect(blockVec[i]->GetComponent<CollisionComponent>()))
		{
			SetState(ActorState::Destroy);
			if (blockVec[i]->isExploding)
			{
				blockVec[i]->Explode();//should set off chain reaction
				Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/BlockExplode.wav"), 0);
			}

		}

	}
}