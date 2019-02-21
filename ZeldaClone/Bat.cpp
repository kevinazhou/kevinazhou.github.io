#include "Bat.h"
#include "Random.h"
#include "Math.h"
#include "Collider.h"

void Bat::OnUpdate(float deltaTime)
{
	unsigned int curr = SDL_GetTicks();
	Vector2 currPos = GetPosition();
	if (lastChange == 0 || curr - lastChange > 3000)
	{
		
		Vector2 min = Vector2(currPos.x - 100, currPos.y - 100);
		Vector2 max = Vector2(currPos.x + 100, currPos.y + 100);

		Vector2 r = Random::GetVector(min, max);

		Direction = (r - currPos);
		Direction.Normalize();
		
		lastChange = curr;
		
	}
	SetPosition(currPos + (deltaTime * speed * Direction));

	std::vector<Collider* > colliderVec = mGame->GetColliderVector();

	for (size_t i = 0; i < colliderVec.size(); i++)
	{
		Vector2 offset = Vector2::Zero;
		CollSide side = collision->GetMinOverlap(colliderVec[i]->GetCollisionComponent(), offset);
		if (side != CollSide::None)
		{
			Vector2 position = GetPosition();

			SetPosition(position + offset);

		}

	}

	
	
}