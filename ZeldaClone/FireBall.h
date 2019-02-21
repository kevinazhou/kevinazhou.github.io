#pragma once
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Player.h"

class FireBall : public Actor
{
public:
	FireBall::FireBall(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this);
		sprite->SetTexture(mGame->GetTexture("Assets/FireballRed.png"));
		move = new MoveComponent(this);
		move->SetForwardSpeed(200.0f);
		collision = new CollisionComponent(this);
		collision->SetSize(16, 16);

		birth = SDL_GetTicks();
	}

	void OnUpdate(float deltaTime)
	{
		unsigned int curr = SDL_GetTicks();
		if (curr - birth >= 2000)
		{
			SetState(ActorState::Destroy);
		}

		CollisionComponent* playerColl = mGame->GetPlayer()->GetComponent<CollisionComponent>();
		Vector2 offset = Vector2::Zero;
		CollSide side = collision->GetMinOverlap(playerColl, offset);
		if (side != CollSide::None)
		{
			mGame->PlayerTakeDmg(1);
			SetState(ActorState::Destroy);
		}
	}

private:
	SpriteComponent* sprite;
	MoveComponent* move;
	CollisionComponent* collision;
	unsigned int birth;
};