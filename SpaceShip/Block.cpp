#include "Block.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include <vector>

Block::Block(class Game* game)
	:Actor(game)
{
	mScale = 25.0f;

	mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mesh->SetTextureIndex(2);

	collision = new CollisionComponent(this);
	collision->SetSize(1, 1, 1);

	mGame->AddBlock(this);

}

Block::~Block()
{
	mGame->RemoveBlock(this);
}

void Block::Explode()
{
	SetState(ActorState::Destroy);

	if (isExploding)
	{//keep the chain reaction going if this block is exploding block
		std::vector<Block*> blocks = mGame->GetBlockVec();
		Vector3 thisPos = GetPosition();
		for (size_t i = 0; i < blocks.size(); i++)
		{
			if (blocks[i] != this && blocks[i]->GetState() != ActorState::Destroy)
			{
				Vector3 otherPos = blocks[i]->GetPosition();
				Vector3 vectorFrom = thisPos - otherPos;
				if (vectorFrom.LengthSq() < 2500)//within 50 units
				{
					blocks[i]->Explode();
				}
			}

		}
	}
	
}

void Block::OnUpdate(float deltaTime)
{
	Player* p = mGame->GetPlayer();
	Vector3 playerPos = p->GetPosition();
	Vector3 thisPos = GetPosition();
	if (playerPos.x - thisPos.x > 2000)
	{
		SetState(ActorState::Destroy);
	}
}

void Block::SetExplodingTex()
{
	if (isExploding)
	{
		mesh->SetTextureIndex(4);
	}
}