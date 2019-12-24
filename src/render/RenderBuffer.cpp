#include "RenderBuffer.h"


RenderBuffer::RenderBuffer() : rbo(0), width(0), height(0)
{
    
}


RenderBuffer::RenderBuffer(const int width, const int height) : 
    rbo(0), width(width), height(height)
{
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 
        width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


RenderBuffer::~RenderBuffer()
{
    glDeleteRenderbuffers(1, &rbo);
}


void RenderBuffer::bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}


void RenderBuffer::unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


