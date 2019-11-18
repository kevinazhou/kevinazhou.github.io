#include "stdafx.h"
#include "PointLight.h"
#include "RenderObj.h"

PointLight::PointLight(Game* pGame, RenderObj* pObj) : Component(pObj)
{
	mGame = pGame;
	lightData = mGame->AllocateLight();
}

PointLight::~PointLight()
{
	mGame->FreeLight(lightData);
}

void PointLight::LoadProperties(const rapidjson::Value& properties)
{
	std::string lightInfo;
	GetStringFromJSON(properties, "type", lightInfo);
	if (lightInfo == "PointLight")
	{
		Vector3 diffuseColor;
		Vector3 specularColor;

		float specularPower;
		float innerRadius;
		float outerRadius;

		GetVectorFromJSON(properties, "diffuseColor", diffuseColor);
		GetVectorFromJSON(properties, "specularColor", specularColor);
		GetFloatFromJSON(properties, "specularPower", specularPower);
		GetFloatFromJSON(properties, "innerRadius", innerRadius);
		GetFloatFromJSON(properties, "outerRadius", outerRadius);

		lightData->diffuseColor = diffuseColor;
		lightData->specularColor = specularColor;
		lightData->specularPower = specularPower;
		lightData->innerRadius = innerRadius;
		lightData->outerRadius = outerRadius;
	}
}

void PointLight::Update(float deltaTime)
{
	lightData->position = mObj->mObjectData->mat.GetTranslation();
}