#pragma once
#include "Component.h"
#include "SDL/SDL.h"
#include "Actor.h"
#include "Game.h"
#include "Player.h"
#include <functional>

class Collectible : public Component
{
public:
	// (Lower draw order corresponds with further back)
	Collectible(class Actor* owner, int updateOrder = 100)
		:Component(owner, updateOrder)
	{

	}
	~Collectible()
	{

	}

	void SetOnCollect(std::function<void()> onCollect)
	{
		mOnCollect = onCollect;
	}

	void Update(float deltaTime)
	{
		CollisionComponent* ownerColl = mOwner->GetComponent<CollisionComponent>();
		Game* g = mOwner->GetGame();
		Player* p = g->GetPlayer();
		CollisionComponent* playerColl = p->GetComponent<CollisionComponent>();

		Vector2 zero = Vector2::Zero;
		CollSide side = ownerColl->GetMinOverlap(playerColl, zero);
		if (CollSide::None != side)
		{
			mOwner->SetState(ActorState::Destroy);
			if (mOnCollect)
			{
				mOnCollect();
				Mix_PlayChannel(-1, g->GetSound("Assets/Sounds/Key.wav"), 0);
			}
		}

		
	}

	std::function<void()> mOnCollect;
};