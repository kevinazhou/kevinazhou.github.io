#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"



class SecretBlock : public Actor
{
public:
	SecretBlock::SecretBlock(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this);
		
		collision = new CollisionComponent(this);
		Setup();
		initiated = false;
		fullyPushed = false;
	}

	void Setup();

	CollSide direction;
	Vector2 initialPos;
	bool initiated;
	bool fullyPushed;
private:
	SpriteComponent* sprite;
	CollisionComponent* collision;


	
};