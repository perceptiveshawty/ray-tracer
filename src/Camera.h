#pragma once
#include <vector>
#include "Scene.h"
#include "Ray.h"
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
	public:
		Camera();
		~Camera();
		Camera(int widthRes, int heightRes);
		Camera(int width, int height, float focal, float fovY, glm::vec3 eye, glm::vec3 lookat, glm::vec3 up);

		glm::vec3 ComputeRayColor(Scene *scene, Ray &ray, float t0, float t1, int level, glm::vec3 shadow_noise);
		void CastRay(int px, int py, Ray &pray);
		void TakePicture(Scene *scene);
		float* GetRenderedImage() { return renderedImage; };

		static int MAX_RECURSION_DEPTH;

	private:
		int width;
		int height;
		float focal;
		float fovY;
		glm::vec3 eye;
		glm::vec3 lookat;
		glm::vec3 up;

		glm::vec3 w;
		glm::vec3 u;
		glm::vec3 v;

		float aspect;

		glm::mat4 inverseProjectionMatrix;
		glm::mat4 cameraMatrix;

		float *renderedImage;

};
