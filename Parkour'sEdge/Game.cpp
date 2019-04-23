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
#include "LevelLoader.h"
#include "Arrow.h"
#include <SDL/SDL_ttf.h>

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

	Mix_AllocateChannels(32);
	Mix_GroupChannels(22, 31, 1);

	TTF_Init();

	LoadData();

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GetRelativeMouseState(nullptr, nullptr);

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
		if (!mNextLevel.empty())
		{//move onto next level
			LoadNextLevel();
		}
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
	wait += deltaTime;
		if (wait < 6.0f)
		{
			return;
		}
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


}

void Game::GenerateOutput()
{
	//TODO: tell renderer to dra
	mRenderer->Draw();
}

void Game::LoadData()
{


	Matrix4 perspective = Matrix4::CreatePerspectiveFOV(1.22f, 1024.0f, 768.0f, 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(perspective);
	
	LevelLoader::Load(this, "Assets/Tutorial.json");

	Arrow* checkpointArrow = new Arrow(this, nullptr);
	Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
}

void Game::SetPlayer(class Player* play)
{
	player = play;
}

Player* Game::GetPlayer()
{
	return player;
}

void Game::AddBlock(Actor* b)
{
	blocks.push_back(b);
}

void Game::RemoveBlock(Actor* b)
{
	std::vector<Actor* >::iterator it = std::find(blocks.begin(), blocks.end(), b);
	if (it != blocks.end())
	{
		blocks.erase(it);
	}
}

void Game::AddCamera(class SecurityCamera* c)
{
	cameras.push_back(c);
}

void Game::RemoveCamera(class SecurityCamera* c)
{
	std::vector<SecurityCamera* >::iterator it = std::find(cameras.begin(), cameras.end(), c);
	if (it != cameras.end())
	{
		cameras.erase(it);
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

std::vector<class Actor*> Game::GetBlockVec()
{
	return blocks;
}

std::vector<class SecurityCamera*> Game::GetCameraVec()
{
	return cameras;
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
	mActors.push_back(actor);
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

void Game::AddCheckpoint(class Checkpoint* c)
{
	checkpoints.push(c);
}

void Game::RemoveFrontCheckpoint()
{
	checkpoints.pop();
}

class Checkpoint* Game::NextCheckpoint()
{
	if (checkpoints.empty())
	{
		return nullptr;
	}
	return checkpoints.front();
}

void Game::LoadNextLevel()
{
	//Delete all the actors in the world
	while (!mActors.empty())
	{
		delete mActors[0];//actor destructor removes from the vector
	}

	//clear out the checkpoint queue just in case
	while (!checkpoints.empty())
	{
		checkpoints.pop();
	}

	LevelLoader::Load(this, mNextLevel);

	//don't need to activate first checkpoint, as it is done
	//automatically in Checkpoint::OnUpdate

	//allocate new arrow
	Arrow* checkpointArrow = new Arrow(this, nullptr);

	mNextLevel.clear();

}