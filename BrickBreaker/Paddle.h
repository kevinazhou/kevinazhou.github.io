#pragma once
#include "Actor.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CollisionComponent.h"

class Paddle : public Actor
{
public:
	Paddle::Paddle(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this, 100);
		Game* ga = GetGame();
		sprite->SetTexture(ga->GetTexture("Assets/Paddle.png"));

		move = new MoveComponent(this);
		collision = new CollisionComponent(this);
		collision->SetSize(104, 24);
	}

	int GetWidth()
	{
		return sprite->GetTexWidth();
	}

	int GetHeight()
	{
		return sprite->GetTexHeight();
	}

	void OnProcessInput(const Uint8* keyState)
	{
		if (keyState[SDL_SCANCODE_LEFT])
		{
			move->SetForwardSpeed(-350.0f);
		}
		if (keyState[SDL_SCANCODE_RIGHT])
		{
			move->SetForwardSpeed(350.0f);
		}
	}

	void OnUpdate(float deltaTime)
	{
		Vector2 pos = GetPosition();
		int width = sprite->GetTexWidth();
		if (pos.x < 32 + (width/2))
		{
			SetPosition(Vector2(32 + (width/2), pos.y));
		}
		if (pos.x > 1024-32-(width/2))
		{
			SetPosition(Vector2(1024 - 32 - (width / 2), pos.y));
		}
		move->SetForwardSpeed(0.0f);
	}



private:
	SpriteComponent* sprite;
	MoveComponent* move;
	CollisionComponent* collision;
};
