#pragma once
#include <vector>
#include <unordered_map>
#include "SDL/SDL.h"
#include "Math.h"
#include "SDL/SDL_mixer.h"

// TODO
class Actor;
class SpriteComponent;
class CollisionComponent;
class Player;
class Block;
class Goomba;

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

	void AddGoomba(Goomba* g);

	void RemoveGoomba(Goomba* g);

	SDL_Texture* GetTexture(std::string str);

	Vector2 GetCamera();

	void SetCamera(Vector2 cam);

	std::vector<Block*> BlockVector;

	std::vector<Goomba* > GoombaVector;

	Player* GetPlayer();

	Mix_Chunk* GetSound(const std::string& filename);

	int GetChannel();
	

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
	void LoadLevel(std::string s);

	std::vector<Actor* > ActorVector;

	std::unordered_map<std::string, SDL_Texture* > textureMap;

	std::unordered_map<std::string, Mix_Chunk* > soundMap;

	std::vector<SpriteComponent*> SpriteVector;

	int musicInt;

	bool GameCondition;
	SDL_Window *window;
	SDL_Renderer *renderer;
	int WallThickness = 10;

	Vector2 Camera;

	unsigned int lastTime;

	Player* player;

	SDL_Point ballVelocity;
};