#pragma once
#include "Graphics.h"
#include "engineMath.h"

class Shader;
class RenderObj;
class Camera;
class Texture;
class AssetManager;
class Skeleton;
class Animation;
class JobManager;

//Lighting stuff
const int MAX_POINT_LIGHTS = 8;
struct PointLightData
{
	Vector3 diffuseColor;
	float specularPower;
	Vector3 specularColor;
	float innerRadius;
	Vector3 position;
	float outerRadius;
	bool isEnabled;
	float pad0;
	float pad1;
	float pad2;
};

struct LightingConstants
{
	//padding here
	Vector3 c_ambient;
	float pad;
	PointLightData c_pointLight[MAX_POINT_LIGHTS];
};
//

class Game
{
public:
    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	bool IsKeyHeld(uint32_t key) const;

	PointLightData* AllocateLight();
	void FreeLight(PointLightData* pLight);
	void SetAmbientLight(const Vector3& color);
	const Vector3& GetAmbientLight() const;
	AssetManager* GetAssetManager() const;
	JobManager* GetJobManager() const;
private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	// TODO Lab 02b
	Graphics* mGraphics = nullptr;
	Camera* camera = nullptr;
	
	float rotationAngle = 0.0f;

	bool LoadLevel(const WCHAR* fileName);

	Skeleton* LoadSkeleton(const std::wstring& fileName);
	Animation* LoadAnimation(const std::wstring& fileName);

	//Lighting stuff
	LightingConstants lightConstant;
	ID3D11Buffer* mLightingBuffer = nullptr;

	AssetManager* pAssetManager = nullptr;

	std::vector<RenderObj*> ObjectVector;
	
	JobManager* mJobManager = nullptr;
};