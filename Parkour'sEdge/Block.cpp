#include "Block.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "CollisionComponent.h"

Block::Block(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	SetScale(64.0f);
	mesh = new MeshComponent(this);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	collision = new CollisionComponent(this);
	collision->SetSize(1, 1, 1);
	GetGame()->AddBlock(this);
}

Block::~Block()
{
	GetGame()->RemoveBlock(this);
}

void Block::OnUpdate(float deltaTime)
{
	if (isRotating)
	{
		mRotation += (Math::PiOver2) / 2 * deltaTime;
	}
}
