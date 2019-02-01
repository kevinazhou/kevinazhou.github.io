#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Random.h"

class Asteroid : public Actor
{
public:
	Asteroid::Asteroid(class Game* game)
		:Actor(game)
	{
		sprite = new SpriteComponent(this, 100);
		move = new MoveComponent(this);
		move->SetForwardSpeed(150.0f);
		SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));
		Vector2 min = Vector2(0, 0);
		Vector2 max = Vector2(1024, 768);
		SetPosition(Random::GetVector(min, max));
		class Game* g = GetGame();
		sprite->SetTexture(g->GetTexture("Assets/Asteroid.png"));
		mGame->AddAsteroid(this);
	}

	Asteroid::~Asteroid()
	{
		mGame->RemoveActor(this);
	}

	void OnUpdate(float deltaTime)
	{
		Vector2 pos = GetPosition();
		if (pos.x < 0)
		{
			Vector2 pos1 = Vector2(1022, pos.y);
			SetPosition(pos1);
		}
		if (pos.x > 1024)
		{
			Vector2 pos2 = Vector2(2, pos.y);
			SetPosition(pos2);
		}
		if (pos.y < 0)
		{
			Vector2 pos3 = Vector2(pos.x, 766);
			SetPosition(pos3);
		}
		if (pos.y > 768)
		{
			Vector2 pos4 = Vector2(pos.x, 2);
			SetPosition(pos4);
		}

	}
private:
	SpriteComponent* sprite;
	MoveComponent* move;
};