#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"


CameraComponent::CameraComponent(class Actor* owner)
	:Component(owner)
{

}

void CameraComponent::Update(float deltaTime)
{
	Vector3 idealPos = CalculateIdealPosition();

	Vector3 Displacement = CameraPos - idealPos;

	Vector3 SpringAccel = (-SpringConstant * Displacement) - 
		(DampeningConstant * CameraVelocity);

	CameraVelocity += SpringAccel * deltaTime;

	CameraPos += CameraVelocity * deltaTime;

	Vector3 targetPos = mOwner->GetPosition() + (mOwner->GetForward() * TargetDist);

	Matrix4 lookMatrix = Matrix4::CreateLookAt(CameraPos, targetPos, Vector3::UnitZ);

	mOwner->GetGame()->GetRenderer()->SetViewMatrix(lookMatrix);
}

Vector3 CameraComponent::CalculateIdealPosition()
{
	Vector3 idealPos = mOwner->GetPosition() - (mOwner->GetForward() * HDist)
		+ (Vector3::UnitZ * 70.0f);
	return idealPos;
}

void CameraComponent::SnapToIdeal()
{
	CameraPos = CalculateIdealPosition();
	Vector3 targetPos = mOwner->GetPosition() + (mOwner->GetForward() * TargetDist);
	
	Matrix4 lookMatrix = Matrix4::CreateLookAt(CameraPos, targetPos, Vector3::UnitZ);

	mOwner->GetGame()->GetRenderer()->SetViewMatrix(lookMatrix);
}