//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//	Implementation by Kevin Zhou

#include "Game.h"
#include "Actor.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "Player.h"
#include "Spawner.h"
#include "TiledBGComponent.h"
#include "Key.h"
#include "Door.h"
#include "Bat.h"
#include "Skeleton.h"
#include "Boss.h"
#include "Triforce.h"
#include <fstream>
#include <algorithm>
#include <SDL/SDL_image.h>
#include <sstream>
#include <iostream>
//#include <stdio.h>



Game::Game()
{
	
}

bool Game::Initialize()
{
	bool ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);//initializing SDL

	//Random::Init();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	

	if (!ret)
	{
		window = SDL_CreateWindow("Zelda is a guy and you play as him",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			600,
			448,
			SDL_WINDOW_OPENGL);

		gameWidth = 600;
		gameHeight = 448;

		renderer = SDL_CreateRenderer(window,
			-1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		int flags = IMG_INIT_PNG;
		int initImg = IMG_Init(flags);
		//can do checking here

		LoadData();
		lastTime = SDL_GetTicks();

		if (window != nullptr && renderer != nullptr)
		{
			return true;
		}

		else
		{
			return false;
		}
	}

	else
	{
		return false;
	}


	
}


void Game::RunLoop()
{
	GameCondition = true;
	while (GameCondition)//the game loop
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
		{
			GameCondition = false;
		}
	}

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		GameCondition = false;
	}

	std::vector<Actor* > copyVec = ActorVector;

	for (size_t i = 0; i < ActorVector.size(); i++)
	{
		ActorVector[i]->ProcessInput(state);
	}


}

void Game::UpdateGame()
{
	unsigned int currentTime = SDL_GetTicks();
	while (currentTime - lastTime < 16)//frame limiting
	{
		currentTime = SDL_GetTicks();
	}

	float timeElapsed = ((float)(currentTime - lastTime) / 1000);//1000 ms in a second

	lastTime = currentTime;

	if (timeElapsed > 0.033)//capping max delta time
	{
		timeElapsed = 0.033f;
	}
	//char fram[16];
	//sprintf_s(fram, "%f", timeElapsed);
	//SDL_Log(fram);

	std::vector<Actor* > copyVec = ActorVector;
	for (size_t i = 0; i < copyVec.size(); i++)
	{
		copyVec[i]->Update(timeElapsed);
	}

	std::vector<Actor* > deleteVec;

	for (size_t i = 0; i < copyVec.size(); i++)
	{
		if (copyVec[i]->GetState() == ActorState::Destroy)
		{
			deleteVec.push_back(copyVec[i]);
		}
	}

	for (size_t d = 0; d < deleteVec.size(); d++)
	{
		delete deleteVec[d];
	}




}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);//blue

	SDL_RenderClear(renderer);

	for (size_t i = 0; i < SpriteVector.size(); i++)
	{
		if (SpriteVector[i]->IsVisible())
		{
			SpriteVector[i]->Draw(renderer);
		}
		
	}



	SDL_RenderPresent(renderer);
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	Mix_CloseAudio();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::AddActor(Actor* a)
{
	ActorVector.push_back(a);
}

void Game::AddBlock(Block* b)
{
	BlockVector.push_back(b);
}


void Game::RemoveActor(Actor* a)
{
	std::vector<Actor* >::iterator it = std::find(ActorVector.begin(), ActorVector.end(), a);
	if (it != ActorVector.end())
	{
		ActorVector.erase(it);
	}
	
}

void Game::RemoveBlock(Block* b)
{
	std::vector<Block* >::iterator it = std::find(BlockVector.begin(), BlockVector.end(), b);
	if (it != BlockVector.end())
	{
		BlockVector.erase(it);
	}
}



void Game::LoadData()
{
	

	Actor* Tiled = new Actor(this);
	TiledBGComponent* tileComp = new TiledBGComponent(Tiled);
	tileComp->LoadTileCSV("Assets/Dungeon/DungeonMapBG.csv", 32, 32);
	tileComp->SetTexture(GetTexture("Assets/Dungeon/DungeonTiles.png"));
	
	LoadRoom("Assets/Dungeon/Room1.csv", "Room1");
	LoadRoom("Assets/Dungeon/Room2.csv", "Room2");
	LoadRoom("Assets/Dungeon/Room3.csv", "Room3");
	LoadRoom("Assets/Dungeon/Room4.csv", "Room4");
	LoadRoom("Assets/Dungeon/Room5.csv", "Room5");
	LoadRoom("Assets/Dungeon/Room6.csv", "Room6");
	LoadRoom("Assets/Dungeon/Room7.csv", "Room7");

	currentRoom = "Room1";

	keyCount = 0;

	musicInt = Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
}

