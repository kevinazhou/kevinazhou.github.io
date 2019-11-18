#include "stdafx.h"
#include "Skeleton.h"
#include "assetManager.h"
#include <fstream>
#include <sstream>
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"


void Skeleton::ComputeGlobalInvBindPose()
{
	for (int i = 0; i < bones.size(); i++)
	{
		Matrix4 local = bones[i].localBindPose.ToMatrix();
		if (bones[i].parentIndex != -1)
		{
			Matrix4 parentInvBind = globalInvBindMatrices[bones[i].parentIndex];
			local = local * parentInvBind;
		}
		globalInvBindMatrices.push_back(local);
	}

	for (int k = 0; k < globalInvBindMatrices.size(); k++)
	{
		globalInvBindMatrices[k].Invert();
	}
}

size_t Skeleton::GetNumBones() const
{
	return bones.size();
}

const Skeleton::Bone& Skeleton::GetBone(size_t idx) const
{
	return bones[idx];
}

const std::vector<Skeleton::Bone>& Skeleton::GetBones() const
{
	return bones;
}

const std::vector<Matrix4>& Skeleton::GetGlobalInvBindPoses() const
{
	return globalInvBindMatrices;
}

Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Skeleton* ret = new Skeleton();
	if (!ret->Load(fileName))
	{
		delete ret;
		return nullptr;
	}
	return ret;
}

bool Skeleton::Load(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) return false;

	std::stringstream fStream;
	fStream << file.rdbuf();
	std::string stuff = fStream.str();
	rapidjson::StringStream jsonStr(stuff.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject()) return false;

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	if (!doc["metadata"].IsObject() || str != "itpskel" || ver != 1) return false;

	unsigned int bonecount = doc["bonecount"].GetInt();
	
	//Loading bones
	const rapidjson::Value& boneArr = doc["bones"];
	if (!boneArr.IsArray() || boneArr.Size() < 1) return false;

	for (rapidjson::SizeType i = 0; i < bonecount; i++)
	{
		if (!boneArr[i].IsObject()) return false;

		std::string boneName;
		boneName += boneArr[i]["name"].GetString();
		int parentInd = boneArr[i]["parent"].GetInt();
		const rapidjson::Value& bindPose = boneArr[i]["bindpose"];

		const rapidjson::Value& rot = bindPose["rot"];
		if (!rot.IsArray() || rot.Size() < 1 || rot.Size() != 4) return false;
		float xRot = rot[0].GetDouble();
		float yRot = rot[1].GetDouble();
		float zRot = rot[2].GetDouble();
		float wRot = rot[3].GetDouble();

		const rapidjson::Value& trans = bindPose["trans"];
		if (!trans.IsArray() || trans.Size() < 1 || trans.Size() != 3) return false;

		float xTrans = trans[0].GetDouble();
		float yTrans = trans[1].GetDouble();
		float zTrans = trans[2].GetDouble();

		Bone toAdd;
		toAdd.name = boneName;
		toAdd.parentIndex = parentInd;
		toAdd.localBindPose.mRotation = Quaternion(xRot, yRot, zRot, wRot);
		toAdd.localBindPose.mTranslation = Vector3(xTrans, yTrans, zTrans);
		bones.push_back(toAdd);
	}
	ComputeGlobalInvBindPose();
	return true;
}