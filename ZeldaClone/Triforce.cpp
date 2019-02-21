#include "Triforce.h"

void Triforce::Setup()
{
	std::vector<SDL_Texture*> idle{
	GetGame()->GetTexture("Assets/Triforce0.png"),
	GetGame()->GetTexture("Assets/Triforce1.png")
	};

	sprite->AddAnimation("idle", idle);
	sprite->SetAnimation("idle");

	collision->SetSize(20, 20);
}