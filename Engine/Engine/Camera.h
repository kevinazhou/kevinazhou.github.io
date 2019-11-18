#pragma once
#include <d3d11.h>
#include "engineMath.h"

class Graphics;
class Matrix4;

struct PerCameraConstants
{
	Matrix4 mat;
	Vector4 camPos;
};

class Camera
{
public:
	Camera(Graphics* graphics);
	virtual ~Camera();

	void SetActive();

	Matrix4 mWorldToCamera;
	Matrix4 mProjectionMatrix;

protected:
	Graphics* mGraphics = nullptr;
	PerCameraConstants PCCInstance;
	ID3D11Buffer* mConstantBuffer = nullptr;
};