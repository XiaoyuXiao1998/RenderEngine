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
	depthShader = new Shader("../../../shader/shadowShader.vert", "../../../shader/shadowShader.frag");
	debugDepthQuad = new Shader("../../../shader/debug_quad.vert", "../../../shader/debug_quad.frag");

	debugDepthQuad->use();
	debugDepthQuad->setInt("depthMap", 0);



}


void Scene::viewDepthMap() {

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

			//set light's perspective

			// lighting info
	// -------------
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);


	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	// render scene from light's point of view
	depthShader->use();
	depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	depthShader->setMat4("model", model);

	//set viewpoint 
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//bind the FrameBuffer map
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);



	for (int i = 0; i < models.size(); i++) {

		models[i]->Draw(*depthShader);
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
	if (lights[0]->hasShadowMap == true) {


		//set light's perspective

			// lighting info
	// -------------
		glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);


		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// render scene from light's point of view
		depthShader->use();
		depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		depthShader->setMat4("model", model);

		//set viewpoint 
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//bind the FrameBuffer map
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);


		
		for (int i = 0; i < models.size(); i++) {
		
			models[i]->Draw(*depthShader);
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
		shaders[i]->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shaders[i]->setVec3("viewPos", camera->Position);

		// light properties
		shaders[i]->setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
		shaders[i]->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
		shaders[i]->setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

	
		
		models[i]->Draw(*shaders[i]);


	}
	
}