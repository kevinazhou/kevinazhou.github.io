#pragma once
#include "Component.h"
#include <functional>

class EnemyComponent : public Component
{
public:
	EnemyComponent(class Actor* owner);

	~EnemyComponent();

	void SetOnDeath(std::function<void()> onDeath)
	{
		mOnDeath = onDeath;
	}

	void SetOnTakeDamage(std::function<void()> onTakeDamage)
	{
		mOnTakeDamage = onTakeDamage;
	}

	void SetHitpoints(int amount)
	{
		hitpoints = amount;
	}

	void TakeDamage(int amount);

	void SetInvincible(unsigned int amount)
	{
		invincible = amount;
	}

	int GetHP()
	{
		return hitpoints;
	}

	int collideDmg;


	std::function<void()> mOnDeath;
	std::function<void()> mOnTakeDamage;
private:
	int hitpoints;
	
	unsigned int invincible;

	unsigned int lastHit;
	
};