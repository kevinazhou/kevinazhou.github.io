#include "stdafx.h"
#include "assetManager.h"
#include "Graphics.h"

AssetManager::AssetManager(Graphics* pGraphics)
    : mGraphics(pGraphics)
{
}

AssetManager::~AssetManager()
{
}

void AssetManager::Clear()
{
	ShaderCache.Clear();
	MeshCache.Clear();
	TextureCache.Clear();
}