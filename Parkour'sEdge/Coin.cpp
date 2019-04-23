#include "Coin.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "CollisionComponent.h"
#include "Player.h"
#include "HUD.h"

Coin::Coin(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Coin.gpmesh"));
	col = new CollisionComponent(this);
	col->SetSize(100, 100, 100);
}

void Coin::OnUpdate(float deltaTime)
{
	float rot = GetRotation();
	rot += Math::Pi * deltaTime;
	SetRotation(rot);

	CollisionComponent* playerCol = mGame->GetPlayer()->GetComponent<CollisionComponent>();
	if (col->Intersect(playerCol))
	{
		Mix_PlayChannel(-1, mGame->GetSound("Assets/Sounds/Coin.wav"), 0);
		SetState(ActorState::Destroy);
		mGame->GetPlayer()->GetComponent<HUD>()->AddCoin();
	}
}
