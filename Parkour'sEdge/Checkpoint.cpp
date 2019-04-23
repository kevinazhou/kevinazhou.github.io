#include "Checkpoint.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "Player.h"
#include "CollisionComponent.h"
#include "HUD.h"

Checkpoint::Checkpoint(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Checkpoint.gpmesh"));
	mesh->SetTextureIndex(1);
	collision = new CollisionComponent(this);
	collision->SetSize(25, 25, 25);
}

void Checkpoint::OnUpdate(float deltaTime)
{
	if (this == mGame->NextCheckpoint())
	{
		isActive = true;
	}
	

	if (isActive)
	{
		ChangeTexture(0);
		CollisionComponent* playerCol = mGame->GetPlayer()->GetComponent<CollisionComponent>();
		if (collision->Intersect(playerCol))
		{//if player collides
			Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Checkpoint.wav"), 0);
			
			SetState(ActorState::Destroy);
			mGame->GetPlayer()->SetRespawnLocation(GetPosition());
			if (!mLevelString.empty())
			{
				mGame->SetNextLevel(mLevelString);
			}

			//setting the HUD text on screen
			mGame->GetPlayer()->GetComponent<HUD>()->UpdateCheckpointText(mCheckpointText);
		}
	}
	else
	{
		ChangeTexture(1);
	}


}

Checkpoint::~Checkpoint()
{
	mGame->RemoveFrontCheckpoint();
}

void Checkpoint::ChangeTexture(int i)
{
	mesh->SetTextureIndex(i);
}