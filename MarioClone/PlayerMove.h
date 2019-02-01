#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "CollisionComponent.h"
//#include "Math.h"

class PlayerMove : MoveComponent
{
public:
	PlayerMove::PlayerMove(class Actor* a)
		:MoveComponent(a)
	{
		g = mOwner->GetGame();
		mYSpeed = 0.0f;
		mSpacePressed = false;
		mInAir = false;
	}

	void ProcessInput(const Uint8* keyState)
	{
		if (keyState[SDL_SCANCODE_LEFT])
		{
			SetForwardSpeed(-300.0f);
		}
		else if (keyState[SDL_SCANCODE_RIGHT])
		{
			SetForwardSpeed(300.0f);
		}
		else
		{
			SetForwardSpeed(0.0f);
		}

		if (keyState[SDL_SCANCODE_SPACE])
		{
			if (!mSpacePressed)//when this frame is the leading edge
			{
				if (!mInAir)
				{
					mYSpeed = -700.0f;
					mSpacePressed = true;
					mInAir = true;
				}

			}

		}
		else
		{
			if (mSpacePressed)//trailing edge
			{

			}
			mSpacePressed = false;//when space is not pressed this fram
		}
	}

	void Update(float deltaTime)
	{
		Vector2 pos = mOwner->GetPosition();
		Vector2 change = Vector2(GetForwardSpeed()*deltaTime, mYSpeed * deltaTime);
		pos = pos + change;
		mOwner->SetPosition(pos);

		if (pos.y > 448.0f)
		{
			mOwner->SetPosition(Vector2(pos.x, 448.0f));
			mInAir = false;
		}

		CollisionComponent* marioCollision = mOwner->GetComponent<CollisionComponent>();

		//bool Colliding = false;

		Vector2 offset = Vector2::Zero;
		for (size_t i = 0; i < g->BlockVector.size(); i++)
		{
			CollSide side = marioCollision->GetMinOverlap(g->BlockVector[i]->collision, offset);
			if (side == CollSide::Top && mYSpeed > 0.0f)
			{
				mYSpeed = 0.0f;
				mOwner->SetPosition(pos + offset);
				mInAir = false;
				//Colliding = true;
				break;
			}
			if (side == CollSide::Bottom && mYSpeed < 0.0f)
			{
				mYSpeed = 0.0f;
				mOwner->SetPosition(pos + offset);
				break;
			}
			
			if (side == CollSide::Left)
			{
				mOwner->SetPosition(pos + offset);
				break;
			}
			
			if (side == CollSide::Right)//messing up here, when the block and mario have dx2 of 0, considers that as colliding on right side of block
			{
				mOwner->SetPosition(pos + offset);
				break;
			}
		}

		//if (!Colliding)//if not colliding, then in the air
		//{
		//	mInAir = true;
		//}

		mYSpeed += 2000.0f * deltaTime;
		/*
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
			if (side == CollSide::Top || side == CollSide::Bottom)
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
			
		}*/
	}

private:
	float mYSpeed;
	bool mSpacePressed;
	bool mInAir;
	Game* g;

};