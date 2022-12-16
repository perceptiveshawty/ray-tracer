#pragma once
#include "Shape.h"

class Sphere :
	public Shape {
		public:
			Sphere() {}
			~Sphere() {}
			// stationary sphere constructor
			Sphere(glm::vec3 o, float r, int mid) : radius(r) {
				center = o;
				material_id = mid;
				moving = false;
			}
			// moving sphere constructor - assumes an aperture of 50 t-units and precomputes the position along the interval
			Sphere(glm::vec3 o, float r, int mid, glm::vec3 v) : radius(r), velocity(v) {
				center = o;
				material_id = mid;
				for (int t = 0; t < 50; t++) {
					xyz[t] = o + (float(t) * velocity);
				}
				moving = true;
			}

			bool hit(Ray &ray, float t1, float t2, Record &rec) { // unchanged because the sphere's center is updated by the scene manager
			  glm::vec3 L = ray.origin - center;
			  float a = glm::dot(ray.direction, ray.direction);
			  float b = glm::dot(2.0f * L, ray.direction);
			  float c = glm::dot(L, L) - (radius * radius);

			  float discriminant = b*b - (4.0f * a * c);
			  if (discriminant <= 0.001f) return false;

			  float sqrt_discriminant = glm::sqrt(discriminant);
			  float root = (-b - sqrt_discriminant) / (2.0f * a);
			  if (root < t1 || root > t2) {
			    root = (-b + sqrt_discriminant) / (2.0f * a);
			    if (root < t1 || root > t2) {
			      return false;
			    }
			  }

			  rec.t = root;
			  rec.point = ray.at(rec.t);
			  rec.center = center;
			  rec.radius = radius;
			  rec.sphere = true;
			  rec.material = material_id;

			  return true;
			}

		private:
			float radius;
			glm::vec3 velocity;

};
