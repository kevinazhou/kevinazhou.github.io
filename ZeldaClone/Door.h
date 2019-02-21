#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "SpriteComponent.h"

enum class DoorState
{
	Closed,
	Locked,
	Open
};

enum class DoorDirection
{
	Down,
	Left,
	Up,
	Right
};

class Door : public Actor
{
public:
	Door::Door(Game* g)
		:Actor(g)
	{
		sprite = new SpriteComponent(this);
		collision = new CollisionComponent(this);
	}

	void Setup(std::string dest, DoorState s, DoorDirection d);

	void UpdateTexture();

	std::string GetDestination();

	DoorState GetDoorState();

	void SetDoorState(DoorState s);

	DoorDirection GetDoorDirection();

	void OnUpdate(float deltaTime);

private:
	SpriteComponent* sprite;
	CollisionComponent* collision;

	std::string destination;
	DoorState state;
	DoorDirection direction;
};