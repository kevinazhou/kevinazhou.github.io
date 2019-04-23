#pragma once
#include "Actor.h"

enum class SecurityCameraState
{
	TurningToEnd,
	TurningToStart,
	PausedAtEnd,
	PausedAtStart
};

class SecurityCamera : public Actor
{
public:
	SecurityCamera(class Game* game, Actor* parent);
	~SecurityCamera();
	
	void OnUpdate(float deltaTime);

	void SetStartAndEndQ(Quaternion sq, Quaternion eq)
	{
		startQ = sq;
		endQ = eq;
	}
	void SetInterpPauseTime(float interp, float pause)
	{
		interpTime = interp;
		pauseTime = pause;
	}
private:
	class MeshComponent* mesh;
	class SecurityCone* cone;

	Quaternion startQ;
	Quaternion endQ;
	float interpTime;
	float pauseTime;

	float currentInterpTime = 0.0f;
	float currentPauseTime = 0.0f;
	SecurityCameraState camState = SecurityCameraState::TurningToEnd;

	void SetSoundLoudness(int channel);

	int rotatingSoundInt;
	int detectedSoundInt;
	bool isYellow = false;

	bool firstFrameDone = false;
};