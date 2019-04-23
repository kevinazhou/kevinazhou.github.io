#include "PlayerMove.h"
#include "Player.h"
#include "Renderer.h"
#include "Random.h"
#include "Actor.h"
#include "Game.h"
#include "CameraComponent.h"
#include "Block.h"
#include <iostream>



//#include "CollisionComponent.h"


PlayerMove::PlayerMove(class Actor* owner)
	:MoveComponent(owner)
{
	/*g = mOwner->GetGame();
	mOwner->SetPosition(g->heightMap->CellToWorld(39, 58));
	*/
	ChangeState(MoveState::Falling);

	mRunningSFX = Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Running.wav"), -1);
	Mix_Pause(mRunningSFX);
	

}

void PlayerMove::Update(float deltaTime)
{
	if (mCurrentState == MoveState::Falling)
	{
		Mix_Pause(mRunningSFX);
		UpdateFalling(deltaTime);
	}
	else if (mCurrentState == MoveState::Jump)
	{
		Mix_Pause(mRunningSFX);
		UpdateJump(deltaTime);
	}
	else if (mCurrentState == MoveState::OnGround)
	{
		if (mVelocity.LengthSq() > 2500.0f)
		{
			Mix_Resume(mRunningSFX);
		}
		else
		{
			Mix_Pause(mRunningSFX);
		}
		UpdateOnGround(deltaTime);
	}
	else if (mCurrentState == MoveState::WallClimb)
	{
		Mix_Resume(mRunningSFX);
		UpdateWallClimb(deltaTime);
	}
	else if (mCurrentState == MoveState::WallRun)
	{
		Mix_Resume(mRunningSFX);
		UpdateWallRun(deltaTime);
	}

	if (mOwner->GetPosition().z < -750.0f)
	{
		mOwner->SetPosition(mOwner->GetGame()->GetPlayer()->GetRespawnLocation());
		mOwner->SetRotation(0.0f);
		mVelocity = Vector3::Zero;
		mPendingForces = Vector3::Zero;
		mCurrentState = MoveState::Falling;
	}

	
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (keyState[SDL_SCANCODE_W] && keyState[SDL_SCANCODE_S])
	{
		Vector3 zero = Vector3::Zero;
		AddForce(zero);
	}
	else if (keyState[SDL_SCANCODE_W])
	{
		Vector3 forward = mOwner->GetForward();
		forward *= 700.0f;
		AddForce(forward);
	}
	else if (keyState[SDL_SCANCODE_S])
	{
		Vector3 forward = mOwner->GetForward();
		forward *= -700.0f;
		AddForce(forward);
	}
	else
	{
		//Vector3 zero = Vector3::Zero;
		//AddForce(zero);
	}

	if (keyState[SDL_SCANCODE_D] && keyState[SDL_SCANCODE_A])
	{
		Vector3 zero = Vector3::Zero;
		AddForce(zero);
	}
	else if (keyState[SDL_SCANCODE_D])
	{
		Vector3 right = mOwner->GetRight();
		right *= 700.0f;
		AddForce(right);
	}
	else if (keyState[SDL_SCANCODE_A])
	{
		Vector3 right = mOwner->GetRight();
		right *= -700.0f;
		AddForce(right);
	}
	else
	{
		//Vector3 zero = Vector3::Zero;
		//AddForce(zero);
	}

	if (keyState[SDL_SCANCODE_SPACE])
	{
		if (!spacePressed)
		{//leading edge
			if (mCurrentState == MoveState::OnGround)
			{
				Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Jump.wav"), 0);
				AddForce(mJumpForce);
				ChangeState(MoveState::Jump);
				spacePressed = true;
			}
		}
	}
	else
	{
		spacePressed = false;
	}

	int x, y;
	SDL_GetRelativeMouseState(&x, &y);

	float xFloat = (float)x / 500.0f;
	xFloat *= Math::Pi * 10.0f;
	SetAngularSpeed(xFloat);

	float yFloat = (float)y / 500.0f;
	yFloat *= Math::Pi * 10.0f;
	mOwner->GetComponent<CameraComponent>()->SetPitchSpeed(yFloat);

	/*else if (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT])
	{
		turnLeft();
	}
	else if (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])
	{
		turnRight();
	}
	else
	{
		noTurn();
	}*/

}

