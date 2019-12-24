#pragma once

#include "GLEW/glew.h"
#include <vector>
#include "Vertex.h"
#include "Texture.h"
#include "../scene/SceneObject.h"
#include "VertexArray.h"


/* a Mesh act like a node in Assimp */
class Mesh : public SceneObject
{
public:
    Mesh();
    Mesh(std::vector<VertexP2T> &vertices);
    Mesh(std::vector<VertexPNT> &vertices, std::vector<GLuint> &indices);
    Mesh(std::vector<VertexP> &vertices);
    ~Mesh();

    inline void pushVertexPNT(VertexPNT v) { verticesPNT.push_back(v); }
    inline void pushIndex(GLuint i) { indices.push_back(i); }
    inline void pushTexture(Texture* t) { textures.push_back(t); }

    inline int getIndicesNum() { return indices.size(); }

    void setUp();
    void bind(ShaderProgram &program, const std::string tex_prefix);
    void bind();
    void unbind();

private:
    std::vector<VertexPNT> verticesPNT;
    std::vector<GLuint> indices;
    std::vector<Texture*> textures;

    // assume each vertex in a Mesh has same format
    // so we use the same VertexArray to render
    VertexArray *vertex_arr;
};