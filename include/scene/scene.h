#pragma once
#include"../model/model.h"
#include"../light/light.h"
#include"../shader/shader.h"
#include"../camera/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../include/utils/SH.h"

enum RENDER_MODE {
	test_IBL_PBR,
	test_IBL_PBR_SHADAW,
};


class Scene {
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;
	//shadow map settings
	unsigned int depthMapFBO;
	unsigned int depthMap;
	unsigned int ibl_diffuse_irradiance_map;
	unsigned int ibl_prefilter_map;
	unsigned int ibl_BRDFLUT_map;

	vector<Model*> models;
	vector<Shader*> shaders;
	Camera* camera;
	vector<EmissiveMaterial*> lights;

	//testAreaLight
	AreaLight* arealight;

	Shader* depthShader;
	Shader* debugDepthQuad;
	

public:
	void addModels(Model* model, Shader* shader) {
		models.push_back(model);
		shaders.push_back(shader);

	}
	void setCamera(Camera* camera) {
		this->camera = camera;
	}
	void addLight(EmissiveMaterial* light) {
		lights.push_back(light);
	}
	void renderScene(RENDER_MODE mode = test_IBL_PBR_SHADAW);

	void setDepthMap();
	
	void viewDepthMap();

	void testSH(float time);

	void setTestAreaLight(glm::vec3 light_intensity, glm::vec3 light_color, bool hasShadowMap, glm::vec3 light_pos, float l, float w) {
		arealight = new AreaLight(light_intensity, light_color, hasShadowMap, light_pos, l, w);
	}

	void setDiffuseIrradianceMap(unsigned int ibl_diffuse_irradiance_id) {
		this->ibl_diffuse_irradiance_map = ibl_diffuse_irradiance_id;
	}
	void setPrefilterMap(unsigned int ibl_prefilter_map) {
		this->ibl_prefilter_map = ibl_prefilter_map;
	}
	void setBRDFLUT(unsigned int ibl_BRDFLUT_map) {
		this->ibl_BRDFLUT_map = ibl_BRDFLUT_map;
	}
};
