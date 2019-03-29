#include "Enemy.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "EnemyMove.h"

Enemy::Enemy(class Game* game)
	:Actor(game)
{
	SetScale(0.75f);
	mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));
	mesh->SetTextureIndex(6);

	move = new EnemyMove(this);
}