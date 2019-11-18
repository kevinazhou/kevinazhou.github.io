#pragma once
#include "Job.h"

class UpdateAnimJob : public Job
{
public:
	UpdateAnimJob(class Character* character, class Skeleton* skeleton, float animTime, class SkinnedObj* skin);
	~UpdateAnimJob();
	void DoIt() override;

private:
	float mAnimTime;
	class SkinnedObj* mSkinnedObj;
	class Skeleton* mSkeleton;
	class Character* mChar;
};