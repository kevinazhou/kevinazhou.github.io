#pragma once

class Graphics;

class Shader
{
public:
    Shader(Graphics* graphics);
    ~Shader();

    bool Load(const WCHAR* fileName, const D3D11_INPUT_ELEMENT_DESC* layoutArray, int numLayoutElements);
    void SetActive() const;

private:
    Graphics* mGraphics;
	// TODO Lab 02g
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
};