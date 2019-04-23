#pragma once
#include "Actor.h"

class SecurityCone : public Actor
{
public:
	SecurityCone(class Game* game, Actor* parent);
	void OnUpdate(float deltaTime);
	bool IsConeNotWhite() { return isCurrentlyNotWhite; }
private:
	class MeshComponent* mesh;
	float timeYellow;
	bool PlayerDetected = false;
	bool DetectPlayer();
	bool KillPlayer = false;
	float HalfSecToKill = 0.0f;

	bool isCurrentlyNotWhite = false;

	
	
};