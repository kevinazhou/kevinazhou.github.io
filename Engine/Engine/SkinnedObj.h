#pragma once

#include "engineMath.h"
#include "RenderObj.h"

#define MAX_SKELETON_BONES 80

class SkinnedObj : public RenderObj
{
public:
	struct SkinConstants
	{
		Matrix4 c_skinMatrix[MAX_SKELETON_BONES];
	};

	SkinnedObj(Graphics* graphics, const class Mesh* mesh);
	~SkinnedObj();
	void Draw() override;
	SkinConstants skinMatrices;

protected:
	ID3D11Buffer* mConstantSkinBuffer = nullptr;
};