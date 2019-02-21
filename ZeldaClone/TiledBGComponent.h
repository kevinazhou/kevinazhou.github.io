#pragma once
#include "SpriteComponent.h"
#include "SDL/SDL.h"
#include <string>
#include <vector>

class TiledBGComponent : public SpriteComponent
{
public:

	TiledBGComponent(class Actor* owner, int drawOrder = 50);

	void Draw(SDL_Renderer* renderer);

	void LoadTileCSV(const std::string& fileName, int tileWidth, int tileHeight);

	std::vector<std::vector<int> > GetVectorVector() {
		return vectorVector;
	}

private:
	int tWidth;
	int tHeight;
	std::vector<std::vector<int> > vectorVector;
};