#include <iostream>
#include <filesystem>

#include "../../include/material/textures.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>
Texture::Texture(
    const string& path)
    : path(path) {

    void* load_data;
    int nrComponents;
    load_data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if (load_data) {
        data = malloc(width * height * nrComponents * sizeof(unsigned char));
        memcpy(data, load_data, sizeof(unsigned char) * width * height * nrComponents);
        stbi_image_free(load_data);
    }
    if (data != nullptr) {
        if (nrComponents == 1) {
            format = GL_RED;
            internalFormat = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            internalFormat = GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
            internalFormat = GL_RGBA;
        }
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
}
void Texture::SetupGL() {
    glGenTextures(1, &glResourceID);
    glBindTexture(GL_TEXTURE_2D, glResourceID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);
    if (generateMips) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
}

Texture::Texture() {}
Texture::~Texture() {
    if (data != nullptr) {
        free(data);
        data = nullptr;
    }
}
Texture::Texture(int width,
    int height,
    GLenum format,
    GLint internal_format,
    GLint s_wrap_param,
    GLint t_wrap_param,
    GLenum data_type,
    GLint min_filter,
    GLint max_filter, bool generateMips)
    : width(width),
    height(height),
    format(format),
    internalFormat(internal_format),
    sWrapParam(s_wrap_param),
    tWrapParam(t_wrap_param),
    dataType(data_type),
    minFilter(min_filter),
    maxFilter(max_filter), generateMips(generateMips) {}
void Texture::Save(string path) {

    //  GLsizei nrChannels = 3;
    //  GLsizei stride = nrChannels * width;
    //  stride += (stride % 4) ? (4 - stride % 4) : 0;
    //  GLsizei bufferSize = stride * height;
    //  std::vector<char> buffer(bufferSize);
    //  glPixelStorei(GL_PACK_ALIGNMENT, 4);
    //  glReadBuffer(GL_FRONT);
    //  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    //
    //  stbi_flip_vertically_on_write(true);
    //  stbi_write_png(path.c_str(), width, height, nrChannels, buffer.data(), stride);
}
HDRTexture::HDRTexture(const string& path) {
    this->path = path;
    void* load_data;
    int nrComponents;
    load_data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    if (load_data) {
        data = malloc(width * height * nrComponents * sizeof(float));
        memcpy(data, load_data, sizeof(float) * width * height * nrComponents);
        stbi_image_free(load_data);
    }
    if (data == nullptr) {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    format = GL_RGB;
    internalFormat = GL_RGB32F; dataType = GL_FLOAT;
    sWrapParam = GL_CLAMP_TO_EDGE;
    tWrapParam = GL_CLAMP_TO_EDGE;
    minFilter = GL_LINEAR;
    maxFilter = GL_LINEAR;
}


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
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width , height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
          
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return textureID;
}