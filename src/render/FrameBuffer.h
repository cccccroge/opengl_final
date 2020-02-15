#pragma once

#include "GLEW/glew.h"
#include "../init.h"
#include "../mesh_data/Texture.h"
#include "../mesh_data/CubemapTexture.h"
#include "RenderBuffer.h"


class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();

    void attachTexture(Texture &tex, const GLenum attach_type);
    void attachCubemapTexture(CubemapTexture& tex, const GLenum attach_type);
    void attachRenderBuffer(RenderBuffer &render_buffer);
    void attachEmptyColorBuffer();
    void attachDepthBuffer(const int width, const int height);
    bool validate();

    void colAttachNumber(const int size);

    void bind();
    void unbind();

private:
    GLuint fbo;
};