#include "stdafx.h"
#include "Shader.h"
#include "Graphics.h"
#include <d3dcompiler.h>

#pragma comment (lib, "d3dcompiler.lib") 

Shader::Shader(Graphics* graphics)
    : mGraphics(graphics)
{
}

Shader::~Shader()
{
    // TODO Lab 02g
	vertexShader->Release();
	pixelShader->Release();
	inputLayout->Release();
}

static bool LoadShader(const WCHAR* filename, const char* entryPoint, const char* model, ID3DBlob*& pBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, model,
        dwShaderFlags, 0, &pBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            static wchar_t szBuffer[4096];
            _snwprintf_s(szBuffer, 4096, _TRUNCATE,
                L"%hs",
                (char*)pErrorBlob->GetBufferPointer());
            OutputDebugString(szBuffer);
#ifdef _WINDOWS
            MessageBox(nullptr, szBuffer, L"Error", MB_OK);
#endif
            pErrorBlob->Release();
        }
        return false;
    }
    if (pErrorBlob)
    {
        pErrorBlob->Release();
    }

    return true;
}

bool Shader::Load(const WCHAR* fileName, const D3D11_INPUT_ELEMENT_DESC* layoutArray, int numLayoutElements)
{
	// TODO Lab 02g
	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;
	LoadShader(fileName, "VS" ,"vs_4_0", vertexBlob);
	LoadShader(fileName, "PS", "ps_4_0", pixelBlob);

	
	mGraphics->GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &vertexShader);
	
	mGraphics->GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &pixelShader);


	mGraphics->GetDevice()->CreateInputLayout(layoutArray, numLayoutElements, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &inputLayout);

	vertexBlob->Release();
	pixelBlob->Release();

    //DbgAssert(false, "Failed to load shader");
	return true;
}

void Shader::SetActive() const
{
	// TODO Lab 02g 
	mGraphics->GetDeviceContext()->VSSetShader(vertexShader, nullptr, 0);
	mGraphics->GetDeviceContext()->PSSetShader(pixelShader, nullptr, 0);
	mGraphics->GetDeviceContext()->IASetInputLayout(inputLayout);
}