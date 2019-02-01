#pragma once
#include <vector>
#include <unordered_map>
#include "SDL/SDL.h"

// TODO
class Actor;
class SpriteComponent;
class CollisionComponent;
class Paddle;
class Block;

class Game {
public:
	Game();

	bool Initialize();

	void Shutdown();

	void RunLoop();

	void AddActor(Actor* a);
	
	void RemoveActor(Actor* a);

	void AddSprite(SpriteComponent* s);

	void RemoveSprite(SpriteComponent* s);

	void AddBlock(Block* b);

	void RemoveBlock(Block* b);

	SDL_Texture* GetTexture(std::string str);

	Paddle* GetPaddle();

	std::vector<Block*> BlockVector;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
	void LoadLevel(std::string s);
	

	std::vector<Actor* > ActorVector;

	std::unordered_map<std::string, SDL_Texture* > textureMap;

	std::vector<SpriteComponent*> SpriteVector;

	bool GameCondition;
	SDL_Window *window;
	SDL_Renderer *renderer;
	int WallThickness = 10;

	unsigned int lastTime;

	Paddle* pad;

	SDL_Point ballVelocity;
};