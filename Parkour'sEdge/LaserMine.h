#pragma once
#include "Actor.h"

class LaserComponent;
class MeshComponent;
class LaserMine : public Actor
{
public:
	LaserMine(class Game* game, Actor* parent);

private:
	MeshComponent* mesh;
	LaserComponent* laser;
};