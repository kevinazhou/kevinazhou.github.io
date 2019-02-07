#include "CollisionComponent.h"
#include "Actor.h"

CollisionComponent::CollisionComponent(class Actor* owner)
:Component(owner)
,mWidth(0.0f)
,mHeight(0.0f)
{
	
}

CollisionComponent::~CollisionComponent()
{
	
}

bool CollisionComponent::Intersect(const CollisionComponent* other)
{
	Vector2 otherMin = other->GetMin();
	Vector2 otherMax = other->GetMax();
	Vector2 min = GetMin();
	Vector2 max = GetMax();

	bool case1 = (max.x < otherMin.x);
	bool case2 = (otherMax.x < min.x);
	bool case3 = (max.y < otherMin.y);
	bool case4 = (otherMax.y < min.y);

	if (!case1 && !case2 && !case3 && !case4)
	{
		return true;
	}
	else
	{
		return false;
	}

}

Vector2 CollisionComponent::GetMin() const
{
	Vector2 pos = GetCenter();
	int width = mWidth;
	int height = mHeight;

	int minX = pos.x - (width * mOwner->GetScale()) / 2.0f;
	int minY = pos.y - (height * mOwner->GetScale()) / 2.0f;
	Vector2 Zero = Vector2(minX, minY);

	return Zero;
}

Vector2 CollisionComponent::GetMax() const
{
	Vector2 pos = GetCenter();
	int width = mWidth;
	int height = mHeight;

	int maxX = pos.x + (width * mOwner->GetScale()) / 2.0f;
	int maxY = pos.y + (height * mOwner->GetScale()) / 2.0f;
	Vector2 Zero = Vector2(maxX, maxY);
	//Vector2::Zero;
	return Zero;
}

const Vector2& CollisionComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

CollSide CollisionComponent::GetMinOverlap(
	const CollisionComponent* other, Vector2& offset)
{

	if (Intersect(other) == false)
	{
		return CollSide::None;
	}
	else
	{
		//finding out which side
		float dy1, dy2, dx1, dx2;
		Vector2 pos = mOwner->GetPosition();
		int topY = pos.y - (mHeight / 2);
		int botY = pos.y + (mHeight / 2);
		int rightX = pos.x + (mWidth / 2);
		int leftX = pos.x - (mWidth / 2);

		Vector2 pos2 = other->GetCenter();
		int topY2 = pos2.y - (other->GetHeight() / 2);
		int botY2 = pos2.y + (other->GetHeight() / 2);
		int rightX2 = pos2.x + (other->GetWidth() / 2);
		int leftX2 = pos2.x - (other->GetWidth() / 2);

		dy1 = abs(botY - topY2);
		dy2 = abs(topY - botY2);
		dx1 = abs(rightX - leftX2);
		dx2 = abs(leftX - rightX2);
		
		if (dy1 <= dy2 && dy1 <= dx1 && dy1 <= dx2)
		{
			offset.y -= dy1;
			return CollSide::Top;
		}
		else if (dy2 <= dy1 && dy2 <= dx1 && dy2 <= dx2)
		{
			offset.y += dy2;
			return CollSide::Bottom;
		}
		else if (dx1 <= dy1 && dx1 <= dy2 && dx1 <= dx2)
		{
			offset.x -= dx1;
			return CollSide::Left;
		}
		else if(dx2 <= dy1 && dx2 <= dy2 && dx2 <= dx1)
		{
			offset.x += dx2;
			return CollSide::Right;
		}

	}

	

}
