#include "Boss.h"
#include "FireBall.h"
#include "SpriteComponent.h"

void Boss::OnUpdate(float deltaTime)
{
	unsigned int curr = SDL_GetTicks();

	sprite->SetIsPaused(false);
	sprite->SetAnimation("idle");

	if (enemyComp->GetHP() > 2)
	{
		if (curr - LastAttack >= 1750)
		{
			sprite->SetAnimation("attack");
		}

		if (curr - LastAttack >= 2000)//attack every 2 sec
		{
			FireBall* fireball = new FireBall(mGame);
			fireball->SetPosition(GetPosition());
			SpriteComponent* fireSprite1 = fireball->GetComponent<SpriteComponent>();
			fireSprite1->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball->SetRotation(Math::Pi);

			FireBall* fireball2 = new FireBall(mGame);
			fireball2->SetPosition(GetPosition());
			SpriteComponent* fireSprite2 = fireball2->GetComponent<SpriteComponent>();
			fireSprite2->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball2->SetRotation(2.61799);

			FireBall* fireball3 = new FireBall(mGame);
			fireball3->SetPosition(GetPosition());
			SpriteComponent* fireSprite3 = fireball3->GetComponent<SpriteComponent>();
			fireSprite3->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball3->SetRotation(3.66519);

			LastAttack = curr;
			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Fireball.wav"), 0);
		}
	}
	else//enraged
	{
		sprite->SetAnimation("enraged");

		if (curr - LastAttack >= 1000)//attack every 2 sec
		{
			FireBall* fireball = new FireBall(mGame);
			fireball->SetPosition(GetPosition());
			SpriteComponent* fireSprite1 = fireball->GetComponent<SpriteComponent>();
			fireSprite1->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball->SetRotation(Math::Pi/2);

			FireBall* fireball2 = new FireBall(mGame);
			fireball2->SetPosition(GetPosition());
			SpriteComponent* fireSprite2 = fireball2->GetComponent<SpriteComponent>();
			fireSprite2->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball2->SetRotation(1.96349);

			FireBall* fireball3 = new FireBall(mGame);
			fireball3->SetPosition(GetPosition());
			SpriteComponent* fireSprite3 = fireball3->GetComponent<SpriteComponent>();
			fireSprite3->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball3->SetRotation(2.35619);

			FireBall* fireball4 = new FireBall(mGame);
			fireball4->SetPosition(GetPosition());
			SpriteComponent* fireSprite4 = fireball4->GetComponent<SpriteComponent>();
			fireSprite4->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball4->SetRotation(2.74889);

			FireBall* fireball5 = new FireBall(mGame);
			fireball5->SetPosition(GetPosition());
			SpriteComponent* fireSprite5 = fireball5->GetComponent<SpriteComponent>();
			fireSprite5->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball5->SetRotation(Math::Pi);

			FireBall* fireball6 = new FireBall(mGame);
			fireball6->SetPosition(GetPosition());
			SpriteComponent* fireSprite6 = fireball6->GetComponent<SpriteComponent>();
			fireSprite6->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball6->SetRotation(3.5342);

			FireBall* fireball7 = new FireBall(mGame);
			fireball7->SetPosition(GetPosition());
			SpriteComponent* fireSprite7 = fireball7->GetComponent<SpriteComponent>();
			fireSprite7->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball7->SetRotation(3.92699);

			FireBall* fireball8 = new FireBall(mGame);
			fireball8->SetPosition(GetPosition());
			SpriteComponent* fireSprite8 = fireball8->GetComponent<SpriteComponent>();
			fireSprite8->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball8->SetRotation(4.3196);

			FireBall* fireball9 = new FireBall(mGame);
			fireball9->SetPosition(GetPosition());
			SpriteComponent* fireSprite9 = fireball9->GetComponent<SpriteComponent>();
			fireSprite9->SetTexture(mGame->GetTexture("Assets/FireballGreen.png"));
			fireball9->SetRotation(4.71239);

			LastAttack = curr;
			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Fireball.wav"), 0);
		}
	}
	
}