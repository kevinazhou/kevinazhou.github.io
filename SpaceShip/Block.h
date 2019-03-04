#pragma once
#include "Actor.h"

class MeshComponent;
class CollisionComponent;
class Block : public Actor
{
public:
	Block(class Game* game);

	~Block();

	void OnUpdate(float deltaTime);

	void Explode();

	void SetExplodingTex();
	bool isExploding;
private:
	MeshComponent* mesh;
	CollisionComponent* collision;

	
};