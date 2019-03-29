#include "HeightMap.h"
#include "CSVHelper.h"
#include <fstream>
#include <iostream>

HeightMap::HeightMap()
{
	std::ifstream in("Assets/HeightMap/HeightMap.csv");
	std::string line;

	size_t currentLine = 0;
	while (std::getline(in, line))
	{
		if (line != "")
		{
			std::vector<std::string> lineVector = CSVHelper::Split(line);
			std::vector<int> intVector;
			for (int i = 0; i < lineVector.size(); i++)
			{
				int num = std::stoi(lineVector[i]);
				intVector.push_back(num);	
			}
			vectorVector.push_back(intVector);
			numCols = lineVector.size();
		}
		currentLine++;
	}

	numRows = currentLine;
}

bool HeightMap::IsCellOnTrack(int row, int col)
{
	if (row < 0 || col < 0 || row >= numRows || col >= numCols)
	{
		return false;
	}
	else
	{
		if (vectorVector[row][col] > -1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

float HeightMap::GetHeightFromCell(int row, int col)
{
	return -40.0f + (vectorVector[row][col] * 5.0f);
}

Vector3 HeightMap::CellToWorld(int row, int col)
{
	if (!IsCellOnTrack(row, col))
	{//if not on track
		return Vector3::Zero;
	}
	Vector3 ret = Vector3(GRID_TOP - (CELL_SIZE * row), 
		GRID_LEFT + (CELL_SIZE * col), GetHeightFromCell(row, col));

	return ret;
}

Vector2 HeightMap::WorldToCell(float x, float y)
{
	int row = (abs(x - GRID_TOP) + CELL_SIZE / 2.0f) / CELL_SIZE;
	int col = (abs(y - GRID_LEFT) + CELL_SIZE / 2.0f) / CELL_SIZE;

	return Vector2(row, col);
}

bool HeightMap::IsOnTrack(float x, float y)
{
	Vector2 yea = WorldToCell(x, y);
	int r = yea.x;
	int c = yea.y;
	
	return IsCellOnTrack(r, c);
}

float HeightMap::GetHeight(float x, float y)
{
	if (IsOnTrack(x, y))
	{
		Vector2 yea = WorldToCell(x, y);
		int r = yea.x;
		int c = yea.y;

		return GetHeightFromCell(r, c);
	}
	else
	{
		return -1000.0f;
	}
}