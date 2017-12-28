#include "grid.h"

glm::vec3 VoxelGrid::GetMinCoordinate(){
	int objectsCount = objects.size();
	if (objectsCount == 0){
		exit(1);
	}
	if (objects[0].coordinate.size() == 0){
		exit(1);
	}
	glm::vec3 ret = objects[0].coordinate[0];
	//std::cout << ret.x << " " << ret.y << " " << ret.z << std::endl;
	for (int i = 0; i < objectsCount; ++i){
		//std::cout << objects[i].coordinate.size() << std::endl;
		int coordinateCount = objects[i].coordinate.size();
		for (int j = 0; j < coordinateCount; ++j){
			if (ret.x > objects[i].coordinate[j].x){
				ret.x = objects[i].coordinate[j].x;
			}
			if (ret.y > objects[i].coordinate[j].y){
				ret.y = objects[i].coordinate[j].y;
			}
			if (ret.z > objects[i].coordinate[j].z){
				ret.z = objects[i].coordinate[j].z;
			}
		}
	}
	minVec = ret;
	//std::cout << ret.x << " " << ret.y << " " << ret.z << std::endl;
	return ret;
}

glm::vec3 VoxelGrid::GetMaxCoordinate(){
	int objectsCount = objects.size();
	if (objectsCount == 0){
		exit(1);
	}
	glm::vec3 ret = objects[0].coordinate[0];
	for (int i = 0; i < objectsCount; ++i){
		int coordinateCount = objects[i].coordinate.size();
		for (int j = 0; j < coordinateCount; ++j){
			if (ret.x < objects[i].coordinate[j].x){
				ret.x = objects[i].coordinate[j].x;
			}
			if (ret.y < objects[i].coordinate[j].y){
				ret.y = objects[i].coordinate[j].y;
			}
			if (ret.z < objects[i].coordinate[j].z){
				ret.z = objects[i].coordinate[j].z;
			}
		}
	}
	return ret;
}

void VoxelGrid::SetGridSize(){
	glm::vec3 min = GetMinCoordinate();
	glm::vec3 max = GetMaxCoordinate();
	height = max.x - min.x;
	width = max.y - min.y;
	length = max.z - min.z;
	cubeSize = height;
	if (cubeSize < width){
		cubeSize = width;
	}
	if (cubeSize < length){
		cubeSize = length;
	}
	voxelSize = cubeSize / 100.0 *1.01;
	for (int i = 0; i < 100; ++i){
		for (int j = 0; j < 100; ++j){
			for (int k = 0; k < 100; ++k){
				grid[i][j][k] = 0;
			}
		}
	}
	minVec -= glm::vec3(voxelSize/2.0, voxelSize/2.0, voxelSize/2.0);
	//std::cout << voxelSize << std::endl;
}

glm::vec3 VoxelGrid::GenerateRay(){
	float a = static_cast<float>(rand()) / RAND_MAX;
	float b = static_cast<float>(rand()) / RAND_MAX;
	float phi = 2 * PI * a;
	float theta = acos(2*b - 1);
	glm::vec3 dir = glm::vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
	//std::cout << dir.x << " " << dir.y << " " << dir.z << std::endl;
	return dir;
}

