#pragma once
#include"light.h"
class DirectionalLight:public EmissiveMaterial {
public:
	
	glm::vec3 light_pos;
	
	
	DirectionalLight(glm::vec3 light_color, glm::vec3 light_intensity, glm::vec3 light_pos, glm::vec3 focalPoint, bool hasShadowMap):EmissiveMaterial(light_intensity, light_color,hasShadowMap,
		light_pos){
		
	
	    


	}


};