#pragma once

#include "../externals/glm/glm/glm.hpp"
#include "vector"
#include "math.h"
#include "time.h"
#include "../externals/glm/glm/gtx/euler_angles.hpp"
#include <../externals/glm/glm/gtx/intersect.hpp>
#include <../externals/glm/glm/gtx/normal.hpp>
#include "iostream"

#define PI 3.14159265

struct Camera {
	glm::vec3 position;       
  	glm::vec3 viewDir;      
  	glm::vec3 right;
  	glm::vec3 up;

  	glm::vec2 viewAngle; 
  	glm::vec2 rotateAngle;   
  	glm::uvec2 resolution;  

  	std::vector<glm::vec3> pixels;
};

struct Ray {
	glm::vec3 position;
	glm::vec3 direction;
};

struct Model {
	std::vector<glm::vec3> coordinate;
	std::vector<glm::uvec3> triangle;
	std::vector<glm::vec3> normals;
	Model(){};
	Model(std::vector<glm::vec3> c, std::vector<glm::uvec3> t){
		coordinate = c;
		triangle = t;
	};
	Model operator=(const Model &tmp){
		for (uint i = 0; i < coordinate.size(); ++i){
			coordinate[i] = tmp.coordinate[i];
		}
		for (uint i = 0; i < triangle.size(); ++i){
			triangle[i] = tmp.triangle[i];
		}
		return *this;
	}
};

struct Antenna {
	glm::vec3 position;
	float power;
};