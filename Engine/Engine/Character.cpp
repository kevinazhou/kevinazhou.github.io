#include "stdafx.h"
#include "Character.h"
#include "Skeleton.h"
#include "../Source/game.h"
#include "assetManager.h"
#include "RenderObj.h"
#include "Job.h"
#include "UpdateAnimJob.h"
#include "SkinnedObj.h"
#include "stringUtil.h"
#include "JobManager.h"
#include <utility>
#include <fstream>
#include <sstream>
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"

Character::Character(class SkinnedObj* rendObj, class Game* game) : Component(rendObj)
{
	mSkinnedObj = rendObj;
	mGame = game;
}

Character::~Character()
{

}

void Character::LoadProperties(const rapidjson::Value& properties)
{
	std::wstring skeletonFileName;
	StringUtil::String2WString(skeletonFileName, properties["skeleton"].GetString());
	mSkeleton = mGame->GetAssetManager()->LoadSkeleton(skeletonFileName);

	const rapidjson::Value& animations = properties["animations"];
	for (rapidjson::SizeType i = 0; i < animations.Size(); i++)
	{
		std::string animationName = animations[i][0].GetString();
		std::wstring animationFilename;
		StringUtil::String2WString(animationFilename, animations[i][1].GetString());
		Animation* newAnim = mGame->GetAssetManager()->LoadAnimation(animationFilename);
		mAnimations.insert(std::make_pair(animationName, newAnim));
	}
}

bool Character::SetAnim(const std::string& animName)
{
	if (mAnimations.find(animName) != mAnimations.end())
	{
		mCurrentAnimation = mAnimations[animName];
		mAnimTime = 0.0f;
		return true;
	}
	return false;
}

void Character::UpdateAnim(float deltaTime)
{
	mAnimTime += deltaTime;
	if (mCurrentAnimation)
	{
		float length = mCurrentAnimation->GetLength();
		if (mAnimTime >= length) mAnimTime -= length;
	}
	
	Job* newJ = new UpdateAnimJob(this, mSkeleton, mAnimTime, mSkinnedObj);
	mGame->GetJobManager()->AddJob(newJ);
}

void Character::Update(float deltaTime)
{
	if (!mCurrentAnimation) SetAnim("idle");
	UpdateAnim(deltaTime);
}