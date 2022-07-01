#pragma once
#include <glm/glm.hpp>
class EmissiveMaterial
{
	glm::vec3 intensity;
	glm::vec3 color;
public:
	EmissiveMaterial(glm::vec3 light_intensity, glm::vec3 light_color) {
		intensity = light_intensity;
		color = light_color;
	}
};