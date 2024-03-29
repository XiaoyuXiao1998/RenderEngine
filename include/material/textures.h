#pragma once
#include <string>
#include <vector>
#include <memory>
#include "glad/glad.h"
using namespace  std;
class Texture {
public:
    unsigned int glResourceID;
    string path;
    void* data = nullptr;
    Texture(int width,
        int height,
        GLenum format,
        GLint internal_format,
        GLint s_wrap_param = GL_REPEAT,
        GLint t_wrap_param = GL_REPEAT,
        GLenum data_type = GL_UNSIGNED_BYTE,
        GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
        GLint max_filter = GL_LINEAR, bool generateMips = true);
    int width, height;
    GLenum format;
    GLint internalFormat;
    GLint sWrapParam = GL_REPEAT, tWrapParam = GL_REPEAT;
    GLenum dataType = GL_UNSIGNED_BYTE;
    GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLint maxFilter = GL_LINEAR;
    bool generateMips = true;
    explicit Texture(const string& path);
    Texture();
    ~Texture();
    virtual void SetupGL();
    virtual void Save(string path);
};
class HDRTexture :public Texture {
public:
    explicit HDRTexture(const string& path);
};


unsigned int loadCubeMap(vector<std::string> faces);
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);



