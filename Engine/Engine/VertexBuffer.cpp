#include "stdafx.h"
#include "VertexBuffer.h"
#include "Graphics.h"


VertexBuffer::VertexBuffer()
{
}

VertexBuffer::VertexBuffer(Graphics* graphics, const void* vertData, uint32_t vertCount, uint32_t vertStride, const void* indexData, uint32_t indexCount, uint32_t indexStride)
{
	vGraphics = graphics;
	stride = vertStride;
	numVertices = vertCount;
	numIndices = indexCount;
	vBuffer = vGraphics->CreateGraphicsBuffer(vertData, vertCount * vertStride, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	if (indexStride == 2)
	{
		indexFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
	}
	else if (indexStride == 4)
	{
		indexFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	}

	indexBuffer = vGraphics->CreateGraphicsBuffer(indexData, indexCount * indexStride, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

VertexBuffer::~VertexBuffer()
{
	vBuffer->Release();
	indexBuffer->Release();
}

void VertexBuffer::SetActive() const
{
	UINT offsets = (UINT)0;
	vGraphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offsets);
	vGraphics->GetDeviceContext()->IASetIndexBuffer(indexBuffer, indexFormat, offsets);
}

void VertexBuffer::Draw() const
{
	vGraphics->GetDeviceContext()->DrawIndexed(numIndices, 0, 0);
}