#pragma once
#include "../../include/material/textures.h"
#include "../../include/model/model.h"


class EnvGenerator {
public:
	unsigned int mapResolution;

	unsigned int EnvTextureID;
	unsigned int DiffuseIrradianceMapId = -1;

public:

	EnvGenerator(unsigned int mapResolution) {
		this->mapResolution = mapResolution;

	}

	void genIrradiance(glm::vec3 ObjectPos, Model* skybox_obj);
	
	//use to generate textures
	void setupGL( bool UseMipMap = false);
	unsigned int  getDiffuseIrradianceMapId() {
		return DiffuseIrradianceMapId;
	}
};