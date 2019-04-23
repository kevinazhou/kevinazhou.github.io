#include "Player.h"
#include "MeshComponent.h"
#include "Game.h"
#include "PlayerMove.h"
#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "HUD.h"

Player::Player(class Game* game, Actor* parent)
	:Actor(game, parent)
{


	move = new PlayerMove(this);
	camera = new CameraComponent(this);
	collision = new CollisionComponent(this);
	collision->SetSize(50, 175, 50);

	hud = new HUD(this);
}