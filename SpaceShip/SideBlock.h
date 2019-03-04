#pragma once
#include "Actor.h"

class MeshComponent;

class SideBlock : public Actor
{
public:
	SideBlock(class Game* game);
	void SetTexIndex(int i);
	void OnUpdate(float deltaTime);

private:
	MeshComponent* mesh;

};