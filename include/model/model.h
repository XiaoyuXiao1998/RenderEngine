#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/pbrmaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "../shader/shader.h"
#include"../material/textures.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


//used for model transform
struct ModelTransform {
    //translation
    float tx;
    float ty;
    float tz;
    //scale
    float sx;
    float sy;
    float sz ;
    //rotation angle;
    float theta;
    //rotation axis ;
    glm::vec3 rotation_axis;

    ModelTransform() {
        //translation
         tx = 0;
         ty = 0;
         tz = 0;
        //scale
         sx = 1.0;
         sy = 1.0;
         sz = 1.0;
        //rotation angle;
         theta = 0;
        //rotation axis ;
         rotation_axis = glm::vec3(0, 1, 0);
    }

    ModelTransform(float Tx, float Ty,float Tz){
        //translation
        tx = Tx;
        ty = Ty;
        tz = Tz;
        //scale
        sx = 1.0;
        sy = 1.0;
        sz = 1.0;
        //rotation angle;
        theta = 0;
        //rotation axis ;
        rotation_axis = glm::vec3(0, 1, 0);
    }
    ModelTransform(float Tx, float Ty, float Tz, float Sx, float Sy, float Sz) {
        //translation
        tx = Tx;
        ty = Ty;
        tz = Tz;
        //scale
        sx = Sx;
        sy = Sy;
        sz = Sz;
        //rotation angle;
        theta = 0;
        //rotation axis ;
        rotation_axis = glm::vec3(0, 1, 0);
    }
};


class Model
{
public:
    // model data 
    map<string, shared_ptr<Texture>>
        textures_loaded;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    vector<shared_ptr<MAT::Material>> mesh_materials;
    string directory;

    //three-oreder SH Light coefficients
    glm::mat3 aPrecomputeLR;
    glm::mat3 aPrecomputeLG;
    glm::mat3 aPrecomputeLB;

    bool gammaCorrection;
    unsigned int mapid;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, MAT::Material_type material_type = MAT::Material_type::SPECULAR, bool gamma = false) : gammaCorrection(gamma)
    {
        ModelTransform transform;
        loadModel(path,transform,material_type);
    }

    Model(string const& path, const ModelTransform& transform, MAT::Material_type material_type = MAT::Material_type::SPECULAR,bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path,transform,material_type);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader, unsigned int& depthMap,
        unsigned int ibl_diffuse_irradiance_map_id = -1,
        unsigned int ibl_prefilter_map_id = -1,
        unsigned int ibl_BRDFLUT_map_id = -1)
    {

        shader.use();
        for (auto& mesh : meshes) {
            auto material = mesh_materials[mesh.mMaterialIndex];
            if (material->getMaterialType() == MAT::Material_type::SKYBOX) {
                glActiveTexture(GL_TEXTURE0);
                shader.setInt("skybox", 0);
                //need to modified
                glBindTexture(GL_TEXTURE_CUBE_MAP, material->getCubeMapId());
                //glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_prefilter_map_id);
                glActiveTexture(GL_TEXTURE1);
                shader.setInt("shadowMap", 1);
                //need to modified
                glBindTexture(GL_TEXTURE_2D , depthMap);

            }


            else if (material->getMaterialType() == MAT::Material_type::MATALIC) {
                shader.setVec4("baseColorFactor", material->getBaseColorFactor());
                shader.setInt("baseColorMode", material->getBaseColorMode());
                shader.setInt("metallicMode", material->getMetallicMode());
                shader.setInt("roughnessMode", material->getRoughnessMode());
                shader.setFloat("metallicFactor", material->getMetallicFactor());
                shader.setFloat("roughnessFactor", material->getRoughnessFactor());

           

             

              

                if (ibl_BRDFLUT_map_id != -1) {


                    shader.setInt(" BRDFLUT", 0);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, ibl_BRDFLUT_map_id);
                }

                auto metallicRoughnessTexture = material->getmetallicRoughnessTexture();
                if (!metallicRoughnessTexture.empty()) {
                    shader.setInt("metallicRoughnessMap", 1);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, textures_loaded[metallicRoughnessTexture]->glResourceID);
                }
                auto baseColorTexture = material->getbaseColorTexture();
                if (!baseColorTexture.empty()) {
                    shader.setInt("baseColorMap", 2);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, textures_loaded[baseColorTexture]->glResourceID);
                }
               // auto NormalTexture;

                if (ibl_diffuse_irradiance_map_id != -1) {
                
                
                    shader.setInt("DiffuseIrradianceMap", 3);
                    glActiveTexture(GL_TEXTURE3);
                    
                    glBindTexture(GL_TEXTURE_CUBE_MAP,ibl_diffuse_irradiance_map_id);
                }

                if (ibl_prefilter_map_id != -1) {
                    shader.setInt("PrefilteredMap", 4);
                    glActiveTexture(GL_TEXTURE4);

                    glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_prefilter_map_id);
                }
            }
            mesh.Draw();
            glActiveTexture(GL_TEXTURE0);
        }
    }

    void loadMaterials(const aiScene* scene,MAT::Material_type material_type);
    void setCubeMapTextures(vector<std::string> faces);
    void setCubeMapTextures(const char* path);
    void loadPRTparameters(string path, Shader& shader);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type);


private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path , const ModelTransform& transform, MAT::Material_type material_type)
    {
        //get the file type
        string suffixStr = path.substr(path.find_last_of(".") + 1);
       // std::cout << suffixStr;
    
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));
        cout << directory << endl;

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene,transform,material_type);
        loadMaterials(scene, material_type);
        

    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene, const ModelTransform& transform, MAT::Material_type material_type)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, transform, material_type));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene,transform, material_type);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene , const ModelTransform& transform, MAT::Material_type material_type)
    {
    
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            //scale --> rotation -> translation
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = transform.sx * mesh->mVertices[i].x + transform.tx;
            vector.y = transform.sy * mesh->mVertices[i].y + transform.ty;
            vector.z = transform.sz * mesh->mVertices[i].z + transform.tz;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

     
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices,mesh->mMaterialIndex);
    }
};


#endif