#pragma once
#include "MoveComponent.h"
#include "Actor.h"
#include "Game.h"

class GoombaMove : MoveComponent
{
public:
	GoombaMove::GoombaMove(class Actor* a);


	void Update(float deltaTime);
	
private:
	
	float mYSpeed;

	Game* g;

};