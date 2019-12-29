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
void Mesh::bind(ShaderProgram &program)
{
    //std::cout << "binding textures: " << textures.size() << " x" << std::endl;
	program.setUniform1i("material.has_diff_map", 0);
	program.setUniform1i("material.has_spec_map", 0);
	program.setUniform1i("material.has_norm_map", 0);

    for (int i = 0; i < textures.size(); ++i) {
		if (textures[i]->getMatType() == TEXTURE_TYPE::DIFFUSE) {
			textures[i]->bind(program, "material.diffuseMap", i);
			program.setUniform1i("material.has_diff_map", 1);
		}
		else if (textures[i]->getMatType() == TEXTURE_TYPE::SPECULAR) {
			textures[i]->bind(program, "material.specularMap", i);
			program.setUniform1i("material.has_spec_map", 1);
		}
		else if (textures[i]->getMatType() == TEXTURE_TYPE::NORMAL) {
			/*textures[i]->bind(program, "material.normalMap", i);
			program.setUniform1i("material.has_norm_map", 1);*/
		}
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