void PlayerMove::UpdateOnGround(float deltaTime)
{
	PhysicsUpdate(deltaTime);

	std::vector<Actor*> vec = mOwner->GetGame()->GetBlockVec();
	CollisionComponent* col = mOwner->GetComponent<CollisionComponent>();
	bool onTop = false;
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] != mOwner)
		{
			CollSide s = FixCollision(col, vec[i]->GetComponent<CollisionComponent>());
			if( s == CollSide::Top)
			{
				onTop = true;
			}
			else if (s == CollSide::SideX1 || s == CollSide::SideX2 || s == CollSide::SideY1
				|| s == CollSide::SideY2)
			{
				if (CanWallClimb(s))
				{
					ChangeState(MoveState::WallClimb);
					mWallClimbTimer = 0.0f;
					return;
				}
			}
		}
	}

	if (!onTop)
	{
		ChangeState(MoveState::Falling);
	}
}
void PlayerMove::UpdateJump(float deltaTime)
{

	AddForce(mGravity);
	PhysicsUpdate(deltaTime);

	std::vector<Actor*> vec = mOwner->GetGame()->GetBlockVec();
	CollisionComponent* col = mOwner->GetComponent<CollisionComponent>();

	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] != mOwner)
		{
			CollSide s = FixCollision(col, vec[i]->GetComponent<CollisionComponent>());
			if (s == CollSide::Bottom)
			{//hit head

				mVelocity.z = 0.0f;
			}
			else if (s == CollSide::SideX1 || s == CollSide::SideX2 || s == CollSide::SideY1
				|| s == CollSide::SideY2)
			{
				if (CanWallClimb(s))
				{
					ChangeState(MoveState::WallClimb);
					mWallClimbTimer = 0.0f;
					return;
				}
				if (CanWallRun(s))
				{
					WallRunSide = s;
					ChangeState(MoveState::WallRun);
					mWallRunTimer = 0.0f;
					return;
				}
			}
		}
	}

	if (mVelocity.z <= 0.0f)
	{
		ChangeState(MoveState::Falling);
	}
}
void PlayerMove::UpdateFalling(float deltaTime)
{

	AddForce(mGravity);
	PhysicsUpdate(deltaTime);


	std::vector<Actor*> vec = mOwner->GetGame()->GetBlockVec();
	CollisionComponent* col = mOwner->GetComponent<CollisionComponent>();
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] != mOwner)
		{
			if (FixCollision(col, vec[i]->GetComponent<CollisionComponent>()) == CollSide::Top)
			{
				mVelocity.z = 0.0f;
				Mix_PlayChannel(-1, mOwner->GetGame()->GetSound("Assets/Sounds/Land.wav"), 0);
 				ChangeState(MoveState::OnGround);
				return;
			}

		}

	}
}

void PlayerMove::UpdateWallClimb(float deltaTime)
{
	
	AddForce(mGravity);
	if (mWallClimbTimer < 0.4f)
	{
		AddForce(mWallClimbForce);
	}
	mWallClimbTimer += deltaTime;
	
	PhysicsUpdate(deltaTime);

	std::vector<Actor*> vec = mOwner->GetGame()->GetBlockVec();
	CollisionComponent* col = mOwner->GetComponent<CollisionComponent>();
	
	bool notFall = false;
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] != mOwner)
		{
			CollSide side = FixCollision(col, vec[i]->GetComponent<CollisionComponent>());
			if (side == CollSide::SideX1 || side == CollSide::SideX2
				|| side == CollSide::SideY1 || side == CollSide::SideY2)
			{
				notFall = true;
			}

		}

	}
	
	if (notFall == false || mVelocity.z <= 0.0f)
	{
		mVelocity.z = 0.0f;
		ChangeState(MoveState::Falling);
	}
}

void PlayerMove::UpdateWallRun(float deltaTime)
{
	AddForce(mGravity);
	if (mWallRunTimer < 0.4f)
	{
		AddForce(mWallRunForce);
	}
	mWallRunTimer += deltaTime;

	PhysicsUpdate(deltaTime);

	std::vector<Actor*> vec = mOwner->GetGame()->GetBlockVec();
	CollisionComponent* col = mOwner->GetComponent<CollisionComponent>();

	
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] != mOwner)
		{
			CollSide side = FixCollision(col, vec[i]->GetComponent<CollisionComponent>());
			/*if (side == CollSide::SideX1 || side == CollSide::SideX2
				|| side == CollSide::SideY1 || side == CollSide::SideY2)
			{

			}*/

		}

	}

	if (mVelocity.z <= 0.0f)
	{
		mVelocity.z = 0.0f;
		ChangeState(MoveState::Falling);
		timeFromLastWallRun = 0.0f;
	}
}


