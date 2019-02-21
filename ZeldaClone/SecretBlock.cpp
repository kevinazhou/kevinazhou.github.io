#include "SecretBlock.h"
#include "Game.h"


void SecretBlock::Setup()
{
	sprite->SetTexture(mGame->GetTexture("Assets/SecretBlock.png"));
	collision->SetSize(32, 32);
}
