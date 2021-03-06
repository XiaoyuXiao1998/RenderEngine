#pragma once
#include <glm/glm.hpp>
class EmissiveMaterial
{
public:
	glm::vec3 intensity;
	glm::vec3 color;
	glm::vec3 light_pos;
	bool hasShadowMap;
public:
	EmissiveMaterial(glm::vec3 light_intensity, glm::vec3 light_color, bool hasShadowMap, glm::vec3 light_pos) {
		intensity = light_intensity;
		color = light_color;
		this->hasShadowMap = hasShadowMap;
		this->light_pos = light_pos;
	}
};

class AreaLight :public EmissiveMaterial {
public:
	float L;
	float W;
public:
	AreaLight(glm::vec3 light_intensity, glm::vec3 light_color, bool hasShadowMap, glm::vec3 light_pos, float l, float w) :
		EmissiveMaterial(light_intensity, light_color, hasShadowMap, light_pos),
		L(l),
		W(w) {

	}

	
};