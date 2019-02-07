#include "GoombaMove.h"
#include "Goomba.h"
#include "CollisionComponent.h"
#include "Block.h"


GoombaMove::GoombaMove(class Actor* a)
	:MoveComponent(a)
{
	g = mOwner->GetGame();
	mYSpeed = 0.0f;
	SetForwardSpeed(-100.0f);
}

void GoombaMove::Update(float deltaTime)
{	
	Goomba* goo = (Goomba*)mOwner;
	if (goo->GetStomped() == true)
	{
		goo->GetComponent<AnimatedSprite>()->SetAnimation("dead");
		g->RemoveGoomba(goo);//to ensure can't collide again with mario
		unsigned int curr = SDL_GetTicks();
		if (curr - goo->StompTime > 250)
		{
			goo->SetState(ActorState::Destroy);
		}
		return;
	}
	Vector2 pos = mOwner->GetPosition();
	Vector2 change = Vector2(GetForwardSpeed()*deltaTime, mYSpeed * deltaTime);
	pos = pos + change;

	mOwner->SetPosition(pos);


	if (pos.y > 464.0f)
	{
		mOwner->SetState(ActorState::Destroy);
	}

	CollisionComponent* goombaCollision = mOwner->GetComponent<CollisionComponent>();

	Vector2 offset = Vector2::Zero;
	for (size_t i = 0; i < g->BlockVector.size(); i++)
	{
		CollSide side = goombaCollision->GetMinOverlap(g->BlockVector[i]->collision, offset);
		if (side != CollSide::None)
		{
			if (side == CollSide::Top && mYSpeed > 0.0f)
			{
				mYSpeed = 0.0f;
				mOwner->SetPosition(pos + offset);
				break;
			}
			/*else if (side == CollSide::Bottom && mYSpeed < 0.0f)
			{
				mYSpeed = 0.0f;
				mOwner->SetPosition(pos + offset);

			} shouldn't be needed since there is not way a goomba can hit the bottom of a block*/
		}
		offset = Vector2::Zero;
	}

	for (size_t i = 0; i < g->BlockVector.size(); i++)
	{
		CollSide side = goombaCollision->GetMinOverlap(g->BlockVector[i]->collision, offset);
		if (side != CollSide::None)
		{
			if (side == CollSide::Left)//left side of the block
			{
				if (offset.x != 0)//to deal with when not real collision cause sides are just touching
				{
					SetForwardSpeed(-100.0f);
					//mOwner->SetPosition(pos + offset);
					pos.x += offset.x;
					mOwner->SetPosition(pos);

					break;//break cause sometimes will detect two collisions, one left one right
				}

			}
			else if (side == CollSide::Right)
			{
				if (offset.x != 0)
				{
					SetForwardSpeed(100.0f);
					//mOwner->SetPosition(pos + offset);
					pos.x += offset.x;
					mOwner->SetPosition(pos);

					break;
				}
			}
		}
	}


	for (size_t i = 0; i < g->GoombaVector.size(); i++)
	{
		if (g->GoombaVector[i]->GetComponent<GoombaMove>() != this)
		{
			CollSide side = goombaCollision->GetMinOverlap(g->GoombaVector[i]->collision, offset);
		
			if (side != CollSide::None)
			{
				//for some reason goombas register as top collisions when they spawn				
				if (side == CollSide::Left)//left side of another goomba
				{
					if (offset.x != 0)//to deal with when not real collision cause sides are just touching
					{
						SetForwardSpeed(-100.0f);
						//mOwner->SetPosition(pos + offset);
						pos.x += offset.x;
						mOwner->SetPosition(pos);
					}

				}
				else if (side == CollSide::Right)
				{
					if (offset.x != 0)
					{
						SetForwardSpeed(100.0f);
						//mOwner->SetPosition(pos + offset);
						pos.x += offset.x;
						mOwner->SetPosition(pos);
					}
				}
			}
			offset = Vector2::Zero;
		}
	}


	mYSpeed += 2000.0f * deltaTime;
}





