#pragma once
#include "Actor.h"

class MeshComponent;
class MoveComponent;
class CollisionComponent;
class Bullet : public Actor
{
public:
	Bullet(class Game* game);

	void OnUpdate(float deltaTime);
private:
	MeshComponent* mesh;
	MoveComponent* move;
	CollisionComponent* collision;
	unsigned int creationTime;
};