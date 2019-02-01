#pragma once
#include "Actor.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"
#include <string>



class Player : public Actor
{
public:
	Player::Player(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this, 200);
		sprite->SetTexture(mGame->GetTexture("Assets/Mario/Idle.png"));
		collision = new CollisionComponent(this);
		collision->SetSize(32, 32);
		move = new PlayerMove(this);
		//mGame->AddBlock(this);
	}

	Player::~Player()
	{
		//mGame->RemoveBlock(this);
	}

	void changeTexture(SDL_Texture* tex)
	{
		Game* g = GetGame();
		sprite->SetTexture(tex);
	}




private:
	SpriteComponent* sprite;
	CollisionComponent* collision;
	PlayerMove* move;
};