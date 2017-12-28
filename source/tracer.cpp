#include "tracer.h"


void Tracer::SetModel(std::vector<glm::vec3> c, std::vector<glm::uvec3> t, int i){
	objects[i].coordinate = c;
	objects[i].triangle = t;
}

std::vector<Model> Tracer::GetModel(){
	return objects;
}

void Tracer::LoadModel(const std::string filename){
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
	if (!ret) {
  		exit(1);
	}

	size_t shapesSize = shapes.size();
	std::vector<Model> retModel;
	// Loop over shapes
	for (size_t s = 0; s < shapesSize; s++) {
	  	// Loop over faces(polygon)
	  	size_t index_offset = 0;
	  	size_t meshSize = shapes[s].mesh.num_face_vertices.size();
	  	std::vector<glm::vec3> coordinate;
	  	std::vector<glm::uvec3> triangle;
	  	std::vector<glm::vec3> normals;
	  	//std::cout << "meshsize * 3: " << meshSize*3 << "\n\n"; 
	   	for (size_t f = 0; f < meshSize; f++) {
	   		//polygon: fv = 3
	     	//uint fv = shapes[s].mesh.num_face_vertices[f];
	     	// Loop over vertices in the face.
	     	int tmpTriangle[] = {0, 0, 0};
	     	for (size_t v = 0; v < 3; v++) {
	       		// access to vertex
		       	tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
		       	tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
		       	tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
		       	tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
		       	tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
		       	tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
		       	tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
		 //      	//tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
		 //      	//tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];

		       	tmpTriangle[v] = idx.vertex_index;
		       	//std::cout << idx.vertex_index << std::endl;
		       	coordinate.push_back(glm::vec3(vx, vy*(-1), vz*(-1)));
		       	normals.push_back(glm::vec3(nx, ny*(-1), nz*(-1)));
			 }
			triangle.push_back(glm::uvec3(tmpTriangle[0], tmpTriangle[1], tmpTriangle[2]));
		    index_offset += 3;

		 //    // per-face material
		     //shapes[s].mesh.material_ids[f];
  	 	}
  	 	Model tmpModel;
   	//tmpModel(coordinate, triangle);
  	 	tmpModel.coordinate = coordinate;
  	 	tmpModel.normals = normals;
  	 	tmpModel.triangle = triangle;
  	 	retModel.push_back(tmpModel);
	}
	objects = retModel;
}

inline glm::vec3 Tracer::GetDirection(float x, float y){
	glm::vec3 ret = camera.viewDir;
	ret += (((x + 0.5f)/w) - 0.5f)*camera.right + (((y + 0.5f)/h) - 0.5f)*camera.up;
	ret = glm::normalize(ret);
	return ret;
}

Image Tracer::GetImage(VoxelGrid& grid){
	Image resImage(w, h);
	glm::vec3 origin = camera.position;
	for (uint k = 0; k < w; ++k){
		for (uint l = 0; l < h; ++l){
			// std::cout << k << ", " << l << std::endl;
			glm::vec3 dir = GetDirection(k, l);
			glm::vec3 baryPosition;
			float minDir = 0;
			bool min = false;
			uint objectsNum = objects.size();
			uint f, o;
			for (uint i = 0; i < objectsNum; ++i){
				Model &object = objects[i];
				uint tris = object.triangle.size();
				for (uint j = 0; j < tris; ++j){
					//glm::uvec3 tmpTriangle = objects[i].triangle[j];
					// glm::vec3 vertex1 = object.coordinate[3*j];
					// glm::vec3 vertex2 = object.coordinate[3*j+1];
					// glm::vec3 vertex3 = object.coordinate[3*j+2];

					bool ret = glm::intersectRayTriangle(origin, dir, 
									object.coordinate[3*j],
									object.coordinate[3*j+1], 
									object.coordinate[3*j+2], 
									baryPosition);
					if (ret && ((minDir > baryPosition.z) || !min)) {
						min = true;
						minDir = baryPosition.z;
						f = j;
						o = i;
						// glm::vec3 normal1 = object.normals[3*j];
						// glm::vec3 normal2 = object.normals[3*j+1];
						// glm::vec3 normal3 = object.normals[3*j+2];
					}
				}
			}
			if (min)
			{
				glm::vec3 tmpVec =    objects[o].normals[3*f] * (1 - baryPosition.x - baryPosition.y) 
									+ objects[o].normals[3*f+1] * baryPosition.x 
									+ objects[o].normals[3*f+2] * baryPosition.y;
				tmpVec = glm::normalize(tmpVec);
				float res = constFong * glm::dot(tmpVec, dir) / (minDir * minDir + 1);
				if (res < 0.f) res = -res;
				resImage(k, l) = std::make_tuple(res, res, res);
			}

			glm::vec3 intersect = origin + minDir * dir;
			//std::cout << intersect.x << " " << intersect.y << " " << intersect.z << std::endl;
			glm::vec3 newDir = glm::vec3(dir.x*(-1), dir.y*(-1), dir.z*(-1));
			int stepCount = minDir * 2 / grid.voxelSize;
			for (int i = 0; i < stepCount; ++i){
				int x = (int)floor((intersect.x - grid.minVec.x + newDir.x * i * grid.voxelSize / 2) / grid.voxelSize);
				int y = (int)floor((intersect.y - grid.minVec.y + newDir.y * i * grid.voxelSize / 2) / grid.voxelSize);
				int z = (int)floor((intersect.z - grid.minVec.z + newDir.z * i * grid.voxelSize / 2) / grid.voxelSize);
				if ((x < 0) || (x >= 100) || (y < 0) || (y >= 100) || (z < 0) || (z >= 100)){
					//std::cout << i << std::endl;
					continue;
				}
				//std::cout << x << " " << y << " " << z << std::endl;
				float alpha = grid.grid[x][y][z] > 1.f ? 1.f : grid.grid[x][y][z];
				//if (alpha > 0.f) std::cout << alpha << std::endl;
				float red, green, blue;
				std::tie(red, green, blue) = resImage(k, l);
				resImage(k, l) = std::make_tuple(alpha*255 + (1 - alpha)* red, alpha*255 + (1 - alpha)*green, (1 - alpha)*blue);
			} 

		}
	}
	return resImage;
}

void Tracer::GetViewAnglex(){
	float ret;
	ret = w * tan((camera.viewAngle.y / 2.0) * PI / 180.0) / h;
	ret = atan(ret) * 180.0 / PI;
	camera.viewAngle.x = ret * 2;
	//std::cout << camera.viewAngle.x << std::endl;
}

void Tracer::RotateCamera(float angley, float anglez){
	angley = glm::radians(angley);
	anglez = glm::radians(anglez);
	auto rotateMatrix = glm::eulerAngleYZ(angley, anglez);
	glm::vec4 newViewDir = glm::vec4(camera.viewDir.x, camera.viewDir.y, camera.viewDir.z, 1.0f);
	glm::vec4 newRight = glm::vec4(camera.right.x, camera.right.y, camera.right.z, 1.0f);
	glm::vec4 newUp = glm::vec4(camera.up.x, camera.up.y, camera.up.z, 1.0f);
	newViewDir = rotateMatrix*newViewDir;
	newRight = rotateMatrix*newRight;
	newUp = rotateMatrix*newUp;
	camera.viewDir = newViewDir;
	camera.up = newUp;
	camera.right = newRight;
	//std::cout << newViewDir.x << " " << newViewDir.y << " " << newViewDir.z << std::endl;
}
