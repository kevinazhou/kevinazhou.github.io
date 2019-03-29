#include "EnemyMove.h"
#include "CSVHelper.h"
#include "HeightMap.h"
#include "Game.h"
#include "Actor.h"
#include <fstream>
#include <iostream>
#include <string>

EnemyMove::EnemyMove(class Actor* owner)
	:VehicleMove(owner)
{
	setEnemyMax(2250.0f);
	setEnemyMin(1250.0f);
	setEnemyRampTime(1.25f);
	std::ifstream in("Assets/HeightMap/Path.csv");
	std::string line;

	size_t currentLine = 0;
	while (std::getline(in, line))
	{
		if (currentLine != 0)
		{
			std::vector<std::string> lineVector = CSVHelper::Split(line);
			routeVector.push_back(mOwner->GetGame()->heightMap->CellToWorld(std::stoi(lineVector[1]), std::stoi(lineVector[2])));
			numTargets++;
		}
		currentLine++;
	}

	mOwner->SetPosition(routeVector[0]);
	nextTarget = 1;

}

void EnemyMove::Update(float deltaTime)
{
	Vector3 currPos = mOwner->GetPosition();
	Vector3 vectorToNext = routeVector[nextTarget] - currPos;

	if (vectorToNext.LengthSq() < 10000)
	{//close enough if within 100 units of target
		if (nextTarget + 1 >= numTargets)
		{
			nextTarget = 0;
		}
		else
		{
			nextTarget++;
		}
	}

	//now figure out if next target is in front to accelerate straight
	vectorToNext = routeVector[nextTarget] - currPos;
	Vector3 Normed = vectorToNext;
	Normed.Normalize();
	float dotProd = Vector3::Dot(mOwner->GetForward(), Normed);
	if (dotProd > 0.992f && dotProd < 1.008f)
	{//dont need to turn
		pressPedal();
		noTurn();
	}
	else
	{
		//unpressPedal();
		Vector3 c = Vector3::Cross(mOwner->GetForward(), vectorToNext);
		if (c.z > 0)
		{
			turnRight();
		}
		else
		{
			turnLeft();
		}
	}


	
	
	VehicleMove::Update(deltaTime);
}