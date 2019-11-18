#pragma once
#include <string>
#include <vector>
#include "BoneTransform.h"
#include "stdafx.h"

class AssetManager;


class Skeleton
{
public:
	struct Bone
	{
		BoneTransform localBindPose;
		std::string name;
		int parentIndex;
	};
	Skeleton() {}
	~Skeleton() {}

	size_t GetNumBones() const;
	const Bone& GetBone(size_t idx) const;
	const std::vector<Bone>& GetBones() const;
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const;

	static Skeleton* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);
private:
	std::vector<Bone> bones;
	std::vector<Matrix4> globalInvBindMatrices;
	void ComputeGlobalInvBindPose();
};