#include "EnemyComponent.h"
#include "Actor.h"
#include "Game.h"

EnemyComponent::EnemyComponent(Actor* owner)
	:Component(owner)
{
	owner->GetGame()->AddEnemy(owner);
	hitpoints = 1;
	collideDmg = 1;
	lastHit = 0;
	invincible = 0;
}

EnemyComponent::~EnemyComponent()
{
	mOwner->GetGame()->RemoveEnemy(mOwner);
}

void EnemyComponent::TakeDamage(int amount)
{

	unsigned int curr = SDL_GetTicks();

	if (curr - lastHit > invincible)
	{
		hitpoints -= amount;
		if (hitpoints <= 0)
		{
			mOnDeath();
			mOwner->SetState(ActorState::Destroy);


		}
		else
		{
			mOnTakeDamage();
		}

		lastHit = curr;
	}

	
}