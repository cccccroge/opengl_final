#include "VertexArray.h"
#include "VertexLayout.h"

VertexArray::VertexArray() : vao(0), vbo(0), ebo(0)
{

}

VertexArray::VertexArray(std::vector<VertexP2T> &vertices) : 
    vao(0), vbo(0), ebo(0)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex: data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexP2T),
        &vertices[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0); //TODO: can unbind?

    // Vertex: specification
    VertexLayout layout({ 2, 2 });
    int stride = layout.getStride();

    for (int i = 0; i < layout.getAttribNum(); ++i) {
        glEnableVertexAttribArray(i);

        GLint count = layout.getCount(i);
        GLvoid *offset = (GLvoid*)layout.getOffset(i);
        glVertexAttribPointer(i, count, GL_FLOAT, GL_FALSE, stride, offset);
    }

    glBindVertexArray(0);
}

VertexArray::VertexArray(std::vector<VertexPNT> &vertices, 
    std::vector<GLuint> &indices) : vao(0), vbo(0), ebo(0)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex: data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexPNT),
        &vertices[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        &indices[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Vertex: specification
    VertexLayout layout({ 3, 3, 2 });
    int stride = layout.getStride();

    for (int i = 0; i < layout.getAttribNum(); ++i) {
        glEnableVertexAttribArray(i);

        GLint count = layout.getCount(i);
        GLvoid *offset = (GLvoid*)layout.getOffset(i);
        glVertexAttribPointer(i, count, GL_FLOAT, GL_FALSE, stride, offset);
    }

    glBindVertexArray(0);
}

VertexArray::VertexArray(std::vector<VertexP> &vertices) : 
    vao(0), vbo(0), ebo(0)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex: data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexP),
        &vertices[0], GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vertex: specification
    VertexLayout layout({ 3, });
    int stride = layout.getStride();

    for (int i = 0; i < layout.getAttribNum(); ++i) {
        glEnableVertexAttribArray(i);

        GLint count = layout.getCount(i);
        GLvoid *offset = (GLvoid*)layout.getOffset(i);
        glVertexAttribPointer(i, count, GL_FLOAT, GL_FALSE, stride, offset);
    }

    glBindVertexArray(0);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void VertexArray::bind()
{
    glBindVertexArray(vao);
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}
