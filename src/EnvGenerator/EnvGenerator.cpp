#include "../../include/EnvGenerator/EnvGenerator.h"


void EnvGenerator::genIrradiance(glm::vec3 ObjectPos, Model * skybox_obj ) {
	//use frame buffer to  save generated inrradiance map;
	unsigned int DiffuseIrradianceFBO;
	unsigned int DiffuseIrradianceRBO;

	glGenFramebuffers(1, &DiffuseIrradianceFBO);
	glGenRenderbuffers(1, &DiffuseIrradianceRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mapResolution, mapResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DiffuseIrradianceRBO);

	//set 90 because we want to get the whole hemisphere
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};


	Shader DiffuseIrradianceCalculateShader("../../../shader/IBL/EnvDiffuseIrradiance.vert", "../../../shader/IBL/EnvDiffuseIrradiance.frag");
	DiffuseIrradianceCalculateShader.use();
	DiffuseIrradianceCalculateShader.setInt("environmentMap", 0);
	DiffuseIrradianceCalculateShader.setVec3("InitialPos", ObjectPos);
	DiffuseIrradianceCalculateShader.setMat4("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_obj->mesh_materials[0]->getCubeMapId());
	glViewport(0, 0, mapResolution, mapResolution); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, DiffuseIrradianceFBO);

	for (unsigned int i = 0; i < 6; i++) {

		DiffuseIrradianceCalculateShader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->DiffuseIrradianceMapId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox_obj->meshes[0].Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLuint b[] = { DiffuseIrradianceFBO,DiffuseIrradianceRBO };
	glDeleteBuffers(2, b);
}


void EnvGenerator::genPrefiltedMap(glm::vec3 ObjectPos, Model* skybox_obj) {

	// use frame buffer to  save generated inrradiance map;
	unsigned int prefilterFBO;
	unsigned int prefilterRBO;

	glGenFramebuffers(1, &prefilterFBO);
	glGenRenderbuffers(1, &prefilterRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mapResolution, mapResolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, prefilterRBO);

	//set 90 because we want to get the whole hemisphere
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	Shader PrefilterCalculateShader("../../../shader/IBL/EnvDiffuseIrradiance.vert", "../../../shader/IBL/Prefilter.frag");
	PrefilterCalculateShader.use();
	PrefilterCalculateShader.setInt("environmentMap", 0);
	PrefilterCalculateShader.setVec3("InitialPos", ObjectPos);
	PrefilterCalculateShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_obj->mesh_materials[0]->getCubeMapId());
	glBindFramebuffer(GL_FRAMEBUFFER, prefilterFBO);

	int MAX_MIPMAP_LEVELS = 4;

	for (auto MIPMAP_LEVELS = 0; MIPMAP_LEVELS <= MAX_MIPMAP_LEVELS; MIPMAP_LEVELS ++ ) {
		unsigned int mipWidth = static_cast<unsigned int> (mapResolution * pow(0.5, MIPMAP_LEVELS));
		unsigned int mipHeight = static_cast<unsigned int>(mapResolution * pow(0.5, MIPMAP_LEVELS));
		glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);
		float roughness = (static_cast<float>(MIPMAP_LEVELS)) / (static_cast<float>(MAX_MIPMAP_LEVELS));
		PrefilterCalculateShader.setFloat("roughness", roughness);



		for (int i = 0; i < 6; i++) {
			PrefilterCalculateShader.setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->preFilteredMapID, MIPMAP_LEVELS);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			skybox_obj->meshes[0].Draw();
		}

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLuint b[] = { prefilterFBO,prefilterRBO };
	glDeleteBuffers(2, b);

	

}



void EnvGenerator::setupGL(GenMapType type,bool UseMipMap) {

	if (type == DiffuseIrradianceMap) {
		glGenTextures(1, &DiffuseIrradianceMapId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DiffuseIrradianceMapId);
	}
	else if (type == preFilteredMap) {
		glGenTextures(1, &preFilteredMapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredMapID);
	}



	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
			mapResolution,
			mapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	if (!UseMipMap) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
}

void EnvGenerator::loadBRDFLUTMap() {
	BRDFLUTMap = make_shared<Texture>("../../../assets/ibl_brdf_LUT.png");
	BRDFLUTMap->SetupGL();

}




