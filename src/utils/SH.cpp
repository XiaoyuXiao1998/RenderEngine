#include"../../include/utils/SH.h"


static double HardcodedSH00(const glm::vec3& d) {
	// 0.5 * sqrt(1/pi)
	return 0.282095;
}

double HardcodedSH1n1(const glm::vec3& d) {
	// -sqrt(3/(4pi)) * y
	return 0.488603 * d.y;
}

double HardcodedSH10(const glm::vec3& d) {
	// sqrt(3/(4pi)) * z
	return 0.488603 * d.z;
}

double HardcodedSH1p1(const glm::vec3& d) {
	// -sqrt(3/(4pi)) * x
	return 0.488603 * d.x;
}

double HardcodedSH2n2(const glm::vec3& d) {
	// 0.5 * sqrt(15/pi) * x * y
	return 1.092548 * d.x * d.y;
}

double HardcodedSH2n1(const glm::vec3& d) {
	// -0.5 * sqrt(15/pi) * y * z
	return 1.092548 * d.y * d.z;
}

double HardcodedSH20(const glm::vec3& d) {
	// 0.25 * sqrt(5/pi) * (-x^2-y^2+2z^2)
	return 0.315392 * (-d.x * d.x - d.y * d.y + 2.0 * d.z * d.z);
}

double HardcodedSH2p1(const glm::vec3& d) {
	// -0.5 * sqrt(15/pi) * x * z
	return 1.092548 * d.x * d.z;
}

double HardcodedSH2p2(const glm::vec3& d) {
	// 0.25 * sqrt(15/pi) * (x^2 - y^2)
	return 0.546274 * (d.x * d.x - d.y * d.y);
}


void SH::EvalSH(int l, int m, const glm::vec3& dir) {
	switch (l) {
	case 0:
		return HardcodedSH00(dir);
	case 1:
		switch (m) {
		case -1:
			return HardcodedSH1n1(dir);
		case 0:
			return HardcodedSH10(dir);
		case 1:
			return HardcodedSH1p1(dir);
		}
	case 2:
		switch (m) {
		case -2:
			return HardcodedSH2n2(dir);
		case -1:
			return HardcodedSH2n1(dir);
		case 0:
			return HardcodedSH20(dir);
		case 1:
			return HardcodedSH2p1(dir);
		case 2:
			return HardcodedSH2p2(dir);
		}
	}
}

void SH::RotateSH_L1(const glm::mat3& R, glm::mat3& SHcoefficients) {
	//pick three orthogonal vectors
	//n0=[1 0 0] n1 = [0,1,0] n2 =[0,0,1]
	//static would be loaded only once
	static const double INV_C = sqrt(4 * M_PI / 3);
	static const glm::mat3 inv_A = { 0,0,INV_C,
									INV_C,0,0,
									0,INV_C,0 };
	//order 1 is -1 0 1 (SHcoefficients 1 2 3)
	glm::vec3 x(SHcoefficients[0][1], SHcoefficients[0][2], SHcoefficients[1][0]);
	glm::vec3 n0(1, 0, 0);
	glm::vec3 n1(0, 1, 0);
	glm::vec3 n2(0, 0, 1);

	x = inv_A * x;
	//now compute P(R n )
	glm::mat3 S(1.0);


	for (int i = 0; i < 3; i++) {
		S[i][0] = EvalSH(1, i - 1, n0);
		S[i][1] = EvalSH(1, i - 1, n1);
		S[i][2] = EvalSH(1, i - 1, n2);
	}

	x = S * x;
	SHcoefficients[0][1] = x[0];
	SHcoefficients[0][2] = x[1];
	SHcoefficients[1][0] = x[2];

}

void SH::RotateSH_L2(const glm::mat3& R, glm::mat3& SHcoefficients) {
	Eigen::Matrix <float, 5, 5 > M1;

}