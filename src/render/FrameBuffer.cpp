#include "FrameBuffer.h"
#include "../global.h"
#include <iostream>
#include "../mesh_data/Mesh.h"
#include "RenderBuffer.h"


FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &fbo);
}


FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &fbo);
}


void FrameBuffer::attachTexture(Texture &tex, const GLenum attach_type)
{
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attach_type, 
        GL_TEXTURE_2D, tex.getTbo(), 0);
    unbind();
}


void FrameBuffer::attachRenderBuffer(RenderBuffer &render_buffer)
{
    bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, render_buffer.getRbo());
    unbind();
}

void FrameBuffer::attachEmptyColorBuffer()
{
    bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    unbind();
}

bool FrameBuffer::validate()
{
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "invalid frame buffer" << std::endl;
        return false;
    }

    return true;
}


void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}


void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
