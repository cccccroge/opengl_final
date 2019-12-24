#pragma once

#include <string>
#include "GLEW/glew.h"
#include "../render/ShaderProgram.h"
#include <string>


class Texture
{
public:
    Texture(const char *path);
    Texture(const int width, const int height);
    Texture(int usage, const int width, const int height);  //TODO: generalize?
    ~Texture();

    void setUp();
    void bind(ShaderProgram &program, const std::string sampler_name, 
        const int index);
    void bind();

    inline const char* getPath() { return path; }
    inline GLuint getTbo() { return tbo; }

private:
    int width;
    int height;
    unsigned char *data;
    std::string type;   //TODO: what to do with this?
    const char *path;   // absolute path get from Model

    GLuint tbo;
};