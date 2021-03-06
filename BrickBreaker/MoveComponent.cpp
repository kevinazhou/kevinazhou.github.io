#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner)
:Component(owner, 50)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
	// TODO: Implement in Part 2
	float rot = mOwner->GetRotation();
	rot += (mAngularSpeed * deltaTime);
	mOwner->SetRotation(rot);

	Vector2 pos = mOwner->GetPosition();
	Vector2 forward = mOwner->GetForward();
	forward = (forward * mForwardSpeed)*deltaTime;
	pos = pos + forward;
	mOwner->SetPosition(pos);


	
}
