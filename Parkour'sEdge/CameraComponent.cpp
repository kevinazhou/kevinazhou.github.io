#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "PlayerMove.h"
#include "CollisionComponent.h"

CameraComponent::CameraComponent(class Actor* owner)
	:Component(owner)
{

}

void CameraComponent::Update(float deltaTime)
{
	Vector3 idealPos = CalculateIdealPosition();

	Vector3 Displacement = CameraPos - idealPos;

	mPitchAngle += mPitchSpeed * deltaTime;

	if (mPitchAngle < -(Math::PiOver2 / 2))
	{
		mPitchAngle = -(Math::PiOver2 / 2);
	}
	if (mPitchAngle > Math::PiOver2 / 2)
	{
		mPitchAngle = Math::PiOver2 / 2;
	}

	Matrix4 pitchMatrix = Matrix4::CreateRotationY(mPitchAngle);
	Matrix4 yawMatrix = Matrix4::CreateRotationZ(mOwner->GetRotation());

	Matrix4 combinedMatrix = pitchMatrix * yawMatrix;

	Vector3 Forward = Vector3::Transform(Vector3(1, 0, 0), combinedMatrix);



	CameraPos += CameraVelocity * deltaTime;

	Vector3 targetPos = mOwner->GetPosition() + (Forward * TargetDist);

	//wallrunning camera
	Vector3 upVector = Vector3::UnitZ;
	PlayerMove* move = mOwner->GetComponent<PlayerMove>();
	if (move->GetCurrentState() == MoveState::WallRun)
	{
  		Matrix4 rot;
		if (move->GetWallRunTimer() > 0.2f)
		{
			angle = 0.6f;
		}
		else
		{
			angle = Math::Lerp(0.0f, 0.48f, move->GetWallRunTimer()/0.2f);
		}
		if (move->WallRunSide == CollSide::SideX1)
		{//positive x
 			rot = Matrix4::CreateRotationY(-angle);
		}
		else if (move->WallRunSide == CollSide::SideX2)
		{
   			rot = Matrix4::CreateRotationY(angle);
		}
		else if (move->WallRunSide == CollSide::SideY1)
		{
			rot = Matrix4::CreateRotationX(angle);
		}
		else if (move->WallRunSide == CollSide::SideY2)
		{
			rot = Matrix4::CreateRotationX(-angle);
		}
		upVector = Vector3::Transform(upVector, rot);
		//now upVector should be properly rotated
		lastWallRunSide = move->WallRunSide;
	}

	//lerping back after wallrun
	if (move->timeFromLastWallRun < 0.2f)
	{
		angle = Math::Lerp(angle, 0.0f, move->timeFromLastWallRun / 0.2f);
		Matrix4 rot;
		if (lastWallRunSide == CollSide::SideX1)
		{//positive x
			rot = Matrix4::CreateRotationY(-angle);
		}
		else if (lastWallRunSide == CollSide::SideX2)
		{
			rot = Matrix4::CreateRotationY(angle);
		}
		else if (lastWallRunSide == CollSide::SideY1)
		{
			rot = Matrix4::CreateRotationX(angle);
		}
		else if (lastWallRunSide == CollSide::SideY2)
		{
			rot = Matrix4::CreateRotationX(-angle);
		}
		upVector = Vector3::Transform(upVector, rot);

		move->timeFromLastWallRun += deltaTime;
	}

	Matrix4 lookMatrix = Matrix4::CreateLookAt(mOwner->GetPosition(), targetPos, upVector);

	mOwner->GetGame()->GetRenderer()->SetViewMatrix(lookMatrix);


}

Vector3 CameraComponent::CalculateIdealPosition()
{
	Vector3 idealPos = mOwner->GetPosition() - (mOwner->GetForward() * HDist)
		+ (Vector3::UnitZ * 70.0f);
	return idealPos;
}

