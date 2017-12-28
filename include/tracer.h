#pragma once

#include "types.h"
#include "../externals/tinyobjloader/tiny_obj_loader.h"
#include "../externals/inih/cpp/INIReader.h"
#include "EasyBMP.h"
#include "matrix.h"
#include <cmath>
#include "io.h"

#include "grid.h"

class VoxelGrid;

class Tracer {
public:
	std::vector<Model> objects;
	Camera camera;
	float h, w;
	int constFong;
	//VoxelGrid grid;
	std::vector<Model> GetModel();
	void SetModel(std::vector<glm::vec3>, std::vector<glm::uvec3>, int);
	void LoadModel(std::string);
	Image GetImage(VoxelGrid&);
	glm::vec3 GetDirection(float , float);
	void GetViewAnglex();
	void RotateCamera(float, float);
	//float GetFong();
};