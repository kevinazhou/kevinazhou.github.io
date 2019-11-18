#include "stdafx.h"
#include "SkinnedObj.h"
#include "Mesh.h"

SkinnedObj::SkinnedObj(Graphics* graphics, const Mesh* mesh)
	:RenderObj(graphics, mesh)
{
	for (int i = 0; i < MAX_SKELETON_BONES; i++)
	{
		skinMatrices.c_skinMatrix[i] = Matrix4::Identity;
	}

	mConstantSkinBuffer = rGraphics->CreateGraphicsBuffer(&skinMatrices, sizeof(skinMatrices),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

SkinnedObj::~SkinnedObj()
{
	mConstantSkinBuffer->Release();
}

void SkinnedObj::Draw()
{
	rGraphics->UploadBuffer(mConstantSkinBuffer, &skinMatrices, sizeof(skinMatrices));
	rGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_SKINNING, 1, &mConstantSkinBuffer);
	RenderObj::Draw();
}