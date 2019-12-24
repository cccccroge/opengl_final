#include "ShaderProgram.h"
#include <iostream>
#include "GLM/gtc/type_ptr.hpp"
#include "../utils.h"

ShaderProgram::ShaderProgram()
{
    id = glCreateProgram();
}


ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(id);
}


void ShaderProgram::addShader(Shader &shader)
{
    if (!shader.getStatus()) {
        std::cout << "Try to attach invalid shader to program" << std::endl;
        return;
    }

    glAttachShader(id, shader.getId());
}


bool ShaderProgram::compile()
{
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (success != GL_TRUE)
    {
        GLsizei length;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar info[80];
        glGetProgramInfoLog(id, length, nullptr, info);
        
        std::cout << "Failed to link program:" << std::endl;
        std::cout << info << std::endl;

        return false;
    }

    return true;
}


void ShaderProgram::bind()
{
    glUseProgram(id);
}


void ShaderProgram::unbind()
{
    glUseProgram(0);
}


GLint ShaderProgram::getUniform(const char *name)
{
    GLint loc = glGetUniformLocation(id, name);
    if (loc == -1) {
        std::cout << "couldn't find uniform: " << std::string(name) << std::endl;
    }

    return  loc;
}


void ShaderProgram::setUniformMat4(const char *name, glm::mat4 matrix)
{
    glUniformMatrix4fv(getUniform(name), 1, false, glm::value_ptr(matrix));
}


void ShaderProgram::setUniform1i(const char *name, GLint val)
{
    glUniform1i(getUniform(name), val);
}

void ShaderProgram::setUniform1f(const char *name, GLfloat val)
{
    glUniform1f(getUniform(name), val);
}

void ShaderProgram::setUniformVec3(const char *name, glm::vec3 vec)
{
    glUniform3fv(getUniform(name), 1, glm::value_ptr(vec));
}
