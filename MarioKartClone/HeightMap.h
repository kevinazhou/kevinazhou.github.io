#pragma once
#include <vector>
#include "Math.h"

class HeightMap
{
public:
	HeightMap();
	Vector3 CellToWorld(int row, int col);
	Vector2 WorldToCell(float x, float y);
	bool IsOnTrack(float x, float y);
	float GetHeight(float x, float y);

private:
	bool IsCellOnTrack(int row, int col);
	float GetHeightFromCell(int row, int col);
	std::vector<std::vector<int> > vectorVector;
	int numRows = 0;
	int numCols = 0;

	const float CELL_SIZE = 40.55f;
	const float GRID_TOP = 1280.0f;
	const float GRID_LEFT = -1641.0f;
};