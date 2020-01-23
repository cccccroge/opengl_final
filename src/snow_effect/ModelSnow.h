#pragma once
#ifndef MODEL_H
#define MODEL_H
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include "GLEW/glew.h" // Contains all the necessery OpenGL includes
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
//#include "SOIL/soil.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "MeshSnow.h"
#include "ShaderSnow.h"

GLint TextureFromFile(const char* path, string directory);

class ModelSnow
{
public:
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	ModelSnow(string const & path);

	ModelSnow(string const & path, bool Instanced);

	// Draws the model, and thus all its meshes
	void Draw(ShaderSnow shader);

	GLuint VAO;
	vector<MeshSnow> meshes;


private:
	/*  ModelSnow Data */
	vector<TextureSnow> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	string directory;


	/*  Functions   */
	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path);

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	MeshSnow processMesh(aiMesh* mesh, const aiScene* scene);
	
	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a TextureSnow struct.
	vector<TextureSnow> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	
};

#endif