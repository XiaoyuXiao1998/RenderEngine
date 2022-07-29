#pragma once
#include<string>
#include <glm/glm.hpp>
using namespace std;

namespace MAT{


enum Material_type {
SKYBOX,
SPECULAR,
MATALIC
};

class Material {
	//default is SPECULAR
	Material_type material_type;

public:
	Material() {
		material_type = SPECULAR;
	}
	Material(Material_type material_type,
	 string AO ,string Normal,string Height) {
		this->material_type = material_type;
		this->AO = AO;
		this->Normal = Normal;
		this->Height = Height;
	}
	Material(Material_type material_type) {
		this->material_type = material_type;
	}

	Material_type getMaterialType() {
		return material_type;
	}
	void setMaterialType(Material_type material_type) {
		this->material_type = material_type;
	}
	virtual unsigned int getCubeMapId() { return -1; }
	virtual string getbaseColorTexture() { return ""; }
	virtual string getmetallicRoughnessTexture() { return ""; }
	virtual void setCubeMapId(unsigned int id) { return; }
	virtual glm::vec4 getBaseColorFactor() { return glm::vec4(1.0); }
	virtual int getBaseColorMode() { return 0; }
	virtual int getMetallicMode() { return 0; }
	virtual int getRoughnessMode() { return 0; }
	virtual float getMetallicFactor() { return -1; }
	virtual float getRoughnessFactor() { return -1; }

public:
	//common texture for Specular and metalic
	string AO;
	string Normal;
	string Height;




};


class MetalicMaterial :public Material {
public:
	string baseColorTexture;
	string metallicRoughnessTexture;

	glm::vec4 baseColorFactor;
	float metallicFactor = 0;
	float roughnessFactor = 0;

	//set rendering mode
	int bcMode = 0, metallicMode = 0, roughnessMode = 0;

	//TODO: 
	bool doubleSided = false;
	float alphaCutOff = 0.5;
	int  alphaMode = 0;

	virtual string getbaseColorTexture() { return baseColorTexture; }
	virtual string getmetallicRoughnessTexture() { return metallicRoughnessTexture; }
	virtual glm::vec4 getBaseColorFactor() { return baseColorFactor; }
	virtual int getBaseColorMode() { return bcMode; }
	virtual int getMetallicMode() { return metallicMode; }
	virtual int getRoughnessMode() { return roughnessMode; }
	virtual float getMetallicFactor() { return metallicFactor; }
	virtual float getRoughnessFactor() { return roughnessFactor; }

	MetalicMaterial(const glm::vec4& base_color_factor,
		float metallic_factor,
		float roughness_facotr,
		const string& base_color_texture,
		const string& metallic_roughness_texture,
		const string& AO_texture,
		const string& normal_texture,
		const string& Height_texture,
		int alphaMode, float alphaCutOff, bool doubleSided) :
		Material(Material_type::MATALIC,AO_texture,normal_texture,Height_texture),
		baseColorFactor(base_color_factor),
		metallicFactor(metallic_factor),
		roughnessFactor(roughness_facotr),
		baseColorTexture(base_color_texture),
		metallicRoughnessTexture(metallic_roughness_texture),
		alphaMode(alphaMode), alphaCutOff(alphaCutOff), doubleSided(doubleSided)

	{
		//if no base color texture
		// mode =2:textures
		// mode =4:base_color
		// mode =6:have base color and texture
		if (!baseColorTexture.empty()) {
			bcMode += 1 << 1;
		}

		if (baseColorFactor != glm::vec4(0)) {
			bcMode += 1 << 2;
	
		}
		
		if (!metallic_roughness_texture.empty()) {
			metallicMode += 1 << 1; roughnessMode += 1 << 1;
		}
		if (roughnessFactor != 0) {
			roughnessMode += 1 << 2;
		}
		if (metallicFactor != 0) {
			metallicMode += 1 << 2;
		}

	}

};

class SkyBoxMaterial :public Material {
	unsigned int CubeMapId;
	glm::vec3 ka;
	glm::vec3 kb;
	glm::vec3 ks;
	virtual unsigned int getCubeMapId() { return CubeMapId; }
	virtual void setCubeMapId(unsigned int id) { this->CubeMapId = id; }
};

}



