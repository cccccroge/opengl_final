#pragma once

#include "GLEW/glew.h"
#include "Shader.h"
#include "GLM/mat4x4.hpp"


class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    void addShader(Shader &shader);
    bool compile();
    void bind();
    void unbind();
    void setUniformMat4(const char *name, glm::mat4 matrix);
    void setUniform1i(const char *name, GLint val);
    void setUniform1f(const char *name, GLfloat val);
    void setUniformVec3(const char *name, glm::vec3 vec);

private:
    GLint getUniform(const char *name);
    GLuint id;
};