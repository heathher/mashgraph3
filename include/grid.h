#pragma once

#include "types.h"
#include "ctime"


class VoxelGrid {
public:
	float width, height, length;
	float cubeSize;
	float voxelSize;
	glm::vec3 minVec;
	float grid[100][100][100];
	std::vector<Model> objects;
	Antenna antenna;
	//float checkConst = 0.01;

	glm::vec3 GetMinCoordinate();
	glm::vec3 GetMaxCoordinate();
	void SetGridSize();
	glm::vec3 GenerateRay();
	void RayTrace();
	void BoxFilter(VoxelGrid &output);
};