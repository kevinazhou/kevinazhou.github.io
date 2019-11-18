#include "stdafx.h"
#include "Animation.h"
#include "assetManager.h"
#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"

Animation* Animation::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Animation* ret = new Animation();
	if (!ret->Load(fileName))
	{
		delete ret;
		return nullptr;
	}
	return ret;
}

bool Animation::Load(const WCHAR* fileName)
{
	//open file
	std::ifstream file(fileName);
	if (!file.is_open()) return false;

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string stuff = fileStream.str();
	rapidjson::StringStream jsonString(stuff.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonString);

	if (!doc.IsObject()) return false;

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	if (!doc["metadata"].IsObject() || str != "itpanim" || ver != 2) return false;

	//loading the animation sequence
	const rapidjson::Value & sequence = doc["sequence"];
	
	unsigned int frameCount = sequence["frames"].GetInt();
	float timeLength = sequence["length"].GetDouble();
	unsigned int bonecount = sequence["bonecount"].GetInt();

	const rapidjson::Value& tracks = sequence["tracks"];

	numBones = bonecount;
	numFrames = frameCount;
	length = timeLength;

	for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
	{
		std::vector<BoneTransform> boneTransf;
		const rapidjson::Value& transforms = tracks[i]["transforms"];
		for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
		{
			const rapidjson::Value& rot = transforms[j]["rot"];
			if (!rot.IsArray() || rot.Size() < 1 || rot.Size() != 4) return false;

			float xRot = rot[0].GetDouble();
			float yRot = rot[1].GetDouble();
			float zRot = rot[2].GetDouble();
			float wRot = rot[3].GetDouble();

			const rapidjson::Value& transl = transforms[j]["trans"];
			if (!transl.IsArray() || transl.Size() < 1 || transl.Size() != 3) return false;

			float xTrans = transl[0].GetDouble();
			float yTrans = transl[1].GetDouble();
			float zTrans = transl[2].GetDouble();
			BoneTransform boneT;
			boneT.mRotation = Quaternion(xRot, yRot, zRot, wRot);
			boneT.mTranslation = Vector3(xTrans, yTrans, zTrans);
			boneTransf.push_back(boneT);
		}
		animData.push_back(boneTransf);
	}
	return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
	outPoses.clear();
	for (unsigned int i = 0; i < animData.size(); i++)
	{
		float timePerFrame = GetLength() / (GetNumFrames() - 1);
		float fInd1 = inTime / timePerFrame;
		int ind1 = static_cast<int>(fInd1);

		float fInd2 = (inTime + timePerFrame) / timePerFrame;
		int ind2 = static_cast<int>(fInd2);

		float lastTime = ind1 * timePerFrame;
		float frac = inTime - lastTime;
		frac = frac / timePerFrame;

		Matrix4 temp = Matrix4::Identity;
		if (!animData[i].empty())
		{
			BoneTransform trans = BoneTransform::Interpolate(animData[i][ind1], animData[i][ind2], frac);
			temp = trans.ToMatrix();
		}
		outPoses.push_back(temp);
		if (inSkeleton->GetBone(i).parentIndex != -1)
		{
			outPoses[i] = outPoses[i] * outPoses[inSkeleton->GetBone(i).parentIndex];
		}
	}
}