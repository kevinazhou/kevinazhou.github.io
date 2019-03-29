#pragma once
#include "Component.h"
#include "Math.h"

class CameraComponent : public Component
{
public:
	CameraComponent(class Actor* owner);
	void Update(float deltaTime);
	Vector3 CalculateIdealPosition();
	void SnapToIdeal();

private:
	float HDist = 60;
	float TargetDist = 50;
	float SpringConstant = 256.0f;
	float DampeningConstant = 2.0f * sqrt(SpringConstant);
	Vector3 CameraPos;
	Vector3 CameraVelocity;


};