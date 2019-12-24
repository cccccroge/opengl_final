#pragma once

#include "GLEW/glew.h"

class RenderBuffer
{
public:
    RenderBuffer();
    RenderBuffer(const int width, const int height);
    ~RenderBuffer();

    void bind();
    void unbind();

    inline GLuint getRbo() { return rbo; }
 
private:
    GLuint rbo;
    int width;
    int height;
};