#pragma once
#include "Component.h"
#include "../Source/game.h"

class PointLight : public Component
{
public:
	PointLight(Game* pGame, class RenderObj* pObj);
	~PointLight();

	void LoadProperties(const rapidjson::Value& properties);
	void Update(float deltaTime);

	Game* mGame;
	PointLightData* lightData;
};