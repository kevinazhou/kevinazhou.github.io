#include "stdafx.h"
#include "RenderCube.h"
#include "assetManager.h"
#include "VertexBuffer.h"
#include "mesh.h"

RenderCube::RenderCube(AssetManager* pAssetManager, const Shader* pShader, const Texture* pTex)
	:RenderObj(pAssetManager->GetGraphics(), nullptr)
{
	mAssetManager = pAssetManager;
	

	static VertexPosColorTex cubeVertex[] =
	{//red/
	{ Vector3(-0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) },
	{ Vector3(0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f) },
	{ Vector3(0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
	{ Vector3(-0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f) },
	//blue/
	{ Vector3(0.5f, 0.5f, -0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
	{ Vector3(0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f) },
	{ Vector3(0.5f, -0.5f, 0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f) },
	{ Vector3(0.5f, -0.5f, -0.5f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f) },

	//side, green/
	{ Vector3(-0.5f, 0.5f, -0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f),  Vector2(0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f) },
	{ Vector3(-0.5f, 0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f) },
	{ Vector3(-0.5f, -0.5f, 0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f) },
	{ Vector3(-0.5f, -0.5f, -0.5f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f) },
	//front, yellow/
	{ Vector3(-0.5f, 0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f) },
	{ Vector3(0.5f, 0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f) },
	{ Vector3(0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) },
	{ Vector3(-0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f) },
	//top, white
	{ Vector3(0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f,1.0f,0.0f) },
	{ Vector3(-0.5f, 0.5f, -0.5f), Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f,1.0f,0.0f) },
	{ Vector3(-0.5f, 0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f,1.0f,0.0f) },
	{ Vector3(0.5f, 0.5f, 0.5f), Graphics::Color4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f,1.0f,0.0f) },
	//bottom, orange
	{ Vector3(-0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.5f, 0.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f,-1.0f,0.0f) },
	{ Vector3(-0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 0.5f, 0.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f,-1.0f,0.0f) },
	{ Vector3(0.5f, -0.5f, 0.5f), Graphics::Color4(1.0f, 0.5f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f,-1.0f,0.0f) },
	{ Vector3(0.5f, -0.5f, -0.5f), Graphics::Color4(1.0f, 0.5f, 0.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f,-1.0f,0.0f) }

	};

	static uint16_t cubeIndex[] =
	{

	2, 1, 0,
	3, 2, 0,

	6, 5, 4,
	7, 6, 4,

	10, 8, 9,
	11, 8, 10,

	14, 12, 13,
	15, 12, 14,

	18, 17, 16,
	19, 18, 16,

	21, 23, 20,
	22, 23, 21

	};



	mObjectData = new PerObjectConstants();
	mObjectData->mat = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));

	VertexBuffer* rVertexBuffer = new VertexBuffer(rGraphics, cubeVertex, (uint32_t)(sizeof(cubeVertex) / sizeof(cubeVertex[0])), (uint32_t)sizeof(cubeVertex[0]), cubeIndex,
		((uint32_t)sizeof(cubeIndex) / sizeof(cubeIndex[0])), (uint32_t)(sizeof(cubeIndex[0])));

	mMesh = new Mesh(pAssetManager, rVertexBuffer, pShader);

	mObjectBuffer = rGraphics->CreateGraphicsBuffer(nullptr, sizeof(Matrix4), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

}

RenderCube::~RenderCube()
{
	delete mMesh;
}
