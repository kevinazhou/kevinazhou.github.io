#pragma once
#include <d3d11.h>
#include "Graphics.h"
#include "texture.h"
#include "engineMath.h"

struct PerObjectConstants
{
public:
	Matrix4 mat;
};


class Graphics;
class VertexBuffer;
class Component;
class Shader;
class Mesh;

class RenderObj
{
public:


	RenderObj();
	RenderObj(Graphics* graphics, const Mesh* pMesh);
	virtual ~RenderObj();
	virtual void Draw();
	void AddComponent(Component* pComp) { mComponents.push_back(pComp); }

	virtual void Update(float deltaTime);

	Graphics* rGraphics = nullptr;

	PerObjectConstants* mObjectData = nullptr;
	ID3D11Buffer* mObjectBuffer = nullptr;

	const Mesh* mMesh = nullptr;

	std::vector<Component*> mComponents;
};