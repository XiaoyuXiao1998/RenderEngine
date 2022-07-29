#include"../../include/scene/scene.h"


void Scene::setDepthMap() {
	//create a frame buffer object to store depth map
	
	glGenFramebuffers(1, &depthMapFBO);

	//create depthmap object
	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//With the generated depth texture we can attach it as the framebuffer's depth buffer:
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//set depth shader;
	depthShader = new Shader("../../../shader/depthShader.vert", "../../../shader/depthShader.frag");

	//used for debug depth shader
	debugDepthQuad = new Shader("../../../shader/debug_quad.vert", "../../../shader/debug_quad.frag");

	debugDepthQuad->use();
	debugDepthQuad->setInt("depthMap", 0);

	//set for shadowrender
	if (lights.size() > 0 && lights[0]->hasShadowMap == true) {
		for (int i = 0; i < models.size(); i++) {
			shaders[i]->use();
			shaders[i]->setBool("hasShadowMap", true);
		}
	}
	else {
		for (int i = 0; i < models.size(); i++) {
			shaders[i]->use();
			shaders[i]->setBool("hasShadowMap", false);

		}
	}
}


void Scene::viewDepthMap() {

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

			//set light's perspective

			// lighting info
	// -------------


	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 0.1f, far_plane = 10.5f;
	//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightProjection = glm::perspective(glm::radians(80.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10.0f);
	lightView = glm::lookAt(arealight->light_pos, glm::vec3(0.f,0.f,0.f), glm::vec3(1.0, 0.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	// render scene from light's point of view
	depthShader->use();
	depthShader->setMat4("uLightMVP", lightSpaceMatrix);
	depthShader->setMat4("model", model);

	//set viewpoint 
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//bind the FrameBuffer map
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);



	for (int i = 0; i < models.size(); i++) {

		models[i]->Draw(*depthShader,depthMap);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//test code --------------------------------------------------
		   // render Depth map to quad for visual debugging
		// ---------------------------------------------
	debugDepthQuad->use();
	debugDepthQuad->setFloat("near_plane", near_plane);
	debugDepthQuad->setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

}


void Scene::renderScene() {

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down


	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();


	//shadow pass
	if (true) {


		//set light's perspective

			// lighting info
		


		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.1, far_plane = 10.5f;
		//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightProjection = glm::perspective(glm::radians(80.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10.0f);
		lightView = glm::lookAt(arealight->light_pos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0, 0.0, -1.0));
		lightSpaceMatrix = lightProjection * lightView;

		// render scene from light's point of view
		depthShader->use();
		depthShader->setMat4("uLightMVP", lightSpaceMatrix);
		depthShader->setMat4("model", model);

		//set viewpoint 
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//bind the FrameBuffer map
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);


		
		for (int i = 0; i < models.size(); i++) {
		
			models[i]->Draw(*depthShader,depthMap);
		
		}
		for (int i = 0; i < models.size(); i++) {
			//set for  shadowrender
			shaders[i]->use();
			shaders[i]->setMat4("uLightMVP", lightSpaceMatrix);
			shaders[i]->setVec3("LightPos", arealight->light_pos);
			shaders[i]->setBool("hasShadowMap", true);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	}
	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	



	for (int i = 0; i < models.size(); i++) {
		shaders[i]->use();

		//set MVP
		shaders[i]->setMat4("projection", projection);
		shaders[i]->setMat4("view", view);
		shaders[i]->setMat4("model", model);

		//set blinn-phong
		shaders[i]->setVec3("dirLight.direction", 0.0f, -1.0f, -1.0f);
		shaders[i]->setVec3("viewPos", camera->Position);

		// light properties
		
		shaders[i]->setVec3("LightIntensity", arealight->intensity);
		


		if (i == models.size() - 1) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			models[i]->Draw(*shaders[i], depthMap, ibl_diffuse_irradiance_map);
			glDisable(GL_CULL_FACE);
		}
		else {
			models[i]->Draw(*shaders[i], depthMap, ibl_diffuse_irradiance_map);
		}


	}


	
}


void Scene::testSH(float time) {
	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();

	//set viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// get the rotation matrix
	glm::mat3 rotation_matrix = glm::mat3(glm::rotate(model, time, glm::vec3(0.0f, -1.0f, 0.0f)));

	for (int i = 0; i < models.size() - 1; i++) {
		shaders[i]->use();

		//set MVP
		shaders[i]->setMat4("projection", projection);
		shaders[i]->setMat4("view", view);
		shaders[i]->setMat4("model", model);


		//rotate the first order  SH coefficients
		static glm::mat3 aPrecomputeLR_rotation;
		static glm::mat3 aPrecomputeLG_rotation;
		static glm::mat3 aPrecomputeLB_rotation;
	
	
		 SH::RotateSH_L1(rotation_matrix, aPrecomputeLR_rotation,models[i]->aPrecomputeLR);
		 SH::RotateSH_L1(rotation_matrix, aPrecomputeLG_rotation, models[i]->aPrecomputeLG);
		 SH::RotateSH_L1(rotation_matrix, aPrecomputeLB_rotation,models[i]->aPrecomputeLB);

		//rotate the second oder SH coefficients
		SH::RotateSH_L2(rotation_matrix, aPrecomputeLR_rotation, models[i]->aPrecomputeLR);
		SH::RotateSH_L2(rotation_matrix, aPrecomputeLG_rotation, models[i]->aPrecomputeLG);
		SH::RotateSH_L2(rotation_matrix, aPrecomputeLB_rotation, models[i]->aPrecomputeLB);






		//buffer SH to vertex shader
		shaders[i]->setMat3("aPrecomputeLR", aPrecomputeLR_rotation);
		shaders[i]->setMat3("aPrecomputeLG", aPrecomputeLG_rotation);
		shaders[i]->setMat3("aPrecomputeLB", aPrecomputeLB_rotation);
		//shaders[i]->setMat3("aPrecomputeLR",  models[i]->aPrecomputeLR);
		//shaders[i]->setMat3("aPrecomputeLG", models[i]->aPrecomputeLG);
		//shaders[i]->setMat3("aPrecomputeLB", models[i]->aPrecomputeLB);
		
		

		models[i]->Draw(*shaders[i], depthMap);


	}

	//------------------draw skybox at last-----------------------------------

	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	shaders[models.size() - 1]->use();

	//set MVP
	shaders[models.size() - 1]->setMat4("projection", projection);
	shaders[models.size() - 1]->setMat4("view", view);
	shaders[models.size() - 1]->setMat4("model", glm::rotate(model, time, glm::vec3(0.0f, -1.0f, 0.0f)));
	models[models.size() - 1]->Draw(*shaders[models.size() - 1], depthMap);
	glDepthFunc(GL_LESS); // set depth function back to default
}