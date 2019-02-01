#pragma once
#include "Actor.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include <string>



class Block : public Actor
{
public:
	Block::Block(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this, 100);
		collision = new CollisionComponent(this);
		collision->SetSize(32, 32);
		mGame->AddBlock(this);
	}

	Block::~Block()
	{
		mGame->RemoveBlock(this);
	}

	void changeTexture(SDL_Texture* tex)
	{
		Game* g = GetGame();
		sprite->SetTexture(tex);
	}



	CollisionComponent* collision;
private:
	SpriteComponent* sprite;

};