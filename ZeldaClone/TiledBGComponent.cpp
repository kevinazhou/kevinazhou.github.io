#include "TiledBGComponent.h"
#include "Actor.h"
#include "Game.h"
#include "CSVHelper.h"
#include <fstream>
#include <iostream>


TiledBGComponent::TiledBGComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
{
}

void TiledBGComponent::Draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		Game* g = mOwner->GetGame();
		int textureWidth = static_cast<int>(mTexWidth);
		int textureHeight = static_cast<int>(mTexHeight);

		for (size_t i = 0; i < vectorVector.size(); i++)
		{
			for (size_t j = 0; j < vectorVector[i].size(); j++)
			{
				if (vectorVector[i][j] != -1)
				{
					int sourceXCoord, sourceYCoord;
					SDL_Rect srcRect;
					srcRect.w = tWidth;
					srcRect.h = tHeight;


					int tileNum = vectorVector[i][j];
					int tilesPerRow = textureWidth / tWidth;
					
					int row = tileNum / tilesPerRow;
					int col = tileNum % tilesPerRow;

					sourceXCoord = tWidth * col;
					sourceYCoord = tHeight * row;//where in the source texture to find the tile

					srcRect.x = sourceXCoord;
					srcRect.y = sourceYCoord;

					SDL_Rect destRect;
					destRect.w = tWidth;
					destRect.h = tHeight;
					destRect.x = (j * tWidth) - g->GetCamera().x;
					destRect.y = (i * tHeight) - g->GetCamera().y;

					SDL_RenderCopyEx(renderer,
						mTexture,
						&srcRect,
						&destRect,
						0.0,
						nullptr,
						SDL_FLIP_NONE);


				}


			}
		}

	}
}


void TiledBGComponent::LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight)
{
	std::ifstream in(fileName);
	std::string line;
	
	size_t currentLine = 0;
	while (std::getline(in, line))
	{
		if (line != "")
		{
			std::vector<std::string> lineVector = CSVHelper::Split(line);
			std::vector<int> intVector;
			for (int i = 0; i < lineVector.size(); i++)
			{
				int num = std::stoi(lineVector[i]);
				intVector.push_back(num);
			}
			vectorVector.push_back(intVector);
		}
		//std::cout << line;
		currentLine++;
	}
	tWidth = tileWidth;
	tHeight = tileHeight;
}

