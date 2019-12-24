#pragma once

#include "GLEW/glew.h"
#include <vector>
#include <string>
#include "../render/ShaderProgram.h"


class CubemapTexture
{
public:
    CubemapTexture(const std::vector<std::string> paths);
    ~CubemapTexture();

    void bind(ShaderProgram &program, const std::string sampler_name, 
        const int index);
    void unbind(const int index);


private:
    std::vector<std::string> paths;
    GLuint tbo;
};