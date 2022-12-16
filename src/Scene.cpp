#include "Scene.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Scene::~Scene() {
	for (auto p : materials) {
		delete p;
	}
	for (auto p : lights) {
		delete p;
	}
}

Scene::Scene() {}

void Scene::LoadScene() {
	InitMaterials();
	InitShapes();
	InitLights();
}

void Scene::InitLights() {
	Light* light1 = new Light(glm::vec3(0.0f, 3.0f, -2.0f), glm::vec3(0.2f, 0.2f, 0.2f));
	Light* light2 = new Light(glm::vec3(-2.0f, 1.0f, 4.0f), glm::vec3(0.5f, 0.5f, 0.5f));

	lights.push_back(light1);
	lights.push_back(light2);
}

void Scene::InitMaterials() {

	Material* sphere1mat = new Material();
	materials.push_back(sphere1mat);
	materials[0]->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materials[0]->kd = glm::vec3(0.2f, 1.0f, 0.2f);
	materials[0]->ks = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[0]->km = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[0]->n = 100.0f;

	Material* sphere2mat = new Material();
	materials.push_back(sphere2mat);
	materials[1]->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materials[1]->kd = glm::vec3(0.0f, 0.0f, 1.0f);
	materials[1]->ks = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[1]->km = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[1]->n = 10.0f;

	Material* sphere3mat = new Material();
	materials.push_back(sphere3mat);
	materials[2]->ka = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[2]->kd = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[2]->ks = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[2]->km = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[2]->n = 0.0f;

	Material* sphere4mat = new Material();
	materials.push_back(sphere4mat);
	materials[3]->ka = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[3]->kd = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[3]->ks = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[3]->km = glm::vec3(0.8f, 0.8f, 0.8f);
	materials[3]->n = 0.0f;

	Material* plane1and2mat = new Material();
	materials.push_back(plane1and2mat);
	materials[4]->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materials[4]->kd = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[4]->ks = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[4]->km = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[4]->n = 0.0f;

	Material* objmaterial = new Material();
	materials.push_back(objmaterial);
	materials[5]->ka = glm::vec3(0.1f, 0.1f, 0.1f);
	materials[5]->kd = glm::vec3(0.0f, 0.0f, 1.0f);
	materials[5]->ks = glm::vec3(1.0f, 1.0f, 0.5f);
	materials[5]->km = glm::vec3(0.0f, 0.0f, 0.0f);
	materials[5]->n = 100.0f;

}

void Scene::InitShapes() {
	if (Scene::FLAG == 0) { // default scene
		Shape* sphere1 = new Sphere(glm::vec3(-1.0f, -0.7f, 3.0f), 0.3f, 0, glm::vec3(0.0f, 0.006f, 0.0f));
		Shape* sphere2 = new Sphere(glm::vec3(1.0f, -0.5f, 3.0f), 0.5f, 1);
		Shape* sphere3 = new Sphere(glm::vec3(-1.0f, 0.0f, -0.0f), 1.0f, 2);
		Shape* sphere4 = new Sphere(glm::vec3(1.0f, 0.0f, -1.0f), 1.0f, 3);
		Shape* plane1 = new Plane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 4);
		Shape* plane2 = new Plane(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 1.0f), 4);

		shapes.push_back(sphere1);
		shapes.push_back(sphere2);
		shapes.push_back(sphere3);
		shapes.push_back(sphere4);
		shapes.push_back(plane1);
		shapes.push_back(plane2);
	} else { // triangle mesh
		Shape* mesh1;
		mesh1 = LoadModel("../obj/bunny.obj");
		shapes.push_back(mesh1);
	}
}

Mesh* Scene::LoadModel(char *name) {
	// Taken from Shinjiro Sueda with slight modification
	std::vector<float> posBuff;
	std::vector<float> norBuff;
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		// std::cerr << errStr << std::endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	return new Mesh(posBuff, norBuff);
}

bool Scene::Hit(Ray &ray, float t0, float t1, Record &rec) {
	bool hit_scene = false;
	for (auto& surface : this->shapes) {

		if (surface->moving && !ray.shadow) { // moving surface - randomly sample the object's position over the aperture

			int time = std::rand() % 50; // we precomputed the sphere's position over the 50 t-unit aperture
			surface->center = surface->xyz[time]; // update the sphere's position
			if (surface->hit(ray, t0, t1, rec)) {
				hit_scene = true;
				t1 = rec.t;
			}

		} else { // stationary object
			if (surface->hit(ray, t0, t1, rec)) {
				hit_scene = true;
				t1 = rec.t;
			}
		}

	}
	// plane normals are precomputed, triangle normals are computed only for the last hit triangle, so only the spheres need normals at this point
	if (hit_scene && Scene::FLAG == 0 && rec.sphere) {
		rec.normal = (rec.point - rec.center) / rec.radius;
	}

	return hit_scene;
}
