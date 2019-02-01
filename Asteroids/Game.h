#pragma once
#include <vector>
#include <unordered_map>
#include "SDL/SDL.h"

// TODO
class Actor;
class Asteroid;
class SpriteComponent;

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

	SDL_Texture* GetTexture(std::string str);
	
	void AddAsteroid(Asteroid* a);

	void RemoveAsteroid(Asteroid* a);

	std::vector<Asteroid* > AsteroidVector;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();
	

	std::vector<Actor* > ActorVector;

	std::unordered_map<std::string, SDL_Texture* > textureMap;

	std::vector<SpriteComponent*> SpriteVector;

	bool GameCondition;
	SDL_Window *window;
	SDL_Renderer *renderer;
	int WallThickness = 10;

	unsigned int lastTime;

	

	SDL_Point ballVelocity;
};