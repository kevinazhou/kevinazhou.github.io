#include "stdafx.h"
#include "Game.h"
#include "engineMath.h"
#include "Graphics.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Camera.h"
#include "RenderObj.h"
#include "RenderCube.h"
#include "Character.h"
#include "assetManager.h"
#include "texture.h"
#include "mesh.h"
#include "JobManager.h"
#include "PointLight.h"
#include "SkinnedObj.h"
#include "jsonUtil.h"
#include "Profiler.h"
#include "stringUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>


// TODO Lab 02e
struct VertexPosColor
{
	Vector3 pos;
	Graphics::Color4 color;
};

struct VertexPosColorTexCoord
{
	Vector3 pos;
	Vector3 normal;
	Graphics::Color4 color;
	Vector2 tex;
};

struct VertexPosNormal
{
	Vector3 pos;
	Vector3 normal;
	Vector2 tex;
};

struct SkeletonInput
{
	Vector3 pos;
	Vector3 normal;
	uint32_t bones;
	uint32_t weight;
	Vector2 tex;
};


Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hWnd, float width, float height)
{
	mGraphics = new Graphics();
	// TODO Lab 02b
	mGraphics->InitD3D(hWnd, width, height);

	pAssetManager = new AssetManager(mGraphics);

	camera = new Camera(mGraphics);
	
	mJobManager = new JobManager();
	mJobManager->Begin();

    // TODO Lab 02e
	VertexPosColor vert[] =
	{
		{ Vector3(0.0f, 0.5f, 0.0f), Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ Vector3(0.45f, -0.5f, 0.0f), Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(-0.45f, -0.5f, 0.0f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) }
	};

	uint16_t triangleVertices[3] = { 0, 1, 2 };

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosColor, color),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Shader* shader = new Shader(mGraphics);
	shader->Load(L"Shaders/Mesh.hlsl", layout, sizeof(layout) / sizeof(layout[0]));
	pAssetManager->SetShader(L"Mesh", shader);

	mLightingBuffer = mGraphics->CreateGraphicsBuffer(&lightConstant, sizeof(lightConstant), D3D11_BIND_CONSTANT_BUFFER,
		D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	D3D11_INPUT_ELEMENT_DESC textureLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColorTexCoord, pos),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColorTexCoord, normal),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColorTexCoord, color),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColorTexCoord, tex),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC lightingLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormal, pos),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormal, normal),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormal, tex),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC skinningLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SkeletonInput, pos),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SkeletonInput, normal),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, offsetof(SkeletonInput, bones),
			  D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(SkeletonInput, weight),
			  D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "VPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(SkeletonInput, tex),
			  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Shader* phongShader = new Shader(mGraphics);
	phongShader->Load(L"Shaders/Phong.hlsl", lightingLayout, sizeof(lightingLayout) / sizeof(lightingLayout[0]));
	pAssetManager->SetShader(L"Phong", phongShader);

	Shader* unlitShader = new Shader(mGraphics);
	unlitShader->Load(L"Shaders/Unlit.hlsl", lightingLayout, sizeof(lightingLayout) / sizeof(lightingLayout[0]));
	pAssetManager->SetShader(L"Unlit", unlitShader);

	Shader* textureShader = new Shader(mGraphics);
	textureShader->Load(L"Shaders/BasicMesh.hlsl", textureLayout, sizeof(textureLayout) / sizeof(textureLayout[0]));
	pAssetManager->SetShader(L"BasicMesh", textureShader);

	Shader* skinningShader = new Shader(mGraphics);
	skinningShader->Load(L"Shaders/Skinned.hlsl", skinningLayout, sizeof(skinningLayout) / sizeof(skinningLayout[0]));
	pAssetManager->SetShader(L"Skinned", skinningShader);

	LoadLevel(L"Assets/Levels/Level07v2.itplevel");

	LoadSkeleton(L"Assets/Anims/SK_Mannequin.itpskel");
	LoadAnimation(L"Assets/Anims/ThirdPersonIdle.itpanim2");

}

void Game::Shutdown()
{

	delete camera;
	mLightingBuffer->Release();
	pAssetManager->Clear();

	for(int i = 0; i < ObjectVector.size(); i++)
	{
		delete ObjectVector[i];
	}

	mGraphics->CleanD3D();
	delete mGraphics;

	delete pAssetManager;
	
	mJobManager->End();
	delete mJobManager;
}

void Game::Update(float deltaTime)
{
	Profiler::Get()->ResetAll();
	for (int i = 0; i < ObjectVector.size(); i++)
	{
		ObjectVector[i]->Update(deltaTime);
	}
	
	mJobManager->WaitForJobs();
}

