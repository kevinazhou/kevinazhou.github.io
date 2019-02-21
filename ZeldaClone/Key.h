#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include "Collectible.h"

class Key : public Actor
{
public:
	Key::Key(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this);
		collision = new CollisionComponent(this);
		collect = new Collectible(this);
		collect->SetOnCollect([this] {
			mGame->keyCount += 1;
		});
		Setup();
	}

	void Setup();
private:
	SpriteComponent* sprite;
	CollisionComponent* collision;
	Collectible* collect;
};
