#pragma once
#include"../model/model.h"
#include"../light/light.h"
#include"../shader/shader.h"
#include"../camera/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Scene {
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	vector<Model*> models;
	vector<Shader*> shaders;
	Camera* camera;
	vector<EmissiveMaterial*> lights;
	

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
	void renderScene();
	
};
