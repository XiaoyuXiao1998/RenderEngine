#include"../../include/model/model.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;

    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubeMap(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;

    for (auto i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Model::setCubeMapTextures(vector<std::string> faces) {
    Texture skymapTexture;
    skymapTexture.id = loadCubeMap(faces);
    skymapTexture.type = "samplerCube";
    meshes[0].textures.emplace_back(std::move(skymapTexture));
    meshes[0].material_type = SAMPLER_CUBE;

}

void Model::loadPRTparameters(string path, Shader& shader) {
    string light_path = path + "/light.txt";
    string LT_path = path + "/transport.txt";
    vector<glm::vec3> preComputeL;
    vector<glm::mat3> preComputeLT;

    ifstream ifs;
    ifs.open(light_path);
    if (!ifs.is_open()) {
        cout << "real light coefficients files fail" << endl;
        return;
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
    ifs.open(LT_path);
    if (!ifs.is_open()) {
        cout << "real light transport coefficients files fail" << endl;
        return;

    }
    else {
        ifs >> temp;
        glm::mat3 LT;
        while (ifs >> LT[0][0]) {
            ifs >> LT[0][1] >> LT[0][2] >> LT[1][0] >> LT[1][1] >> LT[1][2] >> LT[2][0] >> LT[2][1] >> LT[2][2];
            //cout << LT[0][0] << " " << LT[0][1] << " " << LT[0][2] << " " << LT[1][0] << " " << LT[1][1] << " " << LT[1][2] << " " << LT[2][0] << " " << LT[2][1] << " " << LT[2][2] << endl;
            preComputeLT.push_back(std::move(LT));
        }
        preComputeLT.push_back(LT);
    }
    ifs.close();

    shader.use();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int index = i * 3 + j;
            aPrecomputeLR[i][j] = preComputeL[index].r;
            aPrecomputeLG[i][j] = preComputeL[index].g;
            aPrecomputeLB[i][j] = preComputeL[index].b;
        }
    }

    //set vertex light transport parameters
    size_t start = 0;
    for (int i = meshes.size()-1; i >= 0; i--) {
        for (int j = 0; j < meshes[i].vertices.size(); j++) {
            meshes[i].vertices[j].LT = preComputeLT[start + j];
        }
        
        start += meshes[i].vertices.size();
       
        meshes[i].loadAndSetPRTParameters();
    }



    //rebuffer datat to vertex shader


}