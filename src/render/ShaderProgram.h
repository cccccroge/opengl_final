#pragma once

#include "GLEW/glew.h"
#include "Shader.h"
#include "GLM/mat4x4.hpp"
#include <vector>


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
    void setUniformMat4v(const char* name, const int num, glm::mat4 matrix);
    void setUniform1i(const char *name, GLint val);
    void setUniform1f(const char *name, GLfloat val);
    void setUniform1d(const char* name, GLdouble val);
    void setUniformVec2(const char *name, glm::vec2 vec);
    void setUniformVec3(const char *name, glm::vec3 vec);
    void setUniformVec3v(const char *name, const int num, 
        std::vector<glm::vec3> &vectors);

private:
    GLint getUniform(const char *name);
    GLuint id;
};