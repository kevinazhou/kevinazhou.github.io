#pragma once
#include "VehicleMove.h"
#include <vector>

class EnemyMove : public VehicleMove
{
public:
	EnemyMove(class Actor* owner);
	void Update(float deltaTime);

private:
	std::vector<Vector3> routeVector;
	int nextTarget;
	int numTargets = 0;
};