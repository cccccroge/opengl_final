#pragma once

#include "GLEW/glew.h"
#include <string>


class Shader
{
public:
    Shader(GLuint type, const std::string glsl_file);
    ~Shader();
    inline bool getStatus() { return status; }
    inline GLuint getId() { return id; }

private:
    bool validate();
    
    GLuint type;
    bool status;
    GLuint id;
};