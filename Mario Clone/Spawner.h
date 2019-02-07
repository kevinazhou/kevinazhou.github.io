#pragma once
#include "Actor.h"
#include "Game.h"
#include "Goomba.h"
#include "Player.h"
#include <string>



class Spawner : public Actor
{
public:
	Spawner::Spawner(Game* g)
		:Actor(g)
	{
		//player = mGame->GetPlayer();
	}

	Spawner::~Spawner()
	{

	}

	void OnUpdate(float deltaTime)
	{
		Vector2 pos = GetPosition();
		Player* player = mGame->GetPlayer();
		Vector2 playerPos = player->GetPosition();
		
		if (abs(pos.x - playerPos.x) < 600.0f)
		{
			Goomba* goomb = new Goomba(mGame);
			goomb->SetPosition(pos);
			this->SetState(ActorState::Destroy);
		}

	}

private:
	//Player* player;
};