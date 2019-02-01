#pragma once
#include "Actor.h"
#include "BallMove.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "CollisionComponent.h"

class Ball : public Actor
{
public:
	Ball::Ball(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this, 110);
		Game* ga = GetGame();
		sprite->SetTexture(ga->GetTexture("Assets/Ball.png"));
		ballsize = sprite->GetTexWidth();
		move = new BallMove(this);
		collision = new CollisionComponent(this);
		collision->SetSize(20, 20);
	}

	int getBallSize()
	{
		return ballsize;
	}

	int GetWidth()
	{
		return sprite->GetTexWidth();
	}

	int GetHeight()
	{
		return sprite->GetTexHeight();
	}


private:
	SpriteComponent* sprite;
	BallMove* move;
	CollisionComponent* collision;
	int ballsize;

};