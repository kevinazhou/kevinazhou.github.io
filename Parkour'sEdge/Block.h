#pragma once
#include "Actor.h"

class MeshComponent;
class CollisionComponent;
class Block : public Actor
{
public:
	Block(class Game* game, Actor* parent);
	~Block();
	void SetMirrorStatus(bool b)
	{
		isMirror = b;
	}

	void OnUpdate(float deltaTime);

	const bool GetMirrorStatus()
	{
		return isMirror;
	}

	void SetRotatingStatus(bool b)
	{
		isRotating = b;
	}

	const bool GetRotatingStatus()
	{
		return isRotating;
	}

private:
	MeshComponent* mesh;
	CollisionComponent* collision;
	bool isMirror = false;
	bool isRotating;
};