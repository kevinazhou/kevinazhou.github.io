#include "SecurityCamera.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "SecurityCone.h"
#include "Player.h"

SecurityCamera::SecurityCamera(class Game* game, Actor* parent)
	:Actor(game, parent)
{
	mesh = new MeshComponent(this);
	mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Camera.gpmesh"));

	cone = new SecurityCone(mGame, this);
	
	game->AddCamera(this);
}

void SecurityCamera::OnUpdate(float deltaTime)
{
	if (!firstFrameDone)
	{
		rotatingSoundInt = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
		SetSoundLoudness(rotatingSoundInt);
		firstFrameDone = true;
	}


	if (!cone->IsConeNotWhite())
	{//only turn if it is white
		if (isYellow)
		{//turns white again
			Mix_HaltChannel(detectedSoundInt);
		}
		
		

		if (camState == SecurityCameraState::TurningToEnd ||
			camState == SecurityCameraState::TurningToStart)
		{//if turning
			SetSoundLoudness(rotatingSoundInt);
			if (camState == SecurityCameraState::TurningToEnd)
			{
				currentInterpTime += deltaTime;

				if (currentInterpTime > interpTime)
				{
					currentInterpTime = interpTime;
					camState = SecurityCameraState::PausedAtEnd;
					currentPauseTime = 0.0f;

					Mix_HaltChannel(rotatingSoundInt);
					rotatingSoundInt = -1;
					int t = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotorStop.wav"), 0);
					SetSoundLoudness(t);
				}
			}
			else
			{//turning back to start
				currentInterpTime -= deltaTime;

				if (currentInterpTime < 0.0f)
				{
					currentInterpTime = 0.0f;
					camState = SecurityCameraState::PausedAtStart;
					currentPauseTime = 0.0f;

					Mix_HaltChannel(rotatingSoundInt);
					rotatingSoundInt = -1;
					int t = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotorStop.wav"), 0);
					SetSoundLoudness(t);
				}
			}

			if (isYellow)
			{//start to rotate after player leaves cone
				rotatingSoundInt = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
				SetSoundLoudness(rotatingSoundInt);
			}

			SetQuaternion(Quaternion::Slerp(startQ, endQ, currentInterpTime / interpTime));
		}
		else
		{//if waiting
			currentPauseTime += deltaTime;

			if (currentPauseTime > pauseTime)
			{
				if (camState == SecurityCameraState::PausedAtEnd)
				{//start to turn back
					camState = SecurityCameraState::TurningToStart;
				}
				else
				{//start to turn to the end
					camState = SecurityCameraState::TurningToEnd;
				}
				rotatingSoundInt = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotor.wav"), 0);
				SetSoundLoudness(rotatingSoundInt);

			}
			
		}

		isYellow = false;
	}
	else
	{
		if (!isYellow)
		{//when it turns to yellow
			if (camState == SecurityCameraState::TurningToEnd ||
				camState == SecurityCameraState::TurningToStart)
			{
				Mix_HaltChannel(rotatingSoundInt);
				rotatingSoundInt = -1;
			}
			int t = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/CameraMotorStop.wav"), 0);
			
			SetSoundLoudness(t);

			detectedSoundInt = Mix_PlayChannel(Mix_GroupAvailable(1), mGame->GetSound("Assets/Sounds/SecurityDetected.wav"), 0);
		}
		isYellow = true;
	}
	
	
	
}

SecurityCamera::~SecurityCamera()
{
	
	if (rotatingSoundInt != -1)
	{
		Mix_HaltChannel(rotatingSoundInt);
	}

	mGame->RemoveCamera(this);
	
}

void SecurityCamera::SetSoundLoudness(int channel)
{
	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector3 VectorToPlayer = playerPos - GetPosition();

	if (VectorToPlayer.LengthSq() > 1500.0f * 1500.0f)
	{
		Mix_Volume(channel, 0);
	}
	else if (VectorToPlayer.LengthSq() > 500.0f * 500.0f)
	{
		float Vol = Math::Lerp(0, 128, 1.0 - (VectorToPlayer.LengthSq() / (1500.0f * 1500.0f)));
		Mix_Volume(channel, (int)Vol);
	}
	else
	{
		Mix_Volume(channel, 128);
	}
}