#pragma once
#include "Actor.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"
#include "GoombaMove.h"
#include <string>



class Goomba : public Actor
{
public:
	Goomba::Goomba(Game* g)
		:Actor(g)
	{
		std::vector<SDL_Texture*> walkAnimation{
			GetGame()->GetTexture("Assets/Goomba/Walk0.png"),
			GetGame()->GetTexture("Assets/Goomba/Walk1.png")
		};
		std::vector<SDL_Texture*> ded{
			GetGame()->GetTexture("Assets/Goomba/Dead.png")
		};
		sprite = new AnimatedSprite(this, 150);
		sprite->AddAnimation("walk", walkAnimation);
		sprite->AddAnimation("dead", ded);
		sprite->SetAnimation("walk");
		collision = new CollisionComponent(this);
		collision->SetSize(32, 32);
		mGame->AddGoomba(this);
		move = new GoombaMove(this);
		stomped = false;
	}

	Goomba::~Goomba()
	{
		mGame->RemoveGoomba(this);
	}

	void changeTexture(SDL_Texture* tex)
	{
		Game* g = GetGame();
		sprite->SetTexture(tex);
	}

	void SetStomped(bool s)
	{
		stomped = s;
	}

	bool GetStomped()
	{
		return stomped;
	}

	unsigned int StompTime;

	CollisionComponent* collision;
private:
	AnimatedSprite* sprite;

	GoombaMove* move;

	bool stomped;
};