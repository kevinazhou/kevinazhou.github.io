#pragma once
#include "Actor.h"

class Coin : public Actor
{
public:
	Coin(class Game* game, Actor* parent);
	void OnUpdate(float deltaTime);

private:
	class MeshComponent* mesh;
	class CollisionComponent* col;
};