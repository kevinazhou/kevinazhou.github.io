#include "HUD.h"
#include "Font.h"
#include "Texture.h"
#include "Game.h"
#include "Actor.h"
#include "Renderer.h"
#include "SecurityCamera.h"


HUD::HUD(class Actor* owner)
	:UIComponent(owner)
{
	mFont = new Font();

	mFont->Load("Assets/Inconsolata-Regular.ttf");

	mTimerText = mFont->RenderText("00:00.00");

	std::string coinString = std::to_string(mOwner->GetGame()->coins);

	coinString.append("/55");

	mCoinText = mFont->RenderText(coinString);

	//for keeping checkpoint text on screen while switching levels
	if (mOwner->GetGame()->GameJustStarted)
	{
		CheckpointTextShowing = false;
		mOwner->GetGame()->GameJustStarted = false;
	}
	else if (mOwner->GetGame()->textTimer >= 5.0f)
	{
		CheckpointTextShowing = false;
	}
	else
	{
		CheckpointTextShowing = true;
	}

	mRadarBackground = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Radar.png");
	mRadarArrow = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/RadarArrow.png");
	mRadarBlip = mOwner->GetGame()->GetRenderer()->GetTexture("Assets/Blip.png");

}

HUD::~HUD()
{
	mFont->Unload();

	delete mFont;
}

void HUD::Draw(class Shader* shader)
{
	DrawTexture(shader, mTimerText, Vector2(-420.0f, -325.0f));
	DrawTexture(shader, mCoinText, Vector2(-435.0f, -290.0f));

	//drawing radar
	DrawTexture(shader, mRadarBackground, Vector2(405.0f, -280.0f));
	DrawTexture(shader, mRadarArrow, Vector2(405.0f, -280.0f));

	if (CheckpointTextShowing)
	{//if still within 5 seconds
		DrawTexture(shader, mOwner->GetGame()->mCheckpointText, Vector2::Zero);
	}

	//radar calculations
	Vector3 playerForward = mOwner->GetForward();
	Vector2 playerForward2D;
	playerForward2D.y = playerForward.y;
	playerForward2D.x = playerForward.x;
	float angle = atan2(playerForward2D.y, playerForward2D.x);
	Matrix3 rotation = Matrix3::CreateRotation(angle);

	Vector2 playerPos2D;
	playerPos2D.y = mOwner->GetPosition().x;
	playerPos2D.x = mOwner->GetPosition().y;

	std::vector<SecurityCamera*> camVec = mOwner->GetGame()->GetCameraVec();
	for (size_t i = 0; i < camVec.size(); i++)
	{
		Vector2 objectPos2D;
		objectPos2D.y = camVec[i]->GetPosition().x;
		objectPos2D.x = camVec[i]->GetPosition().y;

		Vector2 VectorTo = objectPos2D - playerPos2D;
		if (VectorTo.LengthSq() < 1500.0f * 1500.0f)
		{//if in range on the radar
			VectorTo.x /= 1500.0f;
			VectorTo.y /= 1500.0f;//scale it down to radar scale
			
			VectorTo *= 92.0f;//92 is radius of radar

			VectorTo = Vector2::Transform(VectorTo, rotation);

			//rotating blips
			Vector3 camForward = camVec[i]->GetQuatForward();//cameras (and lasers) use quaternions
			Vector2 camForward2D;
			camForward2D.x = camForward.y;
			camForward2D.y = camForward.x;
			camForward2D = Vector2::Transform(camForward2D, rotation);
			float angle2 = atan2(camForward2D.y, camForward2D.x);

			Vector2 screen = Vector2(405.0f, -280.0f);
			screen += VectorTo;
			DrawTexture(shader, mRadarBlip, screen, 1.0f, angle2);
		}
	}
}

void HUD::Update(float deltaTime)
{//mainly dealing with timer logic in here
	mOwner->GetGame()->time += deltaTime;
	mTimerText->Unload();
	delete mTimerText;

	std::string timeString = std::to_string(mOwner->GetGame()->time);

	std::string decimals = timeString.substr(timeString.find('.') + 1, 2);

	int timeInt = (int)mOwner->GetGame()->time;

	int min = timeInt / 60;

	int seconds = timeInt % 60;

	std::string finalString = "";

	if (min < 10)
	{
		finalString.append("0");
	}

	finalString.append(std::to_string(min));
	finalString.append(":");

	if (seconds < 10)
	{
		finalString.append("0");
	}

	finalString.append(std::to_string(seconds));
	finalString.append(".");

	finalString.append(decimals);

	mTimerText = mFont->RenderText(finalString);

	//checkpoint text should stay on screen for 5 seconds
	mOwner->GetGame()->textTimer += deltaTime;
	if (mOwner->GetGame()->textTimer >= 5.0f)
	{
		CheckpointTextShowing = false;
	}

	

}

void HUD::AddCoin()
{//called in coin
	mOwner->GetGame()->coins++;
	std::string coinString = std::to_string(mOwner->GetGame()->coins);

	coinString.append("/55");

	mCoinText = mFont->RenderText(coinString);
}

void HUD::UpdateCheckpointText(std::string text)
{//called in checkpoint
	mOwner->GetGame()->mCheckpointText = mFont->RenderText(text);
	CheckpointTextShowing = true;
	mOwner->GetGame()->textTimer = 0.0f;
}