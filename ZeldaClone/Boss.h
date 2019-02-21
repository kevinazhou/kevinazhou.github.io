#pragma once
#include "Actor.h"
#include "Game.h"
#include "CollisionComponent.h"
#include "AnimatedSprite.h"
#include "EnemyComponent.h"
#include "Random.h"
#include "Door.h"

class Boss : public Actor
{
public:
	Boss::Boss(Game* g)
		:Actor(g)
	{
		sprite = new AnimatedSprite(this);
		collision = new CollisionComponent(this);
		enemyComp = new EnemyComponent(this);

		std::vector<SDL_Texture*> idle{
			GetGame()->GetTexture("Assets/Dragon/Idle0.png"),
			GetGame()->GetTexture("Assets/Dragon/Idle1.png")
		};

		std::vector<SDL_Texture*> attack{
			GetGame()->GetTexture("Assets/Dragon/Attack0.png"),
			GetGame()->GetTexture("Assets/Dragon/Attack1.png")
		};

		std::vector<SDL_Texture*> enraged{
			GetGame()->GetTexture("Assets/Dragon/Enraged0.png"),
			GetGame()->GetTexture("Assets/Dragon/Enraged1.png"),
			GetGame()->GetTexture("Assets/Dragon/Enraged2.png"),
			GetGame()->GetTexture("Assets/Dragon/Enraged3.png"),
		};

		sprite->AddAnimation("idle", idle);
		sprite->AddAnimation("attack", attack);
		sprite->AddAnimation("enraged", enraged);

		sprite->SetAnimation("idle");

		collision->SetSize(50, 64);
		
		enemyComp->SetHitpoints(5);

		enemyComp->collideDmg = 2;

		LastAttack = 0;

		enemyComp->SetOnDeath([this] {
			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/BossDie.wav"), 0);

			std::vector<Door* > dors = GetGame()->GetDoorsInARoom(GetGame()->currentRoom);
			for (size_t k = 0; k < dors.size(); k++)
			{
				if (dors[k]->GetDoorState() == DoorState::Closed)//opening all the doors
				{
					dors[k]->SetDoorState(DoorState::Open);
					Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/DoorOpen.wav"), 0);
					Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/Secret.wav"), 0);
				}
			}
		});
		enemyComp->SetOnTakeDamage([this] {
			Mix_PlayChannel(-1, GetGame()->GetSound("Assets/Sounds/BossHit.wav"), 0);
		});

		enemyComp->SetInvincible(1000);
	}

	void OnUpdate(float deltaTime);

private:
	AnimatedSprite* sprite;
	CollisionComponent* collision;
	EnemyComponent* enemyComp;

	unsigned int LastAttack;
};