#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include "Math.h"

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void SetPlayer(class Player* play);

	class Player* GetPlayer();

	Mix_Chunk* GetSound(const std::string& fileName);

	class Renderer* GetRenderer() {	return mRenderer; }

	void AddBlock(class Actor* b);

	void RemoveBlock(class Actor* b);

	void AddCamera(class SecurityCamera* c);

	void RemoveCamera(class SecurityCamera* c);

	void LoadBlocks(std::string filename, float xPos);

	std::vector<class Actor*> GetBlockVec();

	std::vector<class SecurityCamera*> GetCameraVec();
	
	void AddCheckpoint(class Checkpoint* c);

	void RemoveFrontCheckpoint();

	class Checkpoint* NextCheckpoint();

	void SetNextLevel(std::string s) { mNextLevel = s;  }

	void LoadNextLevel();

	float time = 0.0f;

	int coins = 0;

	float textTimer = 0.0f;//timer for the checkpoint text

	class Texture* mCheckpointText;

	//for loading into levels without a checkpoint at first point
	bool GameJustStarted = true;
	
	float wait = 0.0f;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// All the actors in the game
	std::vector<class Actor*> mActors;
	//all the blocks in the game
	std::vector<class Actor*> blocks;
	//all the security cameras in the game
	std::vector<class SecurityCamera*> cameras;

	std::queue<class Checkpoint*> checkpoints;

	class Player* player;

	class Renderer* mRenderer;

	Uint32 mTicksCount;
	bool mIsRunning;

	std::string mNextLevel;
	

};
