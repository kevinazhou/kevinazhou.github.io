#pragma once
#include "stdafx.h"
#include <vector>
#include "BoneTransform.h"

class AssetManager;
class Skeleton;

class Animation
{
public:
	Animation() {}
	~Animation() {}

	uint32_t GetNumBones() const { return numBones; }
	uint32_t GetNumFrames() const { return numFrames; }
	float GetLength() const { return length; }

	static Animation* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool Load(const WCHAR* fileName);

	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const;
private:
	uint32_t numBones = 0;
	uint32_t numFrames = 0;
	float length = 0.0f;
	std::vector<std::vector<BoneTransform>> animData;
};