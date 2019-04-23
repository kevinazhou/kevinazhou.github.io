#pragma once
#include "Actor.h"

class CollisionComponent;
class MeshComponent;
class Checkpoint : public Actor
{
public:
	Checkpoint(class Game* game, Actor* parent);
	~Checkpoint();
	void OnUpdate(float deltaTime);
	void ChangeTexture(int i);

	//for setting level string in levelloader
	void SetLevelString(std::string s) { mLevelString = s; }

	//for setting the text in levelloader
	void SetText(std::string s) { mCheckpointText = s; }
private:
	CollisionComponent* collision;
	MeshComponent* mesh;
	bool isActive = false;
	std::string mLevelString;

	std::string mCheckpointText;
};