#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
,mDepth(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector3 aMin = GetMin();
	Vector3 aMax = GetMax();
	Vector3 bMin = other->GetMin();
	Vector3 bMax = other->GetMax();

	bool no = aMax.x < bMin.x ||
		aMax.y < bMin.y ||
		aMax.z < bMin.z ||
		bMax.x < aMin.x ||
		bMax.y < aMin.y ||
		bMax.z < aMin.z;

	return !no;
}

Vector3 CollisionComponent::GetMin() const
{
	Vector3 v = mOwner->GetPosition();
	v.x -= mDepth * mOwner->GetScale() / 2.0f;
	v.y -= mWidth * mOwner->GetScale() / 2.0f;
	v.z -= mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

Vector3 CollisionComponent::GetMax() const
{
	Vector3 v = mOwner->GetPosition();
	v.x += mDepth * mOwner->GetScale() / 2.0f;
	v.y += mWidth * mOwner->GetScale() / 2.0f;
	v.z += mHeight * mOwner->GetScale() / 2.0f;
	return v;
}

const Vector3& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}



CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector3& offset)
{
	offset = Vector3::Zero;
	if (Intersect(other) == false)
	{
		return CollSide::None;
	}
	else
	{
		//finding out which side
		Vector3 aMin = GetMin();
		Vector3 aMax = GetMax();
		Vector3 bMin = other->GetMin();
		Vector3 bMax = other->GetMax();

		float dy1, dy2, dx1, dx2, dz1, dz2;
		Vector3 pos = mOwner->GetPosition();
		float topY = aMin.y;
		float botY = aMax.y;
		float rightX = aMax.x;
		float leftX = aMin.x;
		float maxZ = aMax.z;
		float minZ = aMin.z;

		Vector3 pos2 = other->GetCenter();
		float topY2 = bMin.y;
		float botY2 = bMax.y;
		float rightX2 = bMax.x;
		float leftX2 = bMin.x;
		float maxZ2 = bMax.z;
		float minZ2 = bMin.z;

		dy1 = abs(botY - topY2);
		dy2 = abs(topY - botY2);
		dx1 = abs(rightX - leftX2);
		dx2 = abs(leftX - rightX2);
		dz1 = abs(maxZ - minZ2);
		dz2 = abs(minZ - maxZ2);//top

		if (dy1 <= dy2 && dy1 <= dx1 && dy1 <= dx2
			&& dy1 <= dz1 && dy1 <= dz2)
		{
			offset.y -= dy1;
			return CollSide::SideY1;
		}
		else if (dy2 <= dy1 && dy2 <= dx1 && dy2 <= dx2
			&& dy2 <= dz1 && dy2 <= dz2)
		{
			offset.y += dy2;
			return CollSide::SideY2;
		}
		else if (dx1 <= dy1 && dx1 <= dy2 && dx1 <= dx2
			&& dx1 <= dz1 && dx1 <= dz2)
		{
			offset.x -= dx1;
			return CollSide::SideX1;
		}
		else if (dx2 <= dy1 && dx2 <= dy2 && dx2 <= dx1
			&& dx2 <= dz1 && dx2 <= dz2)
		{
			offset.x += dx2;
			return CollSide::SideX2;
		}
		else if (dz1 <= dx1 && dz1 <= dx2 && dz1 <= dy1
			&& dz1 <= dy2 && dz1 <= dz2)
		{
			offset.z -= dz1;
			return CollSide::Bottom;
		}
		else if (dz2 <= dx1 && dz2 <= dx2 && dz2 <= dy1
			&& dz2 <= dy2 && dz2 <= dz1)
		{
			offset.z += dz2;
			return CollSide::Top;
		}
		else
		{
			return CollSide::Top;
		}
	}
}
