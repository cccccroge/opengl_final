#include "Mesh.h"
#include <iostream>
#include "../utils.h"

Mesh::Mesh() : vertex_arr()
{
    // we can't call setUp() unitil assimp importer
    // from model get the actual data
}

Mesh::Mesh(std::vector<VertexP2T> &vertices)
{
    vertex_arr = new VertexArray(vertices);
}

Mesh::Mesh(std::vector<VertexPNT> &vertices, std::vector<GLuint> &indices)
{
    vertex_arr = new VertexArray(vertices, indices);
}

Mesh::Mesh(std::vector<VertexP> &vertices)
{
    vertex_arr = new VertexArray(vertices);
}

Mesh::~Mesh()
{
    // note: no need to delete texture element, we will delete them in Model's cache
    textures.clear();

    if (vertex_arr != NULL) {
        delete vertex_arr;
        vertex_arr = NULL;
    }
}


/* used for assimp loading */
void Mesh::setUp()
{
    vertex_arr = new VertexArray(verticesPNT, indices);
}


/* bind textures and vao */
void Mesh::bind(ShaderProgram &program, const std::string tex_prefix)
{
    //std::cout << "binding textures: " << textures.size() << " x" << std::endl;
    for (int i = 0; i < textures.size(); ++i) {
        textures[i]->bind(program, tex_prefix, i);
    }
    vertex_arr->bind();
}


/* only bind vao (when there's no texture in this mesh) */
void Mesh::bind()
{
    vertex_arr->bind();
}


/* only unbind vao (when there's no texture in this mesh) */
void Mesh::unbind()
{
    vertex_arr->unbind();
}