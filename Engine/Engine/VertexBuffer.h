#pragma once
#include <d3d11.h>
#include <cstdint>

class Graphics;

class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(Graphics* graphics, const void* vertData, uint32_t vertCount, uint32_t vertStride, const void* indexData, uint32_t indexCount, uint32_t indexStride);
	~VertexBuffer();

	void SetActive() const;

	void Draw() const;

	ID3D11Buffer* vBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	DXGI_FORMAT indexFormat;

	UINT stride = 0;
	UINT numVertices = 0;
	UINT numIndices = 0;
	Graphics* vGraphics = nullptr;
};