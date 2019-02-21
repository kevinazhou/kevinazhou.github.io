#pragma once
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "AnimatedSprite.h"
#include "EnemyComponent.h"

class Bat : public Actor
{
public:
	Bat::Bat(Game* g)
		:Actor(g)
	{
		std::vector<SDL_Texture*> batAnim{
			GetGame()->GetTexture("Assets/Bat0.png"),
			GetGame()->GetTexture("Assets/Bat1.png")
		};
		sprite = new AnimatedSprite(this);
		sprite->AddAnimation("batAnime", batAnim);
		sprite->SetAnimation("batAnime");

		collision = new CollisionComponent(this);
		collision->SetSize(25, 25);

		enemyComp = new EnemyComponent(this);
		enemyComp->SetOnDeath([this] {
			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/EnemyDie.wav"), 0);
		});
		enemyComp->SetOnTakeDamage([this] {

		});

		enemyComp->collideDmg = 1;

		speed = 50.0f;
		lastChange = 0;
	}

	void OnUpdate(float deltaTime);
private:
	AnimatedSprite* sprite;
	CollisionComponent* collision;
	EnemyComponent* enemyComp;
	unsigned int lastChange;
	float speed;
	Vector2 Direction;

};