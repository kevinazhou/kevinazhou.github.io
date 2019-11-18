#pragma once
#include "Shader.h"
#include "texture.h"
#include "mesh.h"
#include "assetCache.h"
#include "Skeleton.h"
#include "Animation.h"

class Graphics;

class AssetManager
{
public:
    AssetManager(Graphics* pGraphics);
    ~AssetManager();

    void Clear();

    Graphics* GetGraphics() { return mGraphics; }

	//ShaderCache functions
	Shader* GetShader(const std::wstring& shaderName) { return ShaderCache.Get(shaderName); }

	void SetShader(const std::wstring& shaderName, Shader* pShader) { ShaderCache.Cache(shaderName, pShader); }

	//TextureCache functions
	Texture* LoadTexture(const std::wstring& fileName) { return TextureCache.Load(fileName); }

	//MeshCache function
	Mesh* GetMesh(const std::wstring& meshName) { return MeshCache.Get(meshName); }

	void SetMesh(const std::wstring& meshName, Mesh* pMesh) { MeshCache.Cache(meshName, pMesh); }

	Mesh* LoadMesh(const std::wstring& fileName) { return MeshCache.Load(fileName); }

	Skeleton* LoadSkeleton(const std::wstring& fileName) { return SkeletonCache.Load(fileName); }

	Animation* LoadAnimation(const std::wstring& fileName) { return AnimationCache.Load(fileName); }

private:
    Graphics* mGraphics;

	AssetCache<Shader> ShaderCache = AssetCache<Shader>(this);
	AssetCache<Texture> TextureCache = AssetCache<Texture>(this);
	AssetCache<Mesh> MeshCache = AssetCache<Mesh>(this);
	AssetCache<Skeleton> SkeletonCache = AssetCache<Skeleton>(this);
	AssetCache<Animation> AnimationCache = AssetCache<Animation>(this);
};
