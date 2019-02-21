#pragma once
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"

class Collider : public Actor
{
public:
	Collider::Collider(Game* g)
		:Actor(g)
	{
		collision = new CollisionComponent(this);
	}

	Collider::~Collider()
	{

	}

	void SetCollisionDimensions(int width, int height)
	{
		collision->SetSize(width, height);
	}

	CollisionComponent* GetCollisionComponent()
	{
		return collision;
	}

private:
	CollisionComponent* collision;
};