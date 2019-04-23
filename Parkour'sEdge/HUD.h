#pragma once
#include "UIComponent.h"
#include <string>

class HUD : public UIComponent
{
public:
	HUD(class Actor* owner);
	~HUD();

	void Update(float deltaTime);
	void Draw(class Shader* shader);
	void AddCoin();

	void UpdateCheckpointText(std::string text);
private:
	class Font* mFont;

	class Texture* mTimerText;

	class Texture* mCoinText;

	//radar textures
	class Texture* mRadarBackground;
	class Texture* mRadarArrow;
	class Texture* mRadarBlip;

	//if true, draw the checkpoint text in Draw
	bool CheckpointTextShowing;
};