#include "Camera.h"
#include <iostream>
#include <random>

Camera::Camera() {}

Camera::~Camera() {}

Camera::Camera(int widthRes, int heightRes) {
	this->width = widthRes;
	this->height = heightRes;
	renderedImage = new float[widthRes * heightRes * 3];
}

Camera::Camera(int width, int height, float focal, float fovY, glm::vec3 eye, glm::vec3 lookat, glm::vec3 up) {
	this->width = width;
	this->height = height;
	this->focal = focal;
	this->fovY = fovY;

	this->eye = eye;
	this->lookat = lookat;
	this->up = up;

	this->w = glm::normalize(eye - lookat);
	this->u = glm::normalize(glm::cross(up, w));
	this->v = glm::normalize(glm::cross(w, u));
	this->aspect = float(width) / float(height);

  this->inverseProjectionMatrix = glm::inverse(glm::perspective(fovY, float(width) / float(height), 0.0001f, 10.0f));
  this->cameraMatrix = glm::inverse(glm::lookAt(eye, lookat, up));

	renderedImage = new float[width * height * 3];
}

void Camera::CastRay(int px, int py, Ray &pray) { // computing the primary viewing ray
  float x = (2.0f * px) / width - 1.0f;
  float y = (2.0f * py) / height - 1.0f;
	glm::vec4 ray_eye = inverseProjectionMatrix * glm::vec4(x, y, -1.0f, 1.0f);
	ray_eye[2] = -1.0f;
	ray_eye[3] = 1.0f;
	glm::vec3 ray_world = glm::vec3(cameraMatrix * ray_eye);
	pray.origin = eye;
	pray.direction = glm::normalize(ray_world - eye);
	pray.inv_direction = glm::vec3(0.0f, 0.0f, 0.0f);
	pray.inv_direction.x = 1.0f / pray.direction.x;
	pray.inv_direction.y = 1.0f / pray.direction.y;
	pray.inv_direction.z = 1.0f / pray.direction.z;
	pray.shadow = false;
}

