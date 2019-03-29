#include "PlayerMove.h"
#include "Player.h"
#include "Renderer.h"
#include "Random.h"
#include "HeightMap.h"
#include "PlayerUI.h"
#include "Actor.h"
#include "Game.h"
#include "Enemy.h"

//#include "CollisionComponent.h"


PlayerMove::PlayerMove(class Actor* owner)
	:VehicleMove(owner)
{
	g = mOwner->GetGame();
	mOwner->SetPosition(g->heightMap->CellToWorld(39, 58));



	

}

void PlayerMove::Update(float deltaTime)
{
	VehicleMove::Update(deltaTime);

	

	
}

void PlayerMove::ProcessInput(const Uint8* keyState)
{
	if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP])
	{
		pressPedal();
	}
	else
	{
		unpressPedal();
	}

	if (keyState[SDL_SCANCODE_A] && keyState[SDL_SCANCODE_D])
	{
		noTurn();
	}
	else if (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT])
	{
		turnLeft();
	}
	else if (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])
	{
		turnRight();
	}
	else
	{
		noTurn();
	}

}

void PlayerMove::OnLapChange(int newLap)
{
	if (newLap == 5)
	{
		Enemy* e = mOwner->GetGame()->GetEnemy();
		VehicleMove* enemyM = e->GetComponent<VehicleMove>();
		Mix_FadeOutChannel(mOwner->GetGame()->musicInt, 250);
		if (enemyM->GetCurrentLap() < 5)
		{//win
			PlayerUI* ui = mOwner->GetComponent<PlayerUI>();
			ui->SetRaceState(PlayerUI::RaceState::Won);
			Mix_PlayChannel(-1, mOwner->GetGame()->wonSound, 0);
		}
		else
		{
			PlayerUI* ui = mOwner->GetComponent<PlayerUI>();
			ui->SetRaceState(PlayerUI::RaceState::Lost);
			Mix_PlayChannel(-1, mOwner->GetGame()->lostSound, 0);
		}
		e->SetState(ActorState::Paused);
		mOwner->SetState(ActorState::Paused);
		return;
	}
	if (newLap == 4)
	{
		Mix_FadeOutChannel(mOwner->GetGame()->musicInt, 250);
		Mix_PlayChannel(-1, mOwner->GetGame()->finalLapSound, 0);
		mOwner->GetGame()->musicInt = Mix_FadeInChannel(-1, mOwner->GetGame()->musicFastSound, -1, 4000);
	}
	Player* p = (Player*)mOwner;
	p->ui->OnLapChange(newLap);
}