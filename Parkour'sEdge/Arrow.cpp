#include "Arrow.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Checkpoint.h"

Arrow::Arrow(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Arrow.gpmesh"));
	SetScale(0.15f);

}

void Arrow::OnUpdate(float deltaTime)
{
	Vector3 pos = GetPosition();
	if (mGame->NextCheckpoint() != nullptr)
	{
		Vector3 nextCheckPos = mGame->NextCheckpoint()->GetPosition();

		Vector3 vecTo = nextCheckPos - pos;
		vecTo.Normalize();

		Vector3 crossProduct;
		if (vecTo.x == 1)
		{
			crossProduct = Vector3::UnitZ;//not sure if this is right
		}
		else
		{
			crossProduct = Vector3::Cross(Vector3::UnitX, vecTo);
		}

		crossProduct.Normalize();

		float theta = acos(Vector3::Dot(Vector3::UnitX, vecTo));


		Quaternion q;
		if (theta == 1)
		{
			q = Quaternion::Identity;
		}
		else if (theta == -1)
		{
			q = Quaternion(crossProduct, Math::Pi);
		}
		else
		{
			q = Quaternion(crossProduct, theta);
		}

		SetQuaternion(q);
	}
	else//set to identity
	{
		SetQuaternion(Quaternion::Identity);
	}
	
	SetPosition(mGame->GetRenderer()->Unproject(Vector3(0.0f, 250.0f, 0.1f)));

	
	

}