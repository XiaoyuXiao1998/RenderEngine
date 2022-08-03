#pragma once
#include "../../include/material/textures.h"
#include "../../include/model/model.h"

enum GenMapType {
	DiffuseIrradianceMap,
	preFilteredMap

};


class EnvGenerator {
public:
	unsigned int mapResolution;

	unsigned int EnvTextureID;
	unsigned int DiffuseIrradianceMapId = -1;
	unsigned int preFilteredMapID = -1;
	unsigned int BRDFLUTMapID = -1;
	shared_ptr<Texture> BRDFLUTMap;
public:

	EnvGenerator(unsigned int mapResolution) {
		this->mapResolution = mapResolution;

	}

	void genIrradiance(glm::vec3 ObjectPos, Model* skybox_obj);
	void genPrefiltedMap(glm::vec3 ObjectPos, Model* skybox_obj);
	void loadBRDFLUTMap();
	
	//use to generate textures
	void setupGL(GenMapType type = DiffuseIrradianceMap, bool UseMipMap = false);
	unsigned int  getDiffuseIrradianceMapId() {
		return DiffuseIrradianceMapId;
	}
	unsigned int getPrefilterMapID() {
		return preFilteredMapID;
	}

	unsigned int getBRDFLUTID() {
		return BRDFLUTMap->glResourceID;
	}
};