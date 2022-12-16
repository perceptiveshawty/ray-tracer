#pragma once
#include <glm/glm.hpp>

class Ray {
	public:
		Ray() {}
		~Ray() {}
		Ray(glm::vec3 o, glm::vec3 d, glm::vec3 id, bool s) : origin(o), direction(d), inv_direction(id), shadow(s) {}
		glm::vec3 at(float t) {return origin + t*direction;}

	public:
		glm::vec3 origin;
		glm::vec3 direction;
		glm::vec3 inv_direction;
		bool shadow;
};
