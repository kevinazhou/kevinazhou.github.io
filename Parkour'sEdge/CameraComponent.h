#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime);
	Vector3 CalculateIdealPosition();

	float GetPitchSpeed()
	{
		return mPitchSpeed;
	}

	void SetPitchSpeed(float set)
	{
		mPitchSpeed = set;
	}

private:
	float HDist = 60;
	float TargetDist = 50;

	float mPitchAngle = 0.0f;
	float mPitchSpeed = 0.0f;

	Vector3 CameraPos;
	Vector3 CameraVelocity;

	float angle;
	enum class CollSide lastWallRunSide;

};