#pragma once

#include "GLEW/glew.h"
#include "GLM/vec3.hpp"
#include "TinyOBJ/tiny_obj_loader.h"
#include "../render/ShaderProgram.h"
#include "SceneObject.h"
#include "Camera.h"
#include <vector>
#include <string>
#include "../mesh_data/Mesh.h"
#include "assimp/scene.h"
#include "../mesh_data/Texture.h"


class Model : public SceneObject
{
public:
	Model();
    Model(const std::string obj_file, 
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f));
	~Model();

	inline std::vector<Mesh*>& getMeshes() { return meshes; }

private:
	void processNode(const aiScene *scene, const aiNode *node);
	Mesh* convertMesh(const aiScene *scene, const aiMesh *mesh);

private:
	std::vector<Mesh*> meshes;
	// where .obj & texture files located, including a '/' as tail
	std::string directory;

	std::vector<Texture*> textures_loaded;
};