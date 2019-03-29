#include "VehicleMove.h"
#include "Actor.h"
#include "Game.h"
#include "HeightMap.h"
#include "CSVHelper.h"
#include <fstream>
#include <iostream>
#include <string>

VehicleMove::VehicleMove(class Actor* owner)
	:Component(owner, 50)
{
	std::ifstream in("Assets/HeightMap/Checkpoints.csv");
	std::string line;

	size_t currentLine = 0;
	while (std::getline(in, line))
	{
		if (currentLine != 0)
		{
			std::vector<std::string> lineVector = CSVHelper::Split(line);
			std::vector<int> intVector;
			for (size_t i = 1; i < lineVector.size(); i++)
			{
				int num = std::stoi(lineVector[i]);
				intVector.push_back(num);
			}
			checkpoints.push_back(intVector);
		}
		currentLine++;
	}
}

void VehicleMove::Update(float deltaTime)
{
	if (pedalPressed)
	{
		accelTime += deltaTime;
		float accelerationMagnitude;
		if (accelTime > accelRampTime)
		{
			accelerationMagnitude = Math::Lerp(minAccel, maxAccel, 1.0f);
		}
		else
		{
			accelerationMagnitude = Math::Lerp(minAccel, maxAccel, accelTime / accelRampTime);
		}
		
		//euler integrate the velocity
		velocity += mOwner->GetForward() * accelerationMagnitude * deltaTime;
		
	}
	else
	{
		accelTime = 0.0f;
	}

	Vector3 pos = mOwner->GetPosition();
	pos += velocity * deltaTime;
	if (mOwner->GetGame()->heightMap->IsOnTrack(pos.x, pos.y))
	{
		float newZ = mOwner->GetGame()->heightMap->GetHeight(pos.x, pos.y);
		pos.z = Math::Lerp(pos.z, newZ, 0.1f);
	}

	mOwner->SetPosition(pos);

	//approximating friction
	if (pedalPressed)
	{
		velocity *= drag;
	}
	else
	{
		velocity *= dragWhenNotPressed;
	}
	

	if (turningState != TurnState::None)
	{
		if (turningState == TurnState::Left)
		{//if turning
			angularVelocity += angularAccel * -1 * deltaTime;
		}
		else if (turningState == TurnState::Right)
		{
			angularVelocity += angularAccel * 1 * deltaTime;
		}
	}

	float angle = mOwner->GetRotation();
	angle += angularVelocity * deltaTime;
	mOwner->SetRotation(angle);

	angularVelocity *= angularDrag;

	std::vector<int> nextCheck = checkpoints[lastCheckpoint + 1];
	if (nextCheck[0] == nextCheck[1])
	{//x cells
		Vector2 cellPos = mOwner->GetGame()->heightMap->WorldToCell(pos.x, pos.y);
		if (cellPos.x == nextCheck[0] && cellPos.y >= nextCheck[2] && cellPos.y <= nextCheck[3])
		{
			lastCheckpoint++;
			
			if (lastCheckpoint == 0)
			{
				currentLap++;
				OnLapChange(currentLap);
			}
			if (lastCheckpoint == checkpoints.size() - 1)
			{
				lastCheckpoint = -1;
			}
		}
	}
	else if (nextCheck[2] == nextCheck[3])
	{//y cells
		Vector2 cellPos = mOwner->GetGame()->heightMap->WorldToCell(pos.x, pos.y);
		if (cellPos.y == nextCheck[2] && cellPos.x >= nextCheck[0] && cellPos.x <= nextCheck[1])
		{
			lastCheckpoint++;
			
			if (lastCheckpoint == 0)
			{
				currentLap++;
				OnLapChange(currentLap);
			}
			if (lastCheckpoint == checkpoints.size() - 1)
			{
				lastCheckpoint = -1;
			}
		}
	}



}

void VehicleMove::pressPedal()
{
	pedalPressed = true;
}
void VehicleMove::unpressPedal()
{
	pedalPressed = false;
}

void VehicleMove::turnRight()
{
	turningState = TurnState::Right;
}
void VehicleMove::turnLeft()
{
	turningState = TurnState::Left;
}
void VehicleMove::noTurn()
{
	turningState = TurnState::None;
}

float VehicleMove::distToNextCheckpoint()
{
	Vector3 Pos = mOwner->GetPosition();

	int nextCheckpointIndex = lastCheckpoint + 1;
	std::vector<int> nextCheckpoint = checkpoints[nextCheckpointIndex];
	int midX = (nextCheckpoint[0] + nextCheckpoint[1]) / 2;
	int midY = (nextCheckpoint[2] + nextCheckpoint[3]) / 2;

	Vector3 checkPos = mOwner->GetGame()->heightMap->CellToWorld(midX, midY);

	Vector3 vectorTo = checkPos - Pos;
	return vectorTo.LengthSq();
}