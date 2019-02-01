//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//  Implementation by Kevin Zhou

#include "Game.h"
#include "Actor.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Random.h"
#include "SpriteComponent.h"
#include <algorithm>
#include <SDL/SDL_image.h>
//#include <stdio.h>

// TODO

Game::Game()
{
	
}

bool Game::Initialize()
{
	bool ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);//initializing SDL

	Random::Init();
	

	if (!ret)
	{
		window = SDL_CreateWindow("Asteroids Window",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1024,
			768,
			SDL_WINDOW_OPENGL);

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
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);//black

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
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::AddActor(Actor* a)
{
	ActorVector.push_back(a);
}

void Game::RemoveActor(Actor* a)
{
	std::vector<Actor* >::iterator it = std::find(ActorVector.begin(), ActorVector.end(), a);
	if (it != ActorVector.end())
	{
		ActorVector.erase(it);
	}
	
}

void Game::AddAsteroid(Asteroid* a)
{
	AsteroidVector.push_back(a);
}

void Game::RemoveAsteroid(Asteroid* a)
{
	std::vector<Asteroid* >::iterator it = std::find(AsteroidVector.begin(), AsteroidVector.end(), a);
	if (it != AsteroidVector.end())
	{
		AsteroidVector.erase(it);
	}
	
}

void Game::LoadData()
{
	Ship* sh = new Ship(this);
	sh->SetPosition(Vector2(500, 300));

	Asteroid * one = new Asteroid(this);
	Asteroid * two = new Asteroid(this);
	Asteroid * three = new Asteroid(this);
	Asteroid * four = new Asteroid(this);
	Asteroid * five = new Asteroid(this);
	Asteroid * six = new Asteroid(this);
	Asteroid * seven = new Asteroid(this);
	Asteroid * eight = new Asteroid(this);
	Asteroid * nine = new Asteroid(this);
	Asteroid * ten = new Asteroid(this);


	Actor* sky = new Actor(this);
	sky->SetPosition(Vector2(512, 384));
	SpriteComponent* skyComp = new SpriteComponent(sky, 50);
	skyComp->SetTexture(GetTexture("Assets/Stars.png"));
	
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