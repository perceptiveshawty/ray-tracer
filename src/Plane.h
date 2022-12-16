#pragma once
#include "Shape.h"

class Plane :
	public Shape {
		public:
			Plane() {}
			~Plane() {}
			Plane(glm::vec3 o, glm::vec3 n, int mid) : normal(n) {
				center = o;
				material_id = mid;
				moving = false;
			}

			bool hit(Ray &ray, float t1, float t2, Record &rec) {
			  float denominator = glm::dot(ray.direction, normal);
			  if (glm::abs(denominator) < 0.0001f) return false;
			  float t = glm::dot(center - ray.origin, normal) / denominator;

			  if (t < t1 || t > t2) return false;

			  rec.t = t;
			  rec.point = ray.at(t);
			  rec.sphere = false;
			  rec.normal = normal;
			  rec.material = material_id;

			  return true;

			}

		private:
			glm::vec3 normal;

};
