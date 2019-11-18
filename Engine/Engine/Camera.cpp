#include "stdafx.h"
#include "Camera.h"
#include "Graphics.h"

Camera::Camera(Graphics* graphics)
{
	mGraphics = graphics;
	mWorldToCamera = Matrix4::CreateTranslation(Vector3(0.0f, 0.0f, 100.0f));

	mProjectionMatrix = Matrix4::CreateRotationY(-Math::PiOver2) * Matrix4::CreateRotationZ(-Math::PiOver2) * Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mGraphics->GetScreenWidth(), mGraphics->GetScreenHeight(), 25.0f, 10000.0f);
	mConstantBuffer = mGraphics->CreateGraphicsBuffer(nullptr, sizeof(PCCInstance), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Camera::~Camera()
{
	mConstantBuffer->Release();
}

void Camera::SetActive()
{
	PCCInstance.mat = mWorldToCamera * mProjectionMatrix;

	Matrix4 posTemp = mWorldToCamera;
	posTemp.Invert();
	PCCInstance.camPos = Vector4(posTemp.GetTranslation().x, posTemp.GetTranslation().y, posTemp.GetTranslation().z, 0);


	mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &mConstantBuffer);

	mGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &mConstantBuffer);

	mGraphics->UploadBuffer(mConstantBuffer, &PCCInstance, sizeof(PCCInstance));
}