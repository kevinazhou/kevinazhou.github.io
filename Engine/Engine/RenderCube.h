#pragma once
#include <cstdint>
#include "Graphics.h"
#include "RenderObj.h"

//class AssetManager;


// Texture stuff
struct VertexPosColorTex
{
	Vector3 pos;
	Graphics::Color4 color;
	Vector2 tex;
	Vector3 normal;
};
//

class RenderCube : public RenderObj
{
public:
	RenderCube(AssetManager* pAssetManager, const Shader* pShader, const Texture* pTex);
	~RenderCube();

	AssetManager* mAssetManager = nullptr;

};