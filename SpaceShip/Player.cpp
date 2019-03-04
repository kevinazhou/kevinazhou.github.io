#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PlayerMove.h"

Player::Player(class Game* game)
	:Actor(game)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Ship.gpmesh"));
	collision = new CollisionComponent(this);
	collision->SetSize(100, 40, 60);
	move = new PlayerMove(this);
}