void VoxelGrid::RayTrace(){
	int rayCount = 10000;
	antenna.power = pow(10.0, antenna.power/10.0);
	//std::cout << antenna.power << std::endl;
	float checkConst = 0.0001;
	
	//srand(time(NULL));
	// srand(10);

	// for (int i = 0; i < 90; ++i)
		// GenerateRay();

	for (int q = 0; q < rayCount; ++q){
		//std::cout << q << std::endl;
		glm::vec3 dir = GenerateRay();
		glm::vec3 baryPosition;
		float minDir = 0;
		bool min =false;
		float length = 0;
		bool end = false;
		glm::vec3 origin = antenna.position;
		while (antenna.power / (length * length + 1) > checkConst){
			glm::vec3 intersectNormal;	
			if (end) {
				break;
			}
			uint objectsNum = objects.size();
			for (uint i = 0; i < objectsNum; ++i){
				Model &object = objects[i];
				uint tris = object.triangle.size();
				for (uint j = 0; j < tris; ++j){
					int ret = glm::intersectRayTriangle(origin, dir, 
									object.coordinate[3*j], 
									object.coordinate[3*j+1], 
									object.coordinate[3*j+2], 
									baryPosition);
					if (ret && ((minDir > baryPosition.z) || !min)){
						min = true;
						minDir = baryPosition.z;
						intersectNormal = glm::triangleNormal(object.coordinate[3*j], 
															  object.coordinate[3*j+1], 
															  object.coordinate[3*j+2]);
					}
				}
			}
			//std::cout << min << std::endl;
			if (min){
				//glm::intersect = origin + dir * minDir;
				length += voxelSize / 2; 
				int stepCount = minDir * 2 / voxelSize;
				//float curRayPower = newRayPower;
				//std::cout << dir.x << " " << dir.y << " " << dir.z << std::endl;
				for (int i = 1; i < stepCount; ++i){
					int x = (int)(floor((origin.x - minVec.x + (dir.x * i * voxelSize / 2)) / voxelSize ));
					int y = (int)(floor((origin.y - minVec.y + (dir.y * i * voxelSize / 2)) / voxelSize ));
					int z = (int)(floor((origin.z - minVec.z + (dir.z * i * voxelSize / 2)) / voxelSize ));
					// std::cout << x << " " << y << " " << z << std::endl;
					if ((x < 0) || (x >= 100) || (y < 0) || (y >= 100) || (z < 0) || (z >= 100)){
						// if (i > 1) std::cout << i << std::endl;
						continue;
					}
					float tmpRayPower = antenna.power / (length*length + 1);
					length += voxelSize / 2; 
					//std::cout << tmpRayPower <<std::endl;
					// std::cout << x << ", " << y << ", " << z << std::endl;
					if (grid[x][y][z] < tmpRayPower){
						grid[x][y][z] = tmpRayPower;
						//std::cout << grid[x][y][z] << std::endl;
					}
				}
				origin = origin + dir * (minDir - voxelSize/2);
				dir = glm::reflect(dir, intersectNormal); 
			} else {
				end = true;
				int i = 1;
				int y = (int)floor((origin.y - minVec.y + dir.y * i * voxelSize / 2) / voxelSize );
				int z = (int)floor((origin.z - minVec.z + dir.z * i * voxelSize / 2) / voxelSize );
				int x = (int)floor((origin.x - minVec.x + dir.x * i * voxelSize / 2) / voxelSize );
				
				length += voxelSize / 2; 
				
				while (x > 0 && x < 100 && y > 0 && y < 100 && z > 0 && z < 100){
					float tmpRayPower = antenna.power / (length*length + 1);
					length += voxelSize / 2; 
					//std::cout << tmpRayPower <<std::endl;
					if (grid[x][y][z] < tmpRayPower){
						grid[x][y][z] = tmpRayPower;
						//std::cout << grid[x][y][z] << std::endl;
					}
					i++;
					y = (int)floor((origin.y - minVec.y + dir.y * i * voxelSize / 2) / voxelSize );
					z = (int)floor((origin.z - minVec.z + dir.z * i * voxelSize / 2) / voxelSize );
					x = (int)floor((origin.x - minVec.x + dir.x * i * voxelSize / 2) / voxelSize );
				}
			}
		}
	}
}

void VoxelGrid::BoxFilter(VoxelGrid &output){
	for (int i = 1; i < 99; ++i){
		for (int j = 1; j < 99; ++j){
			for (int k = 1; k < 99; ++k){
				float s = 0;
				for (int k1 = -1; k1 < 2; ++k1){
					for (int k2 = -1; k2 < 2; ++k2){
						for (int k3 = -1; k3 < 2; ++k3){
							s += grid[i + k1][j + k2][k + k3];
						}
					}
				}
				output.grid[i][j][k] = s / 27.0;
			}
		}
	}
}