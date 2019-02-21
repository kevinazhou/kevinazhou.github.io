#include "Key.h"
#include "Game.h"

void Key::Setup()
{
	sprite->SetTexture(mGame->GetTexture("Assets/Key.png"));
	collision->SetSize(16, 32);
}