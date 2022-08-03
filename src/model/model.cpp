#include"../../include/model/model.h"


void Model::setCubeMapTextures(vector<std::string> faces) {
    

    MAT::Material* skybox_material = new MAT::SkyBoxMaterial();
    skybox_material->setMaterialType(MAT::Material_type::SKYBOX);
    skybox_material->setCubeMapId(loadCubeMap(faces));
    mesh_materials.emplace_back(skybox_material);
    //set material_index;
    meshes[0].mMaterialIndex = 0;
    

}

void Model::setCubeMapTextures(const char* path)
{
    constexpr unsigned int mapResolution = 128;

    MAT::Material* skybox_material = new MAT::SkyBoxMaterial();
    skybox_material->setMaterialType(MAT::Material_type::SKYBOX);

    //**************gen CubeMap from the HDR************************
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
            mapResolution,
            mapResolution, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    HDRTexture hdrTexture(path);
    hdrTexture.SetupGL();
    //***************use this hdr texture to generate cube map*********************************

    unsigned int HDRCubeMapFBO;
    unsigned int HDRCubeMapRBO;
    glGenFramebuffers(1, &HDRCubeMapFBO);
    glGenRenderbuffers(1, &HDRCubeMapRBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mapResolution, mapResolution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, HDRCubeMapRBO);

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

    Shader HdrToCubeMapCalculateShader("../../../shader/IBL/EnvDiffuseIrradiance.vert", "../../../shader/CubeMapFromHdr.frag");
    HdrToCubeMapCalculateShader.use();
    HdrToCubeMapCalculateShader.setInt("HDRMap",0);
    HdrToCubeMapCalculateShader.setMat4("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,hdrTexture.glResourceID );
    glViewport(0, 0, mapResolution, mapResolution); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, HDRCubeMapFBO);


    for (unsigned int i = 0; i < 6; i++) {

        HdrToCubeMapCalculateShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        meshes[0].Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLuint b[] = { HDRCubeMapFBO,HDRCubeMapRBO };
    glDeleteBuffers(2, b);


  

    //***************set gennerate Cubema***************************
    skybox_material->setCubeMapId(textureID);
    mesh_materials.emplace_back(skybox_material);
    //set material_index;
    meshes[0].mMaterialIndex = 0;
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


void Model::loadMaterials(const aiScene* scene,MAT::Material_type material_type) {
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        if (material_type == MAT::Material_type::MATALIC ) {
            aiString bc_str, mr_str, normal_str, AO_str, height_str;
            ai_real metallicFactor, roughnessFactor, alphaCutOff;
            aiString alphaMode;
            aiColor4D bc_factor;
            bool doubleSided = false;
            aiMaterial* mat = scene->mMaterials[i];
            mat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &bc_str);
            mat->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &mr_str);
            mat->GetTexture(aiTextureType_NORMALS, 0, &normal_str);
            mat->GetTexture(aiTextureType_HEIGHT, 0, &height_str);
            mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &AO_str);
            mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, bc_factor);
            mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, metallicFactor);
            mat->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, roughnessFactor);
            mat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, alphaCutOff);
            mat->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode);
            mat->Get(AI_MATKEY_TWOSIDED, doubleSided);

            //test whether path is correct
            //cout <<( normal_str.length > 0 ? directory + "/" + normal_str.C_Str() : "" )<< endl;
            MAT::Material* metalic_material = new MAT::MetalicMaterial(glm::vec4(bc_factor.r, bc_factor.g, bc_factor.b, bc_factor.a), metallicFactor,
                roughnessFactor,
                bc_str.length > 0 ? directory + "/" + bc_str.C_Str() : "",
                mr_str.length > 0 ? directory + "/" + mr_str.C_Str() : "",
                AO_str.length > 0 ? directory + "/" + AO_str.C_Str() : "",
                normal_str.length > 0 ? directory + "/" + normal_str.C_Str() : "",
                height_str.length > 0 ? directory + "/" + height_str.C_Str() : "",
                string(alphaMode.C_Str()).compare("MASK") ? 0 : 1,
                alphaCutOff,
                doubleSided
            );

            mesh_materials.emplace_back(metalic_material);
            if (bc_str.length > 0) {
                loadMaterialTextures(mat, aiTextureType_DIFFUSE);
            }
            if (mr_str.length > 0) {
                loadMaterialTextures(mat, aiTextureType_UNKNOWN);
            }
            if (bc_str.length > 0) {
                loadMaterialTextures(mat, aiTextureType_NORMALS);
            }
            if (AO_str.length > 0) {
                loadMaterialTextures(mat, aiTextureType_AMBIENT_OCCLUSION);
            }
            if (height_str.length > 0) {
                loadMaterialTextures(mat, aiTextureType_HEIGHT);
            }

            for (auto& kv : textures_loaded) {
                kv.second->SetupGL();
            }
         //   cout << bc_str.C_Str() << endl << mr_str.C_Str() << endl << normal_str.C_Str() << endl;


            //create a base pointer
           // MAT::Material* mat = new MAT::MetalicMaterial();
        }

    }
}


vector<shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    vector<shared_ptr<Texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        string texture_path = directory + "/" + str.C_Str();
        for (auto& j : textures_loaded) {
            if (std::strcmp(j.second->path.data(), texture_path.data()) == 0) {

                textures.push_back(j.second);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {   // if texture hasn't been loaded already, load it
            shared_ptr<Texture> texture = make_shared<Texture>(texture_path);
            textures.push_back(texture);

           
            textures_loaded[texture_path] =
                texture;  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}