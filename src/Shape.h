#pragma once
#include <glm/glm.hpp>
#include "Ray.h"

struct Record {
	glm::vec3 point; // intersection point of ray and shape
	glm::vec3 normal; // normal vector at intersection point, not initialized for occluded/non-intersected shapes
	glm::vec3 center; // center point of each shape, not intialized for triangle mesh
	float radius; // sphere radius, not intialized for etc.
	bool sphere; // if shape is a sphere we need to compute the normal with center and radius
	float t; // barycentric coords
	float u; // ..
	float v; // ..
	int mesh_index; // if triangle was hit, record the index of triangle in storage vector
	int material; // index of material object in scene manager's mateerial vector,  storing ks, kd, etc. 
};

class Shape {
	public:
		Shape(void) {};
		~Shape(void) {};

		virtual bool hit(Ray &ray, float t1, float t2, Record &rec) = 0;

	public:
		glm::vec3 center;
		glm::vec3 xyz[50]; // if moving, this stores the center of the shape over a predefined 50 t-unit interval
		int material_id;
		bool moving;

};
