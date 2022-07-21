#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<iostream>
#define M_PI 3.141592653589793f



namespace SH {

	// dir must be a norm unit

	double EvalSH(int l, int m, const glm::vec3& dir);

	
	void RotateSH_L1(const glm::mat3& R, glm::mat3& SHcoefficients, const glm::mat3& ref);

	void RotateSH_L2(const glm::mat3& R, glm::mat3& SHcoefficients, const glm::mat3& ref);




};