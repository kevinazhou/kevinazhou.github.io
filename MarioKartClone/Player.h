#pragma once
#include "Actor.h"

class MeshComponent;
class PlayerMove;
class CameraComponent;
class PlayerUI;
class Player : public Actor
{
public:
	Player(class Game* game);
	PlayerUI* ui;
private:
	MeshComponent* mesh;
	CameraComponent* camera;
	PlayerMove* move;
	
};