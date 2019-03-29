#pragma once
#include "Actor.h"

class MeshComponent;
class EnemyMove;
class Enemy : public Actor
{
public:
	Enemy(class Game* game);

private:
	MeshComponent* mesh;
	EnemyMove* move;
};