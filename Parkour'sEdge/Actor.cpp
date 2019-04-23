#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game, Actor* parent)
	:mGame(game)
	,mState(ActorState::Active)
	,mPosition(Vector3::Zero)
	,mScale(1.0f)
	,mRotation(0.0f)
	,mParent(parent)
{
	if (mParent == nullptr)
	{
		mGame->AddActor(this);
	}
	else
	{
		mParent->AddChild(this);
	}
	

}

Actor::~Actor()
{
	
	while (!mChildren.empty())
	{
		mChildren.pop_back();
	}
	if (mParent == nullptr)
	{
		mGame->RemoveActor(this);
	}
	else
	{
		mParent->RemoveChild(this);
	}
	
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		delete mComponents[i];
	}
	mComponents.clear();
}

void Actor::Update(float deltaTime)
{
	CalcWorldTransform();

	if (mState == ActorState::Active)
	{
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->Update(deltaTime);
		}
		this->OnUpdate(deltaTime);
	}

	CalcWorldTransform();

	for (size_t i = 0; i < mChildren.size(); i++)
	{
		mChildren[i]->Update(deltaTime);
	}
}

void Actor::OnUpdate(float deltaTime)
{
}

void Actor::ProcessInput(const Uint8* keyState)
{
	if (mState == ActorState::Active)
	{
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->ProcessInput(keyState);
		}
		this->OnProcessInput(keyState);
	}
}

void Actor::OnProcessInput(const Uint8* keyState)
{
}

void Actor::AddComponent(Component* c)
{
	mComponents.emplace_back(c);
	std::sort(mComponents.begin(), mComponents.end(), [](Component* a, Component* b) {
		return a->GetUpdateOrder() < b->GetUpdateOrder();
	});
}


const Vector3 Actor::GetForward()
{
	Vector3 ret = Vector3(cos(mRotation), sin(mRotation), 0.0f);
	return ret;
}

const Vector3 Actor::GetRight()
{
	Vector3 ret = Vector3(cos(mRotation + Math::PiOver2), sin(mRotation + Math::PiOver2), 0.0f);
	return ret;
}

Vector3 Actor::GetQuatForward()
{
	Vector3 ret = Vector3::Transform(Vector3::UnitX, quater);
	ret.Normalize();
	return ret;
}

void Actor::CalcWorldTransform()
{
	Vector3 scaleVec = Vector3(mScale, mScale, mScale);
	Matrix4 scale = Matrix4::CreateScale(scaleVec);

	Matrix4 rotate = Matrix4::CreateRotationZ(mRotation);

	Matrix4 position = Matrix4::CreateTranslation(mPosition);

	Matrix4 quatMatrix = Matrix4::CreateFromQuaternion(quater);

	mWorldTransform = scale * rotate * quatMatrix * position;

	if (mParent != nullptr)
	{
		if (mInheritScale)
		{
			mWorldTransform *= mParent->mWorldTransform;
		}
		else
		{
			mWorldTransform *= mParent->CalcWorldRotTrans();
		}
	}
}

Matrix4 Actor::CalcWorldRotTrans()
{
	Matrix4 rotate = Matrix4::CreateRotationZ(mRotation);

	Matrix4 position = Matrix4::CreateTranslation(mPosition);

	Matrix4 quatMatrix = Matrix4::CreateFromQuaternion(quater);

	Matrix4 ret = rotate * quatMatrix * position;

	if (mParent != nullptr)
	{
		ret = ret * mParent->CalcWorldRotTrans();
	}

	return ret;
}

Vector3 Actor::GetWorldPosition()
{
	return mWorldTransform.GetTranslation();
}

Vector3 Actor::GetWorldForward()
{
	return mWorldTransform.GetXAxis();
}

void Actor::AddChild(Actor* child)
{
	mChildren.push_back(child);
}

void Actor::RemoveChild(Actor* child)
{
	std::vector<Actor* >::iterator it = std::find(mChildren.begin(), mChildren.end(), child);
	if (it != mChildren.end())
	{
		mChildren.erase(it);
	}
}