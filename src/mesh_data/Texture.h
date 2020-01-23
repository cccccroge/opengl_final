#pragma once

#include <string>
#include "GLEW/glew.h"
#include "../render/ShaderProgram.h"
#include <string>


enum class TEXTURE_TYPE
{
	NONE,
	DIFFUSE,
	SPECULAR,
	NORMAL,
};

class Texture
{
public:
    Texture(const char *path, TEXTURE_TYPE t = TEXTURE_TYPE::NONE);
    Texture(const int width, const int height, TEXTURE_TYPE t = TEXTURE_TYPE::NONE);
    Texture(int usage, const int width, const int height, 
		TEXTURE_TYPE t = TEXTURE_TYPE::NONE);  //TODO: generalize?
    ~Texture();

    void setUp();
    void bind(ShaderProgram &program, const std::string sampler_name, 
        const int index);
    void bind();

    inline const char* getPath() { return path; }
    inline GLuint getTbo() { return tbo; }
	inline TEXTURE_TYPE getMatType() { return mat_type; }

private:
    int width;
    int height;
    unsigned char *data;
	TEXTURE_TYPE mat_type;
    const char *path;   // absolute path get from Model

    GLuint tbo;
};