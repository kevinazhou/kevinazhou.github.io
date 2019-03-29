#pragma once
#include "VehicleMove.h"
#include "Game.h"
#include "Math.h"


class SideBlock;
class PlayerMove : public VehicleMove
{
public:
	PlayerMove(class Actor* owner);
	void Update(float deltaTime);
	void ProcessInput(const Uint8* keyState);
	void OnLapChange(int newLap);

private:

	Game* g;


};