CollSide PlayerMove::FixCollision(class CollisionComponent* self, class CollisionComponent* block)
{
	Vector3 offset = Vector3::Zero;
	CollSide side = self->GetMinOverlap(block, offset);


	if (side != CollSide::None)
	{
		if (side == CollSide::SideX1)
		{
			AddForce(Vector3(-700.0f, 0.0f, 0.0f));
		}
		else if (side == CollSide::SideX2)
		{
			AddForce(Vector3(700.0f, 0.0f, 0.0f));
		}
		else if (side == CollSide::SideY1)
		{
			AddForce(Vector3(0.0f, -700.0f, 0.0f));
		}
		else if (side == CollSide::SideY2)
		{
			AddForce(Vector3(0.0f, 700.0f, 0.0f));
		}
		Vector3 pos = mOwner->GetPosition();
		pos += offset;
		mOwner->SetPosition(pos);
	}

	return side;
}

void PlayerMove::AddForce(const Vector3& force)
{
	mPendingForces += force;
}

void PlayerMove::PhysicsUpdate(float deltaTime)
{
	mAcceleration = mPendingForces * (1.0f / mMass);
	mVelocity += mAcceleration * deltaTime;

	FixXYVelocity();

	Vector3 pos = mOwner->GetPosition();
	pos += mVelocity * deltaTime;
	mOwner->SetPosition(pos);

	float rot = mOwner->GetRotation();
	rot += (GetAngularSpeed() * deltaTime);
	mOwner->SetRotation(rot);

	mPendingForces = Vector3::Zero;
}

void PlayerMove::FixXYVelocity()
{
	Vector2 temp = Vector2(mVelocity.x, mVelocity.y);
	if (temp.LengthSq() > 400.0f * 400.0f)
	{
		temp.Normalize();
		temp *= 400.0f;
	}


	bool strafingFix = (mAcceleration.x < 0 && temp.x > 0) || (mAcceleration.x > 0 && temp.x < 0);
	bool yFix = (mAcceleration.y < 0 && temp.y > 0) || (mAcceleration.y > 0 && temp.y < 0);

	if (mCurrentState == MoveState::OnGround || mCurrentState == MoveState::WallClimb)
	{
		if (Math::NearZero(mAcceleration.x) || strafingFix)
		{
			temp.x *= 0.9f;
		}
		if (Math::NearZero(mAcceleration.y)|| yFix)
		{
			temp.y *= 0.9f;
		}

		
	}

	mVelocity.x = temp.x;
	mVelocity.y = temp.y;
}

bool PlayerMove::CanWallClimb(CollSide side)
{
	Vector3 blockNormal;
	if (side == CollSide::SideY1)
	{
		blockNormal = Vector3(0, -1, 0);
	}
	else if (side == CollSide::SideY2)
	{
		blockNormal = Vector3(0, 1, 0);
	}
	else if (side == CollSide::SideX1)
	{
		blockNormal = Vector3(-1, 0, 0);
	}
	else if (side == CollSide::SideX2)
	{
		blockNormal = Vector3(1, 0, 0);
	}

	if (Vector3::Dot(mOwner->GetForward(), blockNormal) < -0.8f)
	{//if player facing the block
		Vector3 velocityNormal = Vector3(mVelocity.x, mVelocity.y, 0);
		velocityNormal.Normalize();
		if (Vector3::Dot(blockNormal, velocityNormal) < -0.8f)
		{//if running towards the block
			Vector3 xyVelocity = Vector3(mVelocity.x, mVelocity.y, 0);
			if (xyVelocity.LengthSq() > 350.0f * 350.0f)
			{
				return true;
			}
		}

	}
	return false;
}

bool PlayerMove::CanWallRun(CollSide side)
{
	Vector3 blockNormal;
	if (side == CollSide::SideY1)
	{
		blockNormal = Vector3(0, -1, 0);
	}
	else if (side == CollSide::SideY2)
	{
		blockNormal = Vector3(0, 1, 0);
	}
	else if (side == CollSide::SideX1)
	{
		blockNormal = Vector3(-1, 0, 0);
	}
	else if (side == CollSide::SideX2)
	{
		blockNormal = Vector3(1, 0, 0);
	}

	if (Vector3::Dot(mOwner->GetForward(), blockNormal) > -0.5f)
	{//approx 120 degrees angle
		Vector3 velocityNormal = Vector3(mVelocity.x, mVelocity.y, 0);
		velocityNormal.Normalize();
		if (Vector3::Dot(mOwner->GetForward(), velocityNormal) > 0.7f)
		{//if roughly moving in the forward direction
			Vector3 xyVelocity = Vector3(mVelocity.x, mVelocity.y, 0);
			if (xyVelocity.LengthSq() > 350.0f * 350.0f)
			{//moving fast enough
				return true;
			}
		}
	}
	return false;
}