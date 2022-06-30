#pragma once
#include"light.h"
class DirectionalLight {
public:
	bool hasShadowMap;
	glm::vec3 light_pos;
	EmissiveMaterial mat;
	DirectionalLight(glm::vec3 light_color, glm::vec3 light_intensity, glm::vec3 light_pos, glm::vec3 focalPoint, bool hasShadowMap) :mat(light_intensity,light_color){
		this->light_pos = light_pos;
		this->hasShadowMap = hasShadowMap;
		

	}


};