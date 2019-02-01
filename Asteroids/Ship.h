#pragma once
#include "Actor.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Laser.h"

class Ship : public Actor
{
public:
	Ship::Ship(class Game* game)
		:Actor(game)
	{
		sprite = new SpriteComponent(this, 100);
		move = new MoveComponent(this);
		class Game* g = GetGame();
		sprite->SetTexture(g->GetTexture("Assets/Ship.png"));
	}

	void OnProcessInput(const Uint8* keyState)
	{
		if (keyState[SDL_SCANCODE_UP])
		{
			if (move->GetForwardSpeed() < 300)
			{
				move->SetForwardSpeed(move->GetForwardSpeed() + 10);
			}
		}
		if (keyState[SDL_SCANCODE_DOWN])
		{
			if (move->GetForwardSpeed() > - 300)//max speed
			{
				move->SetForwardSpeed(move->GetForwardSpeed() - 10);
			}
		}
		if (keyState[SDL_SCANCODE_LEFT])
		{
				move->SetAngularSpeed(move->GetAngularSpeed() + .2f);
		}
		if (keyState[SDL_SCANCODE_RIGHT])
		{
				move->SetAngularSpeed(move->GetAngularSpeed() - .2f);
		}
		if (keyState[SDL_SCANCODE_SPACE])
		{
			if (SDL_GetTicks() - lastLaser > 1000)
			{
				Laser* l = new Laser(mGame);
				l->SetPosition(GetPosition());
				l->SetRotation(GetRotation());
				lastLaser = SDL_GetTicks();
			}

		}
		
		if (move->GetForwardSpeed() != 0)
		{
			class Game* g = GetGame();
			sprite->SetTexture(g->GetTexture("Assets/ShipThrust.png"));
		}
		if (move->GetForwardSpeed() == 0)
		{
			class Game* g = GetGame();
			sprite->SetTexture(g->GetTexture("Assets/Ship.png"));
		}
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
	
	unsigned int lastLaser;


};