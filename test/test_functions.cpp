#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/shader/shader.h"
#include "../include/scene/scene.h"
#include "../include/camera/camera.h"
#include"../include/light/dir_light.h"
#include <iostream>
#include<fstream>

using namespace std;

int main() {
	vector<glm::vec3> preComputeL;
	vector<glm::mat3> preComputeLT;

	ifstream ifs;
	ifs.open("../../../assets/cubemap/GraceCathedral/light.txt");
	if (!ifs.is_open()) {
		cout << "real light coefficients files fail" << endl;
		return -1;
	}
	else {
		float r, g, b;
		while (ifs >> r) {
			ifs >> g;
			ifs >> b;

			preComputeL.emplace_back(r, g, b);
		}
	}
	ifs.close();

	float temp;
	ifs.open("../../../assets/cubemap/GraceCathedral/transport.txt");
	if (!ifs.is_open()) {
		cout << "real light transport coefficients files fail" << endl;
		return -1;

	}
	else {
		ifs >> temp;
		glm::mat3 LT;
		while (ifs >> LT[0][0]) {
			ifs >> LT[0][1] >> LT[0][2] >> LT[1][0] >> LT[1][1] >> LT[1][2] >> LT[2][0] >> LT[2][1] >> LT[2][2];
			preComputeLT.push_back(std::move(LT));
		}
		preComputeLT.push_back(LT);


	}
	ifs.close();

	
}