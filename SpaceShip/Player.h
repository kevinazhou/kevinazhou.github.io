#pragma once
#include "Actor.h"

class MeshComponent;
class PlayerMove;
class Player : public Actor
{
public:
	Player(class Game* game);

private:
	MeshComponent* mesh;
	CollisionComponent* collision;
	PlayerMove* move;
};