#pragma once
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "AnimatedSprite.h"
#include "EnemyComponent.h"
#include "Random.h"

class Skeleton : public Actor
{
public:
	Skeleton::Skeleton(Game* g)
		:Actor(g)
	{
		std::vector<SDL_Texture*> skeleAnim{
			GetGame()->GetTexture("Assets/Skeleton0.png"),
			GetGame()->GetTexture("Assets/Skeleton1.png")
		};
		sprite = new AnimatedSprite(this);
		sprite->AddAnimation("skeleAnime", skeleAnim);
		sprite->SetAnimation("skeleAnime");

		collision = new CollisionComponent(this);
		collision->SetSize(25, 25);

		enemyComp = new EnemyComponent(this);
		enemyComp->SetOnDeath([this] {
			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/EnemyDie.wav"), 0);
		});
		enemyComp->SetOnTakeDamage([this] {

		});

		enemyComp->collideDmg = 1;

		randomInitCooldown = Random::GetFloatRange(1.0f, 3.0f);
		lastFire = 0;
		firstShot = false;
		spawnTime = SDL_GetTicks();
	}

	void OnUpdate(float deltaTime);
private:
	AnimatedSprite* sprite;
	CollisionComponent* collision;
	EnemyComponent* enemyComp;
	unsigned int lastFire;
	float randomInitCooldown;
	bool firstShot;
	unsigned int spawnTime;
	//Vector2 Direction;

};