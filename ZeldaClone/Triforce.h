#pragma once
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "AnimatedSprite.h"
#include "Collectible.h"
#include "Player.h"

class Triforce : public Actor
{
public:
	Triforce::Triforce(Game* g)
		:Actor(g)
	{
		sprite = new AnimatedSprite(this);
		collision = new CollisionComponent(this);
		collect = new Collectible(this);
		collect->SetOnCollect([this] {
			Player* p = GetGame()->GetPlayer();
			p->SetState(ActorState::Paused);
			Mix_HaltChannel(GetGame()->GetChannel());
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Triforce.ogg"), 0);
		});
		Setup();
	}

	void Setup();

private:
	AnimatedSprite* sprite;
	CollisionComponent* collision;
	Collectible* collect;
};