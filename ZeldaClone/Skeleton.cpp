#include "Skeleton.h"
#include "FireBall.h"
#include "Player.h"
#include "Game.h"

void Skeleton::OnUpdate(float deltaTime)
{
	unsigned int curr = SDL_GetTicks();
	if (firstShot == true && curr - lastFire >= 4000)
	{
		FireBall* fireball = new FireBall(mGame);
		fireball->SetPosition(GetPosition());

		Player* p = mGame->GetPlayer();
		Vector2 vectorToLink = (p->GetPosition()) - (GetPosition());

		float angle = Math::Atan2(-vectorToLink.y, vectorToLink.x);
		fireball->SetRotation(angle);
		lastFire = curr;

		Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Fireball.wav"), 0);
	}
	else if (firstShot == false)//waiting for the first shot
	{
		if (curr - spawnTime >= randomInitCooldown * 1000)
		{
			FireBall* fireball = new FireBall(mGame);
			fireball->SetPosition(GetPosition());

			Player* p = mGame->GetPlayer();
			Vector2 vectorToLink = (p->GetPosition()) - (GetPosition());

			float angle = Math::Atan2(-vectorToLink.y, vectorToLink.x);
			fireball->SetRotation(angle);
			lastFire = curr;
			firstShot = true;

			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Fireball.wav"), 0);
		}
	}
}