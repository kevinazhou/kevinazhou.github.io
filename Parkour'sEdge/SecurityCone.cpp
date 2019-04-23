#include "SecurityCone.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "Player.h"

SecurityCone::SecurityCone(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	mesh = new MeshComponent(this, true);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Cone.gpmesh"));

	SetPosition(Vector3(30, 0, 0));
	SetScale(3.0f);
}

void SecurityCone::OnUpdate(float deltaTime)
{
	if (KillPlayer)
	{//waiting to kill player
		isCurrentlyNotWhite = true;
		HalfSecToKill += deltaTime;
		if (HalfSecToKill > 0.5f)
		{
			Player* p = mGame->GetPlayer();
			p->SetPosition(p->GetRespawnLocation());
			PlayerDetected = false;
			mesh->SetTextureIndex(0);
			KillPlayer = false;
			isCurrentlyNotWhite = false;
		}
	}
	else
	{
		if (PlayerDetected == false)
		{
			if (DetectPlayer())
			{//Player detected
				mesh->SetTextureIndex(1);
				timeYellow = SDL_GetTicks();
				PlayerDetected = true;
				isCurrentlyNotWhite = true;
				
			}
		}
		else
		{//when player is already detected
			if (DetectPlayer())
			{//if player is still in cone
				if (SDL_GetTicks() - timeYellow > 2000.0f)
				{
					KillPlayer = true;
					mesh->SetTextureIndex(2);
					HalfSecToKill = 0.0f;
				}
			}
			else
			{//if player leaves cone
				mesh->SetTextureIndex(0);
				PlayerDetected = false;
				isCurrentlyNotWhite = false;
				
			}
		}
	}
	
}

bool SecurityCone::DetectPlayer()
{
	Vector3 startPos = GetWorldPosition();
	Vector3 forwardVec = GetWorldForward();

	Vector3 playerPos = mGame->GetPlayer()->GetPosition();

	Vector3 VecToPlayer = playerPos - startPos;


	float dot = Vector3::Dot(VecToPlayer, forwardVec);

	if (dot > 0.0f)
	{//in front of cone
		if (acos(dot / VecToPlayer.Length()) < 0.523599)
		{//30 degrees
			if (dot < 300)
			{//just need dot since forwardVec is unit vector
				return true;

			}
		}
	}

	return false;
}