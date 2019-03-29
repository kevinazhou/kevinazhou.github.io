#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "PlayerUI.h"

Player::Player(class Game* game)
	:Actor(game)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Kart.gpmesh"));
	mScale = 0.75f;
	move = new PlayerMove(this);
	camera = new CameraComponent(this);
	camera->SnapToIdeal();
	ui = new PlayerUI(this);
}