int Game::GetChannel()
{
	return musicInt;
}

void Game::LoadRoom(std::string s, std::string roomName)
{
	//reading Room CSV files
	std::ifstream in(s);
	std::string line;
	int currline = 0;

	std::vector<Door* > dorVector;
	std::vector<Spawner* > spawnVector;
	while (std::getline(in, line))
	{
		std::vector<std::string> lineVector;
		std::istringstream ss(line);
		std::string token;
		while (std::getline(ss, token, ','))
		{
			lineVector.push_back(token);//now each line is parsed by commas
		}

		if (lineVector[0] == "Player")
		{
			player = new Player(this);
			Vector2 pos = Vector2(std::stoi(lineVector[1]), std::stoi(lineVector[2]));
			player->SetPosition(pos);
		}

		else if (lineVector[0] == "Collider")
		{
			Collider* coll = new Collider(this);
			int xPos = std::stoi(lineVector[1]) + std::stoi(lineVector[3]) / 2;
			int yPos = std::stoi(lineVector[2]) + std::stoi(lineVector[4]) / 2;
			Vector2 pos = Vector2(xPos, yPos);
			coll->SetPosition(pos);
			coll->SetCollisionDimensions(std::stoi(lineVector[3]), std::stoi(lineVector[4]));
			ColliderVector.push_back(coll);
		}

		else if (lineVector[0] == "SecretBlock")
		{
			SecretBlock* block = new SecretBlock(this);
			int xPos = std::stoi(lineVector[1]) + std::stoi(lineVector[3]) / 2;
			int yPos = std::stoi(lineVector[2]) + std::stoi(lineVector[4]) / 2;
			Vector2 pos = Vector2(xPos, yPos);
			block->SetPosition(pos);
			if (lineVector[6] == "Left")
			{
				block->direction = CollSide::Left;
			}
			else if (lineVector[6] == "Right")
			{
				block->direction = CollSide::Right;
			}
			if (lineVector[6] == "Up")
			{
				block->direction = CollSide::Top;
			}
			if (lineVector[6] == "Down")
			{
				block->direction = CollSide::Bottom;
			}
			SBlockVector.push_back(block);
		}

		else if (lineVector[0] == "Door")
		{
			Door* dor = new Door(this);
			int xPos = std::stoi(lineVector[1]) + std::stoi(lineVector[3]) / 2;
			int yPos = std::stoi(lineVector[2]) + std::stoi(lineVector[4]) / 2;
			Vector2 pos = Vector2(xPos, yPos);
			dor->SetPosition(pos);//position set

			DoorDirection dir;
			if (lineVector[6] == "Right")
			{
				dir = DoorDirection::Right;
			}
			else if (lineVector[6] == "Left")
			{
				dir = DoorDirection::Left;
			}
			else if (lineVector[6] == "Up")
			{
				dir = DoorDirection::Up;
			}
			else if (lineVector[6] == "Down")
			{
				dir = DoorDirection::Down;
			}

			DoorState sta;
			if (lineVector[7] == "Closed")
			{
				sta = DoorState::Closed;
			}
			else if (lineVector[7] == "Locked")
			{
				sta = DoorState::Locked;
			}
			else if (lineVector[7] == "Open")
			{
				sta = DoorState::Open;
			}

			dor->Setup(lineVector[5], sta, dir);

			dorVector.push_back(dor);


		}
		else if(lineVector[0] == "Key" || lineVector[0] == "Bat" || lineVector[0] == "Skeleton" || lineVector[0] == "Boss" || lineVector[0] == "Triforce")
		{
			Spawner* spawn = new Spawner(this);
			int xPos = std::stoi(lineVector[1]) + std::stoi(lineVector[3]) / 2;
			int yPos = std::stoi(lineVector[2]) + std::stoi(lineVector[4]) / 2;
			Vector2 pos = Vector2(xPos, yPos);
			spawn->SetPosition(pos);//position set
			spawn->SetType(lineVector[0]);
			spawnVector.push_back(spawn);
		}

		currline++;

	}

	if (!dorVector.empty())
	{
		doorMap.insert({ roomName, dorVector });
		//std::cout << roomName;
	}
	if (!spawnVector.empty())
	{
		spawnerMap.insert({ roomName, spawnVector });

	}
}

