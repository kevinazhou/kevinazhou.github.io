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
class Door;
class Collider;
class SecretBlock;
class Spawner;


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

	Vector2 GetCamera();

	void SetCamera(Vector2 cam);

	std::vector<Block*> BlockVector;

	Player* GetPlayer();

	Mix_Chunk* GetSound(const std::string& filename);
	
	float GetGameWidth();
	float GetGameHeight();

	std::vector<Collider* > GetColliderVector();

	std::vector<Door* > GetDoorsInARoom(std::string n);

	std::string currentRoom;

	std::vector<SecretBlock* > GetSBlockVector();

	void TriggerSpawners(std::string room);

	void AddEnemy(Actor* a);

	void RemoveEnemy(Actor* a);

	std::vector<Actor* > GetActorsInARoom(std::string n);

	int keyCount;

	void PlayerTakeDmg(int amount);

	int GetChannel();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
	void LoadLevel(std::string s);

	void LoadRoom(std::string s, std::string roomName);

	std::vector<SecretBlock* > SBlockVector;

	std::vector<Actor* > ActorVector;

	std::vector<Collider* > ColliderVector;

	std::unordered_map<std::string, std::vector<Actor*> > enemyMap;

	std::unordered_map<std::string, std::vector<Spawner*> > spawnerMap;

	std::unordered_map<std::string, std::vector<Door*> > doorMap;

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

	float gameWidth;
	float gameHeight;
};