#pragma once
#include "Actor.h"


class PlayerMove;
class CameraComponent;
class CollisionComponent;
class HUD;
class Player : public Actor
{
public:
	Player(class Game* game, Actor* parent);
	void SetRespawnLocation(Vector3 loc) { respawn = loc; }
	Vector3 GetRespawnLocation() { return respawn; }

private:

	CameraComponent* camera;
	PlayerMove* move;
	CollisionComponent* collision;
	HUD* hud;
	Vector3 respawn;
	
};