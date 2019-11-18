#include "stdafx.h"
#include "RenderObj.h"
#include "VertexBuffer.h"
#include "Component.h"
#include "Graphics.h"
#include "Shader.h"
#include "mesh.h"

RenderObj::RenderObj()
{


}

RenderObj::RenderObj(Graphics* graphics, const Mesh* pMesh)
{
	mObjectData = new PerObjectConstants();
	mObjectData->mat = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));

	rGraphics = graphics;

	mObjectBuffer = rGraphics->CreateGraphicsBuffer(nullptr, sizeof(Matrix4), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	mMesh = pMesh;
}

RenderObj::~RenderObj()
{
	//delete rVertexBuffer;
	mObjectBuffer->Release();
}

void RenderObj::Draw()
{

	rGraphics->UploadBuffer(mObjectBuffer, &(mObjectData->mat), sizeof(mObjectData->mat));
	rGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::ConstantBuffer::CONSTANT_BUFFER_RENDEROBJ, 1, &mObjectBuffer);

	mMesh->Draw();
}

void RenderObj::Update(float deltaTime)
{
	for (int i = 0; i < mComponents.size(); i++)
	{
		mComponents[i]->Update(deltaTime);
	}
}