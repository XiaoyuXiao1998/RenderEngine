#include"../../include/scene/scene.h"


void Scene::renderScene() {

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();

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

		//set light properties
		
		models[i]->Draw(*shaders[i]);


	}

}