void Game::RenderFrame()
{
	// TODO Lab 02b
	{
		PROFILE_SCOPE(BeginFrame);
		mGraphics->BeginFrame(Graphics::Color4(0.0f, 0.2f, 0.4f, 1.0f));
	}
	

	camera->SetActive();

	//Lighting stuff
	{
		PROFILE_SCOPE(UploadBuffer);
		mGraphics->UploadBuffer(mLightingBuffer, &lightConstant, sizeof(lightConstant));

	}
	mGraphics->GetDeviceContext()->PSSetConstantBuffers(2, 1, &mLightingBuffer);
	//
	{
		PROFILE_SCOPE(DrawingObjects);
		for (int i = 0; i < ObjectVector.size(); i++)
		{
			ObjectVector[i]->Draw();
		}
	}
	
	{
		PROFILE_SCOPE(EndFrame);
		mGraphics->EndFrame();
	}
}

void Game::OnKeyDown(uint32_t key)
{
	m_keyIsHeld[key] = true;
}

void Game::OnKeyUp(uint32_t key)
{
	m_keyIsHeld[key] = false;
}

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
}

bool Game::LoadLevel(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 2)
	{
		return false;
	}

	//loading camera data
	Vector3 CamPosition;
	Quaternion CamRotation;
	const rapidjson::Value& cam = doc["camera"];

	GetVectorFromJSON(cam, "position", CamPosition);
	GetQuaternionFromJSON(cam, "rotation", CamRotation);

	Matrix4 CamMatrix = Matrix4::CreateFromQuaternion(CamRotation) * Matrix4::CreateTranslation(CamPosition);
	CamMatrix.Invert();

	camera->mWorldToCamera = CamMatrix;

	//loading lighting data
	const rapidjson::Value& lightingData = doc["lightingData"];
	GetVectorFromJSON(lightingData, "ambient", lightConstant.c_ambient);

	//loading render objects
	const rapidjson::Value& renderObject = doc["renderObjects"];
	for (rapidjson::SizeType i = 0; i < renderObject.Size(); i++)
	{
		Vector3 objPos;
		Quaternion objRot;
		float objScale;
		std::wstring meshName;

		GetVectorFromJSON(renderObject[i], "position", objPos);
		GetQuaternionFromJSON(renderObject[i], "rotation", objRot);
		GetFloatFromJSON(renderObject[i], "scale", objScale);
		GetWStringFromJSON(renderObject[i], "mesh", meshName);

		Matrix4 Scale = Matrix4::CreateScale(objScale);
		Matrix4 Rotation = Matrix4::CreateFromQuaternion(objRot);
		Matrix4 Translation = Matrix4::CreateTranslation(objPos);
		Matrix4 result = Scale * Rotation * Translation;

		Mesh* mesh = pAssetManager->LoadMesh(meshName);
		RenderObj* rend = nullptr;
		SkinnedObj* skinned = nullptr;

		if (mesh->IsSkinned())
		{
			skinned = new SkinnedObj(mGraphics, mesh);
			skinned->mObjectData->mat = result;
			ObjectVector.push_back(skinned);
		}
		else
		{
			rend = new RenderObj(mGraphics, mesh);
			rend->mObjectData->mat = result;
			ObjectVector.push_back(rend);
		}

		const rapidjson::Value& components = renderObject[i]["components"];
		for (rapidjson::SizeType j = 0; j < components.Size(); j++)
		{
			//Component* light = new PointLight(this, ObjectVector[i]);
			//light->LoadProperties(components[j]);
			//ObjectVector[i]->AddComponent(light);
			const rapidjson::Value& comp = components[j];
			std::string type = comp["type"].GetString();
			Component* component = nullptr;
			if (type == "PointLight")
			{
				component = new PointLight(this, rend);
				component->LoadProperties(components[j]);
			}
			else if (type == "Character")
			{
				component = new Character(skinned, this);
				component->LoadProperties(components[j]);
			}
			if (component) ObjectVector[i]->AddComponent(component);
		}
	}


	return true;
}

AssetManager* Game::GetAssetManager() const
{
	return pAssetManager;
}

Skeleton* Game::LoadSkeleton(const std::wstring& fileName)
{
	return pAssetManager->LoadSkeleton(fileName);
}

Animation* Game::LoadAnimation(const std::wstring& fileName)
{
	return pAssetManager->LoadAnimation(fileName);
}

JobManager* Game::GetJobManager() const
{
	return mJobManager;
}

PointLightData* Game::AllocateLight()
{
	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if (lightConstant.c_pointLight[i].isEnabled == false)
		{
			lightConstant.c_pointLight[i].isEnabled = true;
			return &lightConstant.c_pointLight[i];
		}
	}
	return nullptr;
}

void Game::FreeLight(PointLightData* pLight)
{
	pLight->isEnabled = false;
}

void Game::SetAmbientLight(const Vector3& color)
{
	lightConstant.c_ambient.x = color.x;
	lightConstant.c_ambient.y = color.y;
	lightConstant.c_ambient.z = color.z;
}

const Vector3& Game::GetAmbientLight() const
{
	return lightConstant.c_ambient;
}
