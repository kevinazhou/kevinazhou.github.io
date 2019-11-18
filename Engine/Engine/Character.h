#pragma once

#include "Component.h"
#include <unordered_map>

class Character : public Component
{
public:
	Character(class SkinnedObj* rendObj, class Game* game);
	~Character();
	void LoadProperties(const rapidjson::Value& properties) override;
	bool SetAnim(const std::string& animName);
	void UpdateAnim(float deltaTime);
	void Update(float deltaTime) override;
	const class Animation* GetCurrentAnim() { return mCurrentAnimation; }
private:
	class SkinnedObj* mSkinnedObj;
	class Skeleton* mSkeleton;
	std::unordered_map<std::string, const class Animation*> mAnimations;
	const class Animation* mCurrentAnimation;
	float mAnimTime = 0.0f;

	class Game* mGame;
};