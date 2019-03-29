//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "HeightMap.h"
#include "Enemy.h"


Game::Game()
:mIsRunning(true)
{
	
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// TODO: Create renderer
	mRenderer = new Renderer(this);
	
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		SDL_Log("Unable to initialize renderer: %s", SDL_GetError());
		return false;
	}

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.01667f)
	{
		deltaTime = 0.01667f;
	}
	mTicksCount = SDL_GetTicks();
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}

	startup -= deltaTime;
	if (startup <= 0.0f && !started)
	{
		enemy->SetState(ActorState::Active);
		player->SetState(ActorState::Active);
		started = true;
		musicInt = Mix_PlayChannel(-1, musicSound, -1);
	}
}

void Game::GenerateOutput()
{
	//TODO: tell renderer to dra
	mRenderer->Draw();
}

void Game::LoadData()
{
	heightMap = new HeightMap();
	//heightMap must be created before player is
	player = new Player(this);

	enemy = new Enemy(this);
	player->SetState(ActorState::Paused);
	enemy->SetState(ActorState::Paused);

	Matrix4 perspective = Matrix4::CreatePerspectiveFOV(1.22f, 1024.0f, 768.0f, 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(perspective);
	
	Vector3 eyePos = Vector3(-300, 0, 100);
	Vector3 eyeTarget = Vector3(20, 0, 0);
	Matrix4 view = Matrix4::CreateLookAt(eyePos, eyeTarget, Vector3::UnitZ);
	//mRenderer->SetViewMatrix(view);

	Actor* track = new Actor(this);
	track->SetRotation(Math::Pi);
	MeshComponent* trackMesh = new MeshComponent(track);
	trackMesh->SetMesh(GetRenderer()->GetMesh("Assets/Track.gpmesh"));

	
	fallSound = GetSound("Assets/Sounds/Fall.wav");
	finalLapSound = GetSound("Assets/Sounds/FinalLap.wav");
	lostSound = GetSound("Assets/Sounds/Lost.wav");
	musicSound = GetSound("Assets/Sounds/Music.ogg");
	musicFastSound = GetSound("Assets/Sounds/MusicFast.ogg");
	raceStartSound = GetSound("Assets/Sounds/RaceStart.wav");
	wonSound = GetSound("Assets/Sounds/Won.wav");

	Mix_PlayChannel(-1, raceStartSound, 0);
}

Player* Game::GetPlayer()
{
	return player;
}

void Game::AddBlock(Block* b)
{
	BlockVector.push_back(b);
}

void Game::RemoveBlock(Block* b)
{
	std::vector<Block* >::iterator it = std::find(BlockVector.begin(), BlockVector.end(), b);
	if (it != BlockVector.end())
	{
		BlockVector.erase(it);
	}
}

void Game::LoadBlocks(std::string filename, float xPos)
{
	std::ifstream in(filename);
	std::string line;

	int height = 0;
	while (std::getline(in, line))//read in line by line
	{
		for (size_t i = 0; i < line.size(); i++)
		{
			/*if (line[i] == 'A')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector3(xPos, -237.5 + (i*25.0f), 237.5 - (height*25.0f)));
				b->isExploding = false;
			}
			else if (line[i] == 'B')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector3(xPos, -237.5 + (i*25.0f), 237.5 - (height*25.0f)));
				b->isExploding = true;
				b->SetExplodingTex();
			}*/
		}

		height++;
	}
}

std::vector<class Block*> Game::GetBlockVec()
{
	return BlockVector;
}



void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	mRenderer->Shutdown();
	delete mRenderer;

	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}
