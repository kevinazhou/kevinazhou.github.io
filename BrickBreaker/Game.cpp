//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//  Implementation by Kevin Zhou

#include "Game.h"
#include "Actor.h"
#include "Random.h"
#include "Block.h"
#include "Paddle.h"
#include "Ball.h"
#include "SpriteComponent.h"
#include <fstream>
#include <algorithm>
#include <SDL/SDL_image.h>
//#include <stdio.h>



Game::Game()
{
	
}

bool Game::Initialize()
{
	bool ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);//initializing SDL

	//Random::Init();
	

	if (!ret)
	{
		window = SDL_CreateWindow("Brick Breaker",
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
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);//blue

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
	Actor* background = new Actor(this);
	background->SetPosition(Vector2(512, 384));
	SpriteComponent* backSprite = new SpriteComponent(background, 50);
	backSprite->SetTexture(GetTexture("Assets/Background.png"));

	Paddle* paddle = new Paddle(this);
	pad = paddle;
	paddle->SetPosition(Vector2(512, 752));//MAKE SURE IS FULLY CENTERED AT START

	Ball* ball = new Ball(this);
	ball->SetPosition(Vector2(512, 700));

	LoadLevel("Assets/Level.txt");
}

Paddle* Game::GetPaddle()
{
	return pad;
}

void Game::LoadLevel(std::string s)
{
	std::ifstream in(s);
	std::string line;
	int height = 0;
	while (std::getline(in, line))
	{
		for (int i = 0; i < line.size(); i++)//k
		{
			if (line[i] == 'A')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector2(64 + (i * 64), 48 + (height * 32)));//CHANGE EVERYTHING TO USE GETTEXWIDTH
				b->changeTexture(GetTexture("Assets/BlockA.png"));
			}
			else if (line[i] == 'B')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector2(64 + (i * 64), 48 + (height * 32)));
				b->changeTexture(GetTexture("Assets/BlockB.png"));
			}
			else if (line[i] == 'C')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector2(64 + (i * 64), 48 + (height * 32)));
				b->changeTexture(GetTexture("Assets/BlockC.png"));
			}
			else if (line[i] == 'D')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector2(64 + (i * 64), 48 + (height * 32)));
				b->changeTexture(GetTexture("Assets/BlockD.png"));
			}
			else if (line[i] == 'E')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector2(64 + (i * 64), 48 + (height * 32)));
				b->changeTexture(GetTexture("Assets/BlockE.png"));
			}
			else if (line[i] == 'F')
			{
				Block* b = new Block(this);
				b->SetPosition(Vector2(64 + (i * 64), 48 + (height * 32)));
				b->changeTexture(GetTexture("Assets/BlockF.png"));
			}

		}

		height++;
	}
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