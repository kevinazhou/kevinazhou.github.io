#pragma once
#include "MoveComponent.h"
#include "Game.h"
#include "Math.h"
#include <queue>

class SideBlock;
class PlayerMove : public MoveComponent
{
public:
	PlayerMove(class Actor* owner);
	void Update(float deltaTime);
	void ProcessInput(const Uint8* keyState);

	Vector3 trueVelocity;//after the multiplier is applied
private:
	Vector3 velocity;
	Vector3 input;

	float HDist = 300;
	float VDist = 100;
	float TargetDist = 20;

	Game* g;

	float lastSideBlockX;
	int sidesCreated;

	float lastBlockX;
	int blocksCreated;

	bool spacePressed;

	float speedMultiplier;

	unsigned int EveryTwentySec;
};