#pragma once

#include "GLEW/glew.h"
#include "FrameBuffer.h"
#include "../mesh_data/Texture.h"
#include "../mesh_data/Mesh.h"
#include "RenderBuffer.h"


/* This class serve as task specific frame buffer (do post effect) */
/* And it is actually not a 'FrameBuffer', it contains FrameBuffer */
class PostEffectBuffer
{
public:
    PostEffectBuffer(const int width, const int height);
    ~PostEffectBuffer();

    void bindFrameBuffer();
    void unbindFrameBuffer();

    void bindScreen();
    void bindMeshOnly();

private:
    Mesh *screen_mesh;

    FrameBuffer frame_buffer;
    Texture screen_tex;
    RenderBuffer render_buffer;
};