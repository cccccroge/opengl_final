#include "Shader.h"
#include <iostream>
#include "../utils.h"


Shader::Shader(GLuint type, const std::string glsl_file) : status(false), 
    type(type)
{
    id = glCreateShader(type);

    // put string of glsl file to the shader
    std::string src_str = file2String(glsl_file);
    const char *src_cstr = src_str.c_str(); 

    glShaderSource(id, 1, &src_cstr, NULL);
    glCompileShader(id);
    std::cout << "validating shader:" << type << std::endl;
    validate();
}


Shader::~Shader()
{
    glDeleteShader(id);
}


// Check compile status
bool Shader::validate()
{
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (success != GL_TRUE) {
        status = false;

        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar info[80];
        glGetShaderInfoLog(id, length, nullptr, info);

        std::cout << "Failed to compile shader:" << std::endl;
        std::cout << info << std::endl;

        return false;
    }

    status = true;

    return true;
}