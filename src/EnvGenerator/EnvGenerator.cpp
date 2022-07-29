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


void EnvGenerator::setupGL(bool UseMipMap) {
	glGenTextures(1, &DiffuseIrradianceMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, DiffuseIrradianceMapId);



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






