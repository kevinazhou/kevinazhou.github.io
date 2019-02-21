#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "Game.h"

class Sword: public Actor
{
public:
	Sword::Sword(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this);
		sprite->SetTexture(mGame->GetTexture("Assets/Sword/Up.png"));
		collision = new CollisionComponent(this);
		collision->SetSize(25, 25);
	}

private:
	SpriteComponent* sprite;
	CollisionComponent* collision;
};