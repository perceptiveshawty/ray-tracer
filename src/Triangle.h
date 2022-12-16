#pragma once
#include "Shape.h"
#include <algorithm>

class Triangle {
		public:
			Triangle() {}
			~Triangle() {}
			Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 n0, glm::vec3 n1, glm::vec3 n2, int mid) {
				p[0] = v0;
				p[1] = v1;
				p[2] = v2;
				n[0] = n0;
				n[1] = n1;
				n[2] = n2;
				material_id = mid;

				E1 = p[1] - p[0];
				E2 = p[2] - p[0];
			}

			bool intersect(Ray &ray, float t1, float t2, Record &rec) {
				glm::vec3 P, T, Q;
				float det, inv_det, t, u, v;

				P = glm::cross(ray.direction, E2);
				det = glm::dot(E1, P);
				if (det < 0.00001f)
					return false;
				T = ray.origin - p[0];
				u = glm::dot(T, P);
				if (u < 0.0f || u > det)
					return false;
				Q = glm::cross(T, E1);
				v = glm::dot(ray.direction, Q);
				if (v < 0.0f || u + v > det)
					return false;
				t = glm::dot(E2, Q);
				inv_det = 1.0f / det;
				t *= inv_det;
				if (t < t1 || t > t2)
					return false;

				u *= inv_det;
				v *= inv_det;


				rec.t = t;
				rec.point = ray.at(rec.t);
				rec.u = u;
				rec.v = v;
				rec.sphere = false;
				rec.material = material_id;
				return true;
			}

		public:
			glm::vec3 p[3];
			glm::vec3 n[3];
			glm::vec3 E1;
			glm::vec3 E2;

			int material_id;
};

class Mesh :
	public Shape {
		public:
			Mesh() {}
			~Mesh() {}
			Mesh(const std::vector<float> &posBuff, const std::vector<float> &norBuff) {
				moving = false;
				bbox_max = glm::vec3(0.0f, 0.0f, 0.0f);
				bbox_min = glm::vec3(1000.0f, 1000.0f, 1000.0f);

				for (size_t i = 0; i < posBuff.size(); i += 9) {
					glm::vec3 xyz[3];
					glm::vec3 n[3];

					xyz[0].x = posBuff[i + 0];
					xyz[0].y = posBuff[i + 1];
					xyz[0].z = posBuff[i + 2];
					xyz[1].x = posBuff[i + 3];
					xyz[1].y = posBuff[i + 4];
					xyz[1].z = posBuff[i + 5];
					xyz[2].x = posBuff[i + 6];
					xyz[2].y = posBuff[i + 7];
					xyz[2].z = posBuff[i + 8];

					bbox_max = glm::max(bbox_max, xyz[0]);
					bbox_max = glm::max(bbox_max, xyz[1]);
					bbox_max = glm::max(bbox_max, xyz[2]);
					bbox_min = glm::min(bbox_min, xyz[0]);
					bbox_min = glm::min(bbox_min, xyz[1]);
					bbox_min = glm::min(bbox_min, xyz[2]);

					n[0].x = norBuff[i + 0];
					n[0].y = norBuff[i + 1];
					n[0].z = norBuff[i + 2];
					n[1].x = norBuff[i + 3];
					n[1].y = norBuff[i + 4];
					n[1].z = norBuff[i + 5];
					n[2].x = norBuff[i + 6];
					n[2].y = norBuff[i + 7];
					n[2].z = norBuff[i + 8];

					triangles.push_back(Triangle(xyz[0], xyz[1], xyz[2], n[0], n[1], n[2], 5));
				}

			}

			// based on https://tavianator.com/2011/ray_box.html
			bool hit(Ray &ray, float t1, float t2, Record &rec) {
				float tx1 = (bbox_min.x - ray.origin.x) * ray.inv_direction.x;
				float tx2 = (bbox_max.x - ray.origin.x) * ray.inv_direction.x;
				float tmin = glm::min(tx1, tx2);
				float tmax = glm::max(tx1, tx2);

				float ty1 = (bbox_min.y - ray.origin.y) * ray.inv_direction.y;
				float ty2 = (bbox_max.y - ray.origin.y) * ray.inv_direction.y;

				tmin = glm::max(tmin, glm::min(ty1, ty2));
				tmax = glm::min(tmax, glm::max(ty1, ty2));

				float tz1 = (bbox_min.z - ray.origin.z) * ray.inv_direction.z;
				float tz2 = (bbox_max.z - ray.origin.z) * ray.inv_direction.z;

				tmin = glm::max(tmin, glm::min(tz1, tz2));
				tmax = glm::min(tmax, glm::max(tz1, tz2));

				bool intersected = false;
				if (tmax >= glm::max(0.0f, tmin) && tmin < t2)
					intersected = true;

				if (!intersected)
					return false;

				bool hit_mesh = false;
				for (size_t i = 0; i < triangles.size(); i++) {
					if (triangles[i].intersect(ray, t1, t2, rec)) { // triangle intersect will store barycentric coords in rec
						rec.mesh_index = i; // store the position of the hit triangle, after the loop we use this to index the triangle vector to get the vertex normals
						t2 = rec.t;
						hit_mesh = true;
					}
				}

				if (hit_mesh) // only computing the normal for the intersected triangle
					rec.normal = glm::normalize(((1.0f - rec.u - rec.v) * triangles[rec.mesh_index].n[0]) +
																											 (rec.u * triangles[rec.mesh_index].n[1]) +
																											 (rec.v * triangles[rec.mesh_index].n[2]));

				return hit_mesh;
			}



		public:
			std::vector<Triangle> triangles;
			glm::vec3 bbox_min;
			glm::vec3 bbox_max;
	};
