#pragma once
#include "MoveComponent.h"
#include "Ball.h"
#include "Game.h"
#include "Paddle.h"
#include "CollisionComponent.h"
//#include "Math.h"

class BallMove : MoveComponent
{
public:
	BallMove::BallMove(class Actor* a)
		:MoveComponent(a)
	{
		velocity = Vector2(250, -250);
		g = mOwner->GetGame();
		p = g->GetPaddle();
		LeftNormal = Vector2(cos(-0.55*M_PI), sin(-0.55*M_PI));
		RightNormal = Vector2(cos(-0.45*M_PI), sin(-0.45*M_PI));
		lastPadBounce = 0;
	}

	void Update(float deltaTime)
	{
		Vector2 pos = mOwner->GetPosition();
		Vector2 dis = (velocity)*deltaTime;
		pos = pos + dis;
		mOwner->SetPosition(pos);

		unsigned int currTime = SDL_GetTicks();

			if (pos.x > 1024 - 32 - 10)//ball width
			{
				mOwner->SetPosition(Vector2(1024 - 32 - 10, pos.y));
				velocity = Vector2(-velocity.x, velocity.y);
			}
			if (pos.x < 32 + 10)
			{
				mOwner->SetPosition(Vector2(32 + 10, pos.y));
				velocity = Vector2(-velocity.x, velocity.y);
			}

			if (pos.y < 32 + 10)
			{
				mOwner->SetPosition(Vector2(pos.x, 32 + 10));
				velocity = Vector2(velocity.x, -velocity.y);
			}

			if (pos.y > 768 - 10)
			{
				mOwner->SetPosition(Vector2(512, 700));
				velocity = Vector2(250, -250);
			}
		


		CollisionComponent* padCollision = p->GetComponent<CollisionComponent>();
		CollisionComponent* ballCollision = mOwner->GetComponent<CollisionComponent>();
		Vector2 PadPos = p->GetPosition();
		int PadWidth = p->GetWidth();
		int offset = (PadWidth / 2) - (PadWidth / 3);
		
		if (currTime - lastPadBounce > 200)
		{
			if (padCollision->Intersect(ballCollision))
			{
				lastPadBounce = currTime;
				if (pos.x < (PadPos.x - offset))
				{
					velocity = Vector2::Reflect(velocity, LeftNormal);
				}
				else if (pos.x > (PadPos.x + offset))
				{
					velocity = Vector2::Reflect(velocity, RightNormal);
				}
				else
				{
					velocity.y = velocity.y * -1;
				}
			}
		}

		Vector2 Temp = Vector2::Zero;
		for (size_t i = 0; i < g->BlockVector.size(); i++)
		{
			CollSide side = ballCollision->GetMinOverlap(g->BlockVector[i]->GetComponent<CollisionComponent>(), Temp);
			if ( side == CollSide::Top || side == CollSide::Bottom)
			{
				g->BlockVector[i]->SetState(ActorState::Destroy);
				velocity = Vector2(velocity.x, -velocity.y);
				break;
			}
			else if (side == CollSide::Left || side == CollSide::Right)
			{
				g->BlockVector[i]->SetState(ActorState::Destroy);
				velocity = Vector2(-velocity.x, velocity.y);
				break;
			}
		}
	}

private:
	unsigned int lastPadBounce;
	Vector2 velocity;
	Game* g;
	Paddle* p;

	Vector2 LeftNormal;
	Vector2 RightNormal;
};