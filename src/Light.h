#pragma once
#include <glm/glm.hpp>

class Light {
	public:
		Light() {};
		~Light() {};
		Light(glm::vec3 xyz, glm::vec3 c) : position(xyz), color(c) {};

	public:
		glm::vec3 position;
		glm::vec3 color;

};
