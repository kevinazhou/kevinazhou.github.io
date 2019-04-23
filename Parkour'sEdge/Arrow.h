#pragma once
#include "Actor.h"

class Arrow : public Actor
{
public:
	Arrow(class Game* game, Actor* parent);
	void OnUpdate(float deltaTime);

private:
	class MeshComponent* mesh;
};