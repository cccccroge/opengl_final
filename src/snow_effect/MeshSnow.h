#pragma once
#ifndef MESH_H
#define MESH_H
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include "GLEW/glew.h" // Contains all the necessery OpenGL includes
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <assimp\types.h>

#include "ShaderSnow.h"

struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
	// Tangent
	glm::vec3 Tangent;
	// Bitangent
	glm::vec3 Bitangent;
};

struct TextureSnow {
	GLuint id;
	string type;
	aiString path;
};

class MeshSnow {
public:
	/*  MeshSnow Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<TextureSnow> textures;

	/*  Functions  */
	// Constructor
	MeshSnow(vector<Vertex> vertices, vector<GLuint> indices, vector<TextureSnow> textures);

	// Render the mesh
	void Draw(ShaderSnow shader);

	GLuint VAO;
	GLuint position_vbo;

private:
	/*  Render data  */
	GLuint VBO, EBO;

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh();
};


#endif