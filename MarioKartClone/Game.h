#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
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

	class Player* GetPlayer();

	Mix_Chunk* GetSound(const std::string& fileName);

	class Renderer* GetRenderer() {	return mRenderer; }

	void AddBlock(class Block* b);

	void RemoveBlock(class Block* b);

	void LoadBlocks(std::string filename, float xPos);

	std::vector<class Block*> GetBlockVec();
	
	int shipSound;

	class HeightMap* heightMap;

	class Enemy* GetEnemy()
	{
		return enemy;
	}

	int musicInt;

	Mix_Chunk* fallSound;
	Mix_Chunk* finalLapSound;
	Mix_Chunk* lostSound;
	Mix_Chunk* musicSound;
	Mix_Chunk* musicFastSound;
	Mix_Chunk* raceStartSound;
	Mix_Chunk* wonSound;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// All the actors in the game
	std::vector<class Actor*> mActors;

	std::vector<class Block*> BlockVector;

	class Player* player;

	class Enemy* enemy;

	class Renderer* mRenderer;

	Uint32 mTicksCount;
	bool mIsRunning;

	float startup = 8.5f;
	bool started = false;
	

};
