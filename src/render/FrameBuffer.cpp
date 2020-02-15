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


void FrameBuffer::attachCubemapTexture(CubemapTexture& tex, const GLenum attach_type)
{
    bind();
    glFramebufferTexture(GL_FRAMEBUFFER, attach_type, tex.getTbo(), 0);
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

void FrameBuffer::attachDepthBuffer(const int width, const int height)
{
    bind();

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    unbind();
}

bool FrameBuffer::validate()
{
    bool success = false;

    bind();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "invalid frame buffer" << std::endl;
    }
    else {
        success = true;
    }
    unbind();

    return success;
}

void FrameBuffer::colAttachNumber(const int size)
{
    bind();
    // only consider when size == 2
    if (size == 2) {
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
    }
    unbind();
}


void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}


void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