glm::vec3 Camera::ComputeRayColor(Scene *scene, Ray &ray, float t0, float t1, int level, glm::vec3 shadow_noise) {

	if (level > Camera::MAX_RECURSION_DEPTH) return glm::vec3(0.0f, 0.0f, 0.0f); // recursion base case

	Record hit_record;
	float eps = 0.001f;

	if (scene->Hit(ray, t0, t1, hit_record)) {
		// I = ka*A + C[kd(dot(L, N)) + ks(dot(R, E))^n]
		glm::vec3 color = scene->materials[hit_record.material]->ka * (scene->lights[0]->color + scene->lights[1]->color); // ambient
		for (auto& light : scene->lights) {
			// computing the color for each light
			glm::vec3 E, L, N, R, diffuse, specular;
			float D; // distance for naive attenuation
			Record shadow_record;
			Ray sray = Ray(hit_record.point, (light->position + shadow_noise) - hit_record.point, glm::vec3(0.0f, 0.0f, 0.0f), true); // no need to compute inv_direction for shadow ray
			if (!scene->Hit(sray, eps, 1.0f + eps, shadow_record)) { // shadow's direction vector is non-normalized.. so t1 = 1.0f
				E = glm::normalize(hit_record.point - eye);
				L = glm::normalize((light->position + shadow_noise) - hit_record.point);
				N = hit_record.normal;
				R = glm::reflect(L, N);
				diffuse = scene->materials[hit_record.material]->kd * glm::dot(L, N);
				specular = scene->materials[hit_record.material]->ks * glm::pow(glm::dot(R, E), scene->materials[hit_record.material]->n);
				D = glm::distance((light->position + shadow_noise), hit_record.point);
				color = color + (light->color * (diffuse + specular)) * (1.0f / D*D);
			}
		}
		// computing the reflected ray
		glm::vec3 rdir = glm::normalize(glm::reflect(ray.direction, hit_record.normal));
		glm::vec3 irdir = glm::vec3(0.0f, 0.0f, 0.0f);
		irdir.x = 1.0f / rdir.x;
		irdir.y = 1.0f / irdir.y;
		irdir.z = 1.0f / irdir.z;
		Ray rray = Ray(hit_record.point, rdir, irdir, false);
		return color + scene->materials[hit_record.material]->km * ComputeRayColor(scene, rray, eps, t1, level + 1, shadow_noise); // recursive call
	} else {
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

}

void Camera::TakePicture(Scene *scene) {
	memset(renderedImage, 0, sizeof(float) * width * height * 3);
	Ray primary_ray;
	float tmax = std::numeric_limits<float>::infinity();
	glm::vec3 color;
	const int n = Scene::SAMPLING;
	const int nsq = n * n;

	for (int dx = 0; dx < width; dx++) {
		for (int dy = 0; dy < height; dy++) {

			// random float generator in [0, 1)
			std::random_device rd;
	    std::mt19937 gen(rd());
			std::uniform_real_distribution<float> N1(0.0f, 1.0f);
			std::uniform_real_distribution<float> N2(0.0f, 1.0f);

			glm::vec2 r[nsq]; // jittering pixel samples
			glm::vec3 s[nsq]; // jittering light sources

			// fill r and s with randomly sampled floats in [0,1)
			for (int k = 0; k < nsq; k++) {
				r[k].x = N1(gen);
				r[k].y = N1(gen);

				s[k].x = N2(gen);
				s[k].y = N2(gen);
				s[k].z = 0.0f;
			}

			// shuffle the light source jittering array to remove any spurious correlation between pixel and light jittering - suggested in MS 13.4
			for (int k = nsq - 1; k > 0; k--) {
				int j = std::rand() % k;
				std::swap(s[j], s[k]);
			}

			color = glm::vec3(0.0f, 0.0f, 0.0f);
			for (int p = 0; p < nsq; p++) {
				// adding these random floats to deviate away from 0.5f, it yields less jagged edges
				float rand1 = 0.5f * (r[p].x + static_cast<float> (std::rand()) / static_cast <float> (RAND_MAX));
				float rand2 = 0.5f * (r[p].y + static_cast<float> (std::rand()) / static_cast <float> (RAND_MAX));
				CastRay(dx + rand1 + r[p].x, dy + rand2 + r[p].y, primary_ray);
				color = color + ComputeRayColor(scene, primary_ray, 0.0f, tmax, 0, s[p]);
			}

			color.x /= nsq;
			color.y /= nsq;
			color.z /= nsq;


			renderedImage[dy * width * 3 + dx * 3 + 0] = color[0];
			renderedImage[dy * width * 3 + dx * 3 + 1] = color[1];
			renderedImage[dy * width * 3 + dx * 3 + 2] = color[2];
		}
	}
}



// ---------------------- begin garbage -------------------------------

// Ray Camera::pixel2ray(int dx, int dy) {
//
// 	float x = float(dx) / float(width) - 0.5f;
// 	float y = float(dy) / float(height) - 0.5f;
//
// 	glm::vec3 dir = -w + x * (glm::tan(fovY / 1.0f) * aspect) * u + y * glm::tan(fovY / 1.0f) * up;
//
// 	return Ray(eye, glm::normalize(dir));
// }

// glm::vec3 Camera::ComputeRayColor(Scene *scene, Ray &ray, float t0, float t1) {
// 	Record rec;
// 	glm::vec3 color, L, R, N, E;
// 	if (scene->Hit(ray, t0, t1, rec)) {
// 		color = scene->materials[rec.material]->ka * ((scene->lights[0]->color + scene->lights[1]->color) / 1.0f);
// 		for (auto& light : scene->lights) {
// 			Ray sray = Ray(rec.point, L);
// 			Record srec;
// 			L = glm::normalize(light->position - rec.point);
// 			t1 = std::numeric_limits<float>::infinity();
// 			if (!scene->Hit(sray, 0.001f, t1, srec)) {
// 				N = rec.normal;
// 				R = glm::reflect(L, N);
// 				E = glm::normalize(rec.point - eye);
// 				color = color + (light->color * (scene->materials[rec.material]->kd * glm::max(0.0f, glm::dot(L, N)))) +
// 												(scene->materials[rec.material]->ks * glm::pow(glm::max(0.0f, glm::dot(R, E)), scene->materials[rec.material]->n));
// 			}
// 		}
// 		return color;
// 	}
// 	return glm::vec3(0.0f, 0.0f, 0.0f);
// }


/*
function ComputeRayColor(ray, t0, t1)
	if (scene->Hit(ray, t0, t1, rec)) then
		color c = ambient
		for each light
			if (not scene->Hit(sray, eps, tlight, srec)) then
				c = c + diffuse + specular
		return c;
	else
		return black
*/

// glm::vec3 Camera::ComputeRayColor(Scene *scene, Ray &ray, Record &hit_record, int level) {
//
// 	if (level > 4) return glm::vec3(0.0f, 0.0f, 0.0f);
//
// 	float tmin = 0.0001f;
// 	float tmax = std::numeric_limits<float>::infinity();
// 	bool hit_scene = false;
//
// 	// if scene->hit(ray, t0, t1, rec) then color c = ambient ... else return black
// 	for (auto& surface : scene->shapes) {
// 		if (surface->hit(ray, tmin, tmax, hit_record)) {
// 			hit_scene = true;
// 			tmax = hit_record.t;
// 		}
// 	}
//
// 	if (!hit_scene) return glm::vec3(0.0f, 0.0f, 0.0f);
//
// 	glm::vec3 color, L, R, N, E; // I = ka*A + C[kd(dot(L, N)) + ks(dot(R, E))^n]
// 	if (hit_scene) {
// 		color = scene->materials[hit_record.material]->ka * (0.5f * scene->lights[0]->color + 0.5f * scene->lights[1]->color);
// 		for (auto& light : scene->lights) {
// 			N = glm::normalize(hit_record.normal);
// 			L = glm::normalize(light->position - hit_record.point);
//
// 			bool occluded = false;
// 			Ray sray = Ray(hit_record.point, L);
// 			tmax = hit_record.t;
// 			Record srec;
//
// 			for (auto& surface : scene->shapes) {
// 				if (surface->hit(sray, tmin, tmax, srec)) {
// 					occluded = true;
// 					// tmax = srec.t - 1.0f;
// 				}
// 			}
//
// 			if (!occluded) {
// 				E = glm::normalize(hit_record.point - eye);
// 				R = glm::normalize(glm::reflect(L, hit_record.normal));
// 				glm::vec3 diffuse = scene->materials[hit_record.material]->kd * glm::dot(L, N);
// 				glm::vec3 specular = scene->materials[hit_record.material]->ks * glm::pow(glm::dot(R, E), scene->materials[hit_record.material]->n);
// 				float D = glm::distance(light->position, hit_record.point);
// 				if (glm::dot(L, N) > -0.0001f) color = color + (light->color * (diffuse + specular)) * (1.0f / D*D);
// 				// color = color + (light->color * (scene->materials[hit_record.material]->kd * glm::max(0.0f, glm::dot(L, N)))) +
// 												// (scene->materials[hit_record.material]->ks * glm::pow(glm::max(0.0f, glm::dot(R, E)), scene->materials[hit_record.material]->n));
// 			}
// 		}
// 	}
//
// 	Ray rray = Ray(hit_record.point, glm::normalize(glm::reflect(ray.direction, hit_record.normal)));
// 	return color + scene->materials[hit_record.material]->km * ComputeRayColor(scene, rray, hit_record, level + 1);
//
// 	// return color;
//
// }

// if (dot(ray_direction, outward_normal) > 0.0) {
//     // ray is inside the sphere
//     normal = -outward_normal;
//     front_face = false;
// } else {
//     // ray is outside the sphere
//     normal = outward_normal;
//     front_face = true;
// }
