#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"
#include "texture.h"

#pragma comment (lib, "d3d11.lib") 

/*static*/ Graphics* Graphics::s_theGraphics = nullptr;


Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
{
    DbgAssert(nullptr == s_theGraphics, "You can only have 1 Graphics");
    s_theGraphics = this;
}

Graphics::~Graphics()
{
    DbgAssert(this == s_theGraphics, "There can only be 1 Graphics");
    s_theGraphics = nullptr;
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
	mScreenWidth = width;
	mScreenHeight = height;

	// TODO Lab 02c
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = (UINT)width;
	sd.BufferDesc.Height = (UINT)height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;



	const D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1 };

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//ID3D11Device* device = nullptr;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, /*&FeatureLevelsSupported*/nullptr, &pImmediateContext);
	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &lvl[1], _countof(lvl) - 1, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, /*&FeatureLevelsSupported*/nullptr, &pImmediateContext);
	}

	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	pImmediateContext->RSSetViewports(1, &vp);


	// TODO Lab 02d
	ID3D11Texture2D* pBackBuffer;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);

	pBackBuffer->Release();



	// Lab 04
	CreateDepthStencil((int)width, (int)height, nullptr, nullptr);
	mDepthStencilState = CreateDepthStencilState(true, D3D11_COMPARISON_LESS);

	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Bind the depth stencil view

	//Texture stuff		
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr2 = pd3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState);

	SetActiveSampler(0, mSamplerState);
	//

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = true;
	GetDevice()->CreateRasterizerState(&rasterDesc, &mRasterizationState);
	GetDeviceContext()->RSSetState(mRasterizationState);
	mRasterizationState->Release();
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//  Disable GPU access to the vertex buffer data.
	pImmediateContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//  Update the vertex buffer here.
	memcpy(mappedResource.pData, data, dataSize);
	//  Reenable GPU access to the vertex buffer data.
	pImmediateContext->Unmap(buffer, 0);
}

ID3D11Buffer* Graphics::CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
	ID3D11Buffer* returnBuffer = nullptr;

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.ByteWidth = inDataSize;
	vertexBufferDesc.Usage = inUsage;
	vertexBufferDesc.BindFlags = inBindFlags;
	vertexBufferDesc.CPUAccessFlags = inCPUAccessFlags;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;


	pd3dDevice->CreateBuffer(&vertexBufferDesc, nullptr, &returnBuffer);

	if (initialData != nullptr)
	{
		UploadBuffer(returnBuffer, initialData, inDataSize);
	}	

	//FOR DEBUGGING
	//char text[256];
	//sprintf_s(text, "0x%016llx\n", (uintptr_t)returnBuffer);
	//OutputDebugStringA(text);

	return returnBuffer;
}

void Graphics::BeginFrame(const Color4 &clearColor)
{
	pImmediateContext->OMSetDepthStencilState(mDepthStencilState, 0);
	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, mDepthStencilView);

	pImmediateContext->ClearRenderTargetView(pRenderTargetView, (const float*)&clearColor);

	pImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
}

void Graphics::EndFrame()
{
	// TODO Lab 02c
	pSwapChain->Present(1, 0);
}

void Graphics::CleanD3D()
{
//#ifdef _DEBUG
//	ID3D11Debug* pDbg = nullptr;
//	HRESULT hr = pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDbg));
//	DbgAssert(S_OK == hr, "Unable to create debug device");
//#endif
	// TODO Lab 02c
	pSwapChain->Release();
	pd3dDevice->Release();
	pImmediateContext->Release();
    // TODO Lab 02d
	pRenderTargetView->Release();
	mDepthStencilState->Release();
	mDepthStencilView->Release();
	mDepthTexture->Release();

	mSamplerState->Release();

//#ifdef _DEBUG
//	pDbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
//	pDbg->Release();
//#endif
}


bool Graphics::CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D** pDepthTexture, ID3D11DepthStencilView** pDepthView)
{
	if (pDepthTexture != nullptr)
	{
		mDepthTexture = *pDepthTexture;
	}
	else
	{
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = inWidth;
		descDepth.Height = inHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		HRESULT hr = pd3dDevice->CreateTexture2D(&descDepth, NULL, &mDepthTexture);
	}

	if (pDepthView != nullptr)
	{
		mDepthStencilView = *pDepthView;
	}
	else
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;


		// Create the depth stencil view
		HRESULT hr = pd3dDevice->CreateDepthStencilView(mDepthTexture, // Depth stencil texture
			&descDSV, // Depth stencil desc
			&mDepthStencilView);  // [out] Depth stencil view


	}
	
	return true;
}

ID3D11DepthStencilState* Graphics::CreateDepthStencilState(bool inDepthTestEnable, D3D11_COMPARISON_FUNC inDepthComparisonFunction)
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = inDepthTestEnable;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = inDepthComparisonFunction;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	pd3dDevice->CreateDepthStencilState(&dsDesc, &mDepthStencilState);
	return mDepthStencilState;
}

//Texture stuff
void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
	pImmediateContext->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
	pImmediateContext->PSSetSamplers(slot, 1, &pSampler);
}
