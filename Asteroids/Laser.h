#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Asteroid.h"

class Laser : public Actor
{
public:
	Laser::Laser(class Game* game)
		:Actor(game)
	{
		sprite = new SpriteComponent(this, 100);
		move = new MoveComponent(this);
		move->SetForwardSpeed(400.0f);
		class Game* g = GetGame();
		sprite->SetTexture(g->GetTexture("Assets/Laser.png"));
		birth = SDL_GetTicks();
	}

	void OnUpdate(float deltaTime)
	{
		if (SDL_GetTicks() - birth > 1000)
		{
			SetState(ActorState::Destroy);
		}
		for (size_t i = 0; i < mGame->AsteroidVector.size(); i++)
		{
			Vector2 pos1 = mGame->AsteroidVector[i]->GetPosition();
			Vector2 pos2 = GetPosition();
			float xD = (pos1.x - pos2.x) * (pos1.x - pos2.x);
			float yD = (pos1.y - pos2.y) * (pos1.y - pos2.y);
			if (xD + yD <= 4900)
			{
				SetState(ActorState::Destroy);
				mGame->AsteroidVector[i]->SetState(ActorState::Destroy);
			}
		}
		
	}


private:
	SpriteComponent* sprite;
	MoveComponent* move;

	unsigned int birth;

};
