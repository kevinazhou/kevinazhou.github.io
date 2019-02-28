#include "AnimatedSprite.h"
#include "Actor.h"
#include "Game.h"

AnimatedSprite::AnimatedSprite(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
{
}

void AnimatedSprite::Update(float deltaTime)
{
	if (!mAnimName.empty())
	{
		std::vector<SDL_Texture*> copy = mAnims[mAnimName];
		if (!mIsPaused)
		{
			mAnimTimer += mAnimFPS * deltaTime;
			while (mAnimTimer >= copy.size())
			{
				mAnimTimer -= copy.size();
			}
		}
		SetTexture(copy[mAnimTimer]);
	}
}

void AnimatedSprite::SetAnimation(const std::string& name, bool resetTimer)
{
	if (mAnimName != name)
	{
		mAnimName = name;
	}
	
	if (resetTimer)
	{
		mAnimTimer = 0.0f;
	}
}

void AnimatedSprite::AddAnimation(const std::string& name,
								  const std::vector<SDL_Texture*>& images)
{
	mAnims.emplace(name, images);
}
