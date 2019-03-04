#include "SideBlock.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Player.h"

SideBlock::SideBlock(class Game* game)
	:Actor(game)
{
	mScale = 500;

	mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	mesh->SetTextureIndex(2);
}

void SideBlock::SetTexIndex(int i)
{
	mesh->SetTextureIndex(i);
}

void SideBlock::OnUpdate(float deltaTime)
{
	Player* p = mGame->GetPlayer();
	Vector3 playerPos = p->GetPosition();
	Vector3 thisPos = GetPosition();
	if (playerPos.x - thisPos.x > 2000)
	{
		SetState(ActorState::Destroy);
	}
}