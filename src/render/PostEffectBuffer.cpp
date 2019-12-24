#include "PostEffectBuffer.h"
#include "../utils.h"


PostEffectBuffer::PostEffectBuffer(const int width, const int height) :
    /*screen_mesh(),*/ frame_buffer(), screen_tex(width, height),
    render_buffer(width, height)
{
    // create screen mesh (two triangles filling up whole screen)
	float screenVertices[] = { 
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    std::vector<VertexP2T> vertices;
    for (int i = 0; i < 6; ++i) {
        vertices.push_back(VertexP2T(
            glm::vec2(screenVertices[4*i], screenVertices[4*i+1]), 
            glm::vec2(screenVertices[4*i+2], screenVertices[4*i+3]))
        );
    }

    screen_mesh = new Mesh(vertices);

    // attach a texture buffer and render buffer
    frame_buffer.attachTexture(screen_tex, GL_COLOR_ATTACHMENT0);
    frame_buffer.attachRenderBuffer(render_buffer);
    frame_buffer.validate();
}

PostEffectBuffer::~PostEffectBuffer()
{
    if (screen_mesh != NULL) {
        delete screen_mesh;
        screen_mesh = NULL;
    }
}

void PostEffectBuffer::bindFrameBuffer()
{
    frame_buffer.bind();
}


void PostEffectBuffer::unbindFrameBuffer()
{
    frame_buffer.unbind();
}


void PostEffectBuffer::bindScreen()
{
    screen_mesh->bind();
    screen_tex.bind();
}

void PostEffectBuffer::bindMeshOnly()
{
    screen_mesh->bind();
}