void Game::PlayerTakeDmg(int amount)//cause i dont have a playermove.cpp lmao
{
	player->TakeDamage(amount);
}

void Game::AddEnemy(Actor* a)
{
	std::vector<Actor*> vec = enemyMap[currentRoom];
	vec.push_back(a);
	enemyMap[currentRoom] = vec;
}

void Game::RemoveEnemy(Actor* a)
{
	std::vector<Actor*> vec = enemyMap[currentRoom];
	auto it = std::find(vec.begin(), vec.end(), a);
	if (it != vec.end()) { vec.erase(it); }
	enemyMap[currentRoom] = vec;
}

void Game::TriggerSpawners(std::string room)
{
	std::vector<Spawner* > spawners = spawnerMap[room];
	for (size_t i = 0; i < spawners.size(); i++)
	{
		Vector2 pos = spawners[i]->GetPosition();

		if (spawners[i]->GetType() == "Key")
		{
			Key* k = new Key(this);
			k->SetPosition(pos);
		}
		if (spawners[i]->GetType() == "Bat")
		{
			Bat* b = new Bat(this);
			b->SetPosition(pos);
		}
		if (spawners[i]->GetType() == "Skeleton")
		{
			Skeleton* s = new Skeleton(this);
			s->SetPosition(pos);
		}
		if (spawners[i]->GetType() == "Boss")
		{
			Boss* b = new Boss(this);
			b->SetPosition(pos);
		}
		if (spawners[i]->GetType() == "Triforce")
		{
			Triforce* t = new Triforce(this);
			t->SetPosition(pos);
		}

	}
	for (size_t i = 0; i < spawners.size(); i++)
	{
		spawners[i]->SetState(ActorState::Destroy);
	}
}

std::vector<Door* > Game::GetDoorsInARoom(std::string n)
{
	return doorMap[n];
}

std::vector<Actor* > Game::GetActorsInARoom(std::string n)
{
	return enemyMap[n];
}


void Game::LoadLevel(std::string s)
{
	

	
}

std::vector<SecretBlock* > Game::GetSBlockVector()
{
	return SBlockVector;
}

void Game::UnloadData()
{
	for (size_t i = 0; i<ActorVector.size(); i++)
	{
		delete ActorVector[i];
	}
	ActorVector.clear();
	for (auto it : textureMap)
	{
		SDL_DestroyTexture(it.second);
	}
	textureMap.clear();
	for (auto it : soundMap)
	{
		Mix_FreeChunk(it.second);
	}
	soundMap.clear();

}

SDL_Texture* Game::GetTexture(std::string str)
{
	if (textureMap.find(str) == textureMap.end())//texture not found
	{
		char *arr = &str[0u];

		SDL_Surface* image;
		image = IMG_Load(arr);//error testing if returns nullptr
		if (image == nullptr)
		{
			char* err = "Image file failed to load.";
			SDL_Log(arr);
			SDL_Log(err);
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);

		SDL_FreeSurface(image);

		textureMap.insert({ str, texture });

		return textureMap[str];
	}
	else
	{
		return textureMap[str];
	}
}

Vector2 Game::GetCamera()
{
	return Camera;
}

void Game::SetCamera(Vector2 cam)
{
	Camera = cam;
}


void Game::AddSprite(SpriteComponent* s)
{
	SpriteVector.push_back(s);
	std::sort(SpriteVector.begin(), SpriteVector.end(),
		[](SpriteComponent* a, SpriteComponent* b) {
		return a-> GetDrawOrder() < b-> GetDrawOrder();
	});
}

void Game::RemoveSprite(SpriteComponent* s)
{
	std::vector<SpriteComponent* >::iterator it = std::find(SpriteVector.begin(), SpriteVector.end(), s);
	SpriteVector.erase(it);
}

Player* Game::GetPlayer()
{
	return player;
}

Mix_Chunk* Game::GetSound(const std::string& filename)
{
	if (soundMap.find(filename) == soundMap.end())//sound not found
	{
		const char *arr = filename.c_str();

		Mix_Chunk* sound;
		sound = Mix_LoadWAV(arr);

		if (sound == nullptr)
		{
			char* err = "Sound file failed to load.";
			SDL_Log(arr);
			SDL_Log(err);
		}

		soundMap.insert({ filename, sound });

		return soundMap[filename];
	}
	else
	{
		return soundMap[filename];
	}
}


float Game::GetGameWidth()
{
	return gameWidth;
}
float Game::GetGameHeight()
{
	return gameHeight;
}

std::vector<Collider* > Game::GetColliderVector()
{
	return ColliderVector;
}