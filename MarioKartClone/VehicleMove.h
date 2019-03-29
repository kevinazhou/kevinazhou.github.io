#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

enum class TurnState {
	Left,
	Right,
	None
};
class VehicleMove : public Component
{
public:
	VehicleMove(class Actor* owner);
	void Update(float deltaTime);

	void pressPedal();
	void unpressPedal();
	void turnRight();
	void turnLeft();
	void noTurn();

	void setEnemyRampTime(float val)
	{
		accelRampTime = val;
	}
	void setEnemyMin(float val)
	{
		minAccel = val;
	}
	void setEnemyMax(float val)
	{
		maxAccel = val;
	}

	virtual void OnLapChange(int newLap) { }

	float distToNextCheckpoint();

	int GetCurrentLap()
	{
		return currentLap;
	}

	int GetLastCheckpoint()
	{
		return lastCheckpoint;
	}
private:
	bool pedalPressed = false;
	TurnState turningState = TurnState::None;
	Vector3 velocity;
	float angularVelocity = 0.0f;
	float accelTime = 0.0f;

	std::vector<std::vector<int> > checkpoints;
	int currentLap = 0;
	int lastCheckpoint = -1;

	//parameters
	float minAccel = 1000.0f;
	float maxAccel = 2500.0f;
	float accelRampTime = 2.0f;
	float angularAccel = 5 * Math::Pi;
	float drag = 0.9f;
	float dragWhenNotPressed = 0.975f;
	float angularDrag = 0.9f;
};