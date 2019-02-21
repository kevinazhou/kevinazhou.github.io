#pragma once
#include "Actor.h"
#include "Game.h"
#include "AnimatedSprite.h"
#include "CollisionComponent.h"
#include "PlayerMove.h"
#include <string>



class Player : public Actor
{
public:
	Player::Player(Game* g)
		:Actor(g)
	{
		std::vector<SDL_Texture*> walkUp{
			GetGame()->GetTexture("Assets/Link/WalkUp0.png"),
			GetGame()->GetTexture("Assets/Link/WalkUp1.png")
		};
		std::vector<SDL_Texture*> walkDown{
			GetGame()->GetTexture("Assets/Link/WalkDown0.png"),
			GetGame()->GetTexture("Assets/Link/WalkDown1.png")
		};
		std::vector<SDL_Texture*> walkLeft{
			GetGame()->GetTexture("Assets/Link/WalkLeft0.png"),
			GetGame()->GetTexture("Assets/Link/WalkLeft1.png")
		};
		std::vector<SDL_Texture*> walkRight{
			GetGame()->GetTexture("Assets/Link/WalkRight0.png"),
			GetGame()->GetTexture("Assets/Link/WalkRight1.png")
		};
		std::vector<SDL_Texture*> attackUp{
			GetGame()->GetTexture("Assets/Link/AttackUp.png")
		};
		std::vector<SDL_Texture*> attackDown{
			GetGame()->GetTexture("Assets/Link/AttackDown.png")
		};
		std::vector<SDL_Texture*> attackLeft{
			GetGame()->GetTexture("Assets/Link/AttackLeft.png")
		};
		std::vector<SDL_Texture*> attackRight{
			GetGame()->GetTexture("Assets/Link/AttackRight.png")
		};



		sprite = new AnimatedSprite(this, 200);
		sprite->AddAnimation("walkUp", walkUp);
		sprite->AddAnimation("walkDown", walkDown);
		sprite->AddAnimation("walkLeft", walkLeft);
		sprite->AddAnimation("walkRight", walkRight);
		sprite->AddAnimation("attackUp", attackUp);
		sprite->AddAnimation("attackDown", attackDown);
		sprite->AddAnimation("attackLeft", attackLeft);
		sprite->AddAnimation("attackRight", attackRight);

		sprite->SetAnimation("walkUp");
		sprite->SetIsPaused(true);


		collision = new CollisionComponent(this);
		collision->SetSize(20, 20);
		move = new PlayerMove(this);
		
		hitpoints = 10;

	}

	Player::~Player()
	{
		
	}

	void changeTexture(SDL_Texture* tex)
	{
		Game* g = GetGame();
		sprite->SetTexture(tex);
	}

	void TakeDamage(int amount)
	{
		hitpoints -= amount;
		if (hitpoints <= 0)
		{
			SetState(ActorState::Paused);
			Mix_HaltChannel(mGame->GetChannel());
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/LinkDie.wav"), 0);
		}
		else
		{
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/LinkHit.wav"), 0);
		}
	}



private:
	AnimatedSprite* sprite;
	CollisionComponent* collision;
	PlayerMove* move;
	int hitpoints;
};