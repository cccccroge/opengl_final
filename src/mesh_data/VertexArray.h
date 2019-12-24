#pragma once

#include "GLEW/glew.h"
#include <vector>
#include "Vertex.h"


class VertexArray
{
public:
    VertexArray();
    VertexArray(std::vector<VertexP2T> &vertices);
    VertexArray(std::vector<VertexPNT> &vertices, std::vector<GLuint> &indices);
    VertexArray(std::vector<VertexP> &vertices);

    ~VertexArray();

    void bind();
    void unbind();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;    
};