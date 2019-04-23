#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "Math.h"
#include "CollisionComponent.h"

enum class MoveState {
	OnGround,
	Jump,
	Falling,
	WallClimb,
	WallRun
};

class SideBlock;
class PlayerMove : public MoveComponent
{
public:
	PlayerMove(class Actor* owner);
	~PlayerMove() { Mix_HaltChannel(mRunningSFX); }
	void Update(float deltaTime);
	void ProcessInput(const Uint8* keyState);

	void ChangeState(MoveState state)
	{
		mCurrentState = state;
	}

	void PhysicsUpdate(float deltaTime);
	void AddForce(const Vector3& force);
	void FixXYVelocity();

	bool CanWallClimb(CollSide side);
	bool CanWallRun(CollSide side);

	MoveState GetCurrentState()
	{
		return mCurrentState;
	}

	float GetWallRunTimer()
	{
		return mWallRunTimer;
	}


	float timeFromLastWallRun;//to be updated in cameracomponent
	CollSide WallRunSide;

private:
	MoveState mCurrentState;
	//float mZSpeed = 0.0f;
	//const float Gravity = -980.0f;
	//const float JumpSpeed = 500.0f;
	Vector3 mVelocity;
	Vector3 mAcceleration;
	Vector3 mPendingForces;
	float mMass = 1.0f;
	Vector3 mGravity = Vector3(0.0f, 0.0f, -980.0f);
	Vector3 mJumpForce = Vector3(0.0f, 0.0f, 35000.0f);
	Vector3 mWallClimbForce = Vector3(0.0f, 0.0f, 1800.0f);
	bool spacePressed = false;
	float mWallClimbTimer;
	//Game* g;
	Vector3 mWallRunForce = Vector3(0.0f, 0.0f, 1200.0f);
	float mWallRunTimer = 0.0f;

	int mRunningSFX;

protected:
	void UpdateOnGround(float deltaTime);
	void UpdateJump(float deltaTime);
	void UpdateFalling(float deltaTime);
	void UpdateWallClimb(float deltaTime);
	void UpdateWallRun(float deltaTime);

	CollSide FixCollision(class CollisionComponent* self, class CollisionComponent* block);

};