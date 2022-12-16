#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Shape.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Light.h"

// here, material is grouped with the scene manager so that the scene can store
// all the materials, and each object needs just an integer to point to it's corresponding material
class Material {
	public:
		Material() {};
		~Material() {};
		Material(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 km, float n) : ka(ka), kd(kd), ks(ks), km(km), n(n) {}

	public:
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;
		glm::vec3 km;
		float n;
};


class Scene {
	public:
		Scene();
		~Scene();

		void InitMaterials();
		void InitShapes();
		void InitLights();
		void LoadScene();
		bool Hit(Ray &ray, float t0, float t1, Record &rec);
		Mesh* LoadModel(char *name);

	public:
		std::vector<Material*> materials;
		std::vector<Shape*> shapes;
		std::vector<Light*> lights;
		static int FLAG;
		static int SAMPLING;

};
