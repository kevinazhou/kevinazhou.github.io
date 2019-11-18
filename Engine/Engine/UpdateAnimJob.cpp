#include "stdafx.h"
#include "UpdateAnimJob.h"
#include "Character.h"
#include "SkinnedObj.h"
#include "Skeleton.h"
#include "Animation.h"

UpdateAnimJob::UpdateAnimJob(Character* character, Skeleton* skeleton, float animTime, SkinnedObj* skin)
	:mChar(character), mSkeleton(skeleton), mAnimTime(animTime), mSkinnedObj(skin)
{

}

UpdateAnimJob::~UpdateAnimJob()
{
	delete mChar;
	delete mSkinnedObj;
	delete mSkeleton;
}

void UpdateAnimJob::DoIt()
{
	std::vector<Matrix4> outposes;
	mChar->GetCurrentAnim()->GetGlobalPoseAtTime(outposes, mSkeleton, mAnimTime);
	std::vector<Matrix4> invertPoses = mSkeleton->GetGlobalInvBindPoses();

	for (unsigned int i = 0; i < outposes.size(); i++)
	{
		invertPoses[i] = invertPoses[i] * outposes[i];
		mSkinnedObj->skinMatrices.c_skinMatrix[i] = invertPoses[i];
	}
}