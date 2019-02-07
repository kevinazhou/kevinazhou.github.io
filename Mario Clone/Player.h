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
		std::vector<SDL_Texture*> idle{
			GetGame()->GetTexture("Assets/Mario/Idle.png")
		};
		std::vector<SDL_Texture*> dead{
			GetGame()->GetTexture("Assets/Mario/Dead.png")
		};
		std::vector<SDL_Texture*> jumpLeft{
			GetGame()->GetTexture("Assets/Mario/JumpLeft.png")
		};
		std::vector<SDL_Texture*> jumpRight{
			GetGame()->GetTexture("Assets/Mario/JumpRight.png")
		};
		std::vector<SDL_Texture*> runLeft{
			GetGame()->GetTexture("Assets/Mario/RunLeft0.png"),
			GetGame()->GetTexture("Assets/Mario/RunLeft1.png"),
			GetGame()->GetTexture("Assets/Mario/RunLeft2.png")
		};
		std::vector<SDL_Texture*> runRight{
			GetGame()->GetTexture("Assets/Mario/RunRight0.png"),
			GetGame()->GetTexture("Assets/Mario/RunRight1.png"),
			GetGame()->GetTexture("Assets/Mario/RunRight2.png")
		};

		sprite = new AnimatedSprite(this, 200);
		sprite->AddAnimation("idle", idle);
		sprite->AddAnimation("dead", dead);
		sprite->AddAnimation("jumpLeft", jumpLeft);
		sprite->AddAnimation("jumpRight", jumpRight);
		sprite->AddAnimation("runLeft", runLeft);
		sprite->AddAnimation("runRight", runRight);
		sprite->SetAnimation("idle");

		collision = new CollisionComponent(this);
		collision->SetSize(32, 32);
		move = new PlayerMove(this);
		//mGame->AddBlock(this);
	}

	Player::~Player()
	{
		//mGame->RemoveBlock(this);
	}

	void changeTexture(SDL_Texture* tex)
	{
		Game* g = GetGame();
		sprite->SetTexture(tex);
	}




private:
	AnimatedSprite* sprite;
	CollisionComponent* collision;
	PlayerMove* move;
};