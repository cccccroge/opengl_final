#include "Model.h"
#include "../utils.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <iostream>
#include "../mesh_data/Vertex.h"


Model::Model() : SceneObject(glm::vec3(0.0f, 0.0f, 0.0f))
{

}


Model::Model(const std::string obj_file, 
	glm::vec3 pos /*= glm::vec3(0.0f, 0.0f, 0.0f)*/) : SceneObject(pos)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(obj_file, 
		aiProcess_Triangulate | aiProcess_GenNormals | 
		aiProcess_CalcTangentSpace);

	if (!scene || 
		scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << 
			importer.GetErrorString() << std::endl;
		return;
	}

	directory = obj_file.substr(0, obj_file.find_last_of('/') + 1);

	processNode(scene, scene->mRootNode);
}

Model::~Model()
{
	for (auto mPtr : meshes) {
		if (mPtr != NULL) {
			delete mPtr;
			mPtr = NULL;
		}
	}
	meshes.clear();

	for (auto tPtr : textures_loaded) {
		if (tPtr != NULL) {
			delete tPtr;
			tPtr = NULL;
		}
	}
	textures_loaded.clear();
}


void Model::processNode(const aiScene *scene, const aiNode *node)
{
	for (int i = 0; i < node->mNumMeshes; ++i) {
		Mesh *m = convertMesh(scene, scene->mMeshes[node->mMeshes[i]]);
		m->setUp();
		meshes.push_back(m);
	}

	for (int i = 0; i < node->mNumChildren; ++i) {
		processNode(scene, node->mChildren[i]);
	}
}


Mesh* Model::convertMesh(const aiScene *scene, const aiMesh *mesh)
{
	Mesh *targetMesh = new Mesh();
	
	// retrieve vertex data
	for (int i = 0; i < mesh->mNumVertices; ++i) {
		aiVector3D &v = mesh->mVertices[i];
		aiVector3D &n = mesh->mNormals[i];
		aiVector3D *tc = (mesh->mTextureCoords[0]) ? 
			&mesh->mTextureCoords[0][i] : NULL;
		aiVector3D *ts = (mesh->mTangents) ?	// used in normal mapping
			&mesh->mTangents[i] : NULL;

		glm::vec3 pos(v.x, v.y, v.z);
		glm::vec3 normal(n.x, n.y, n.z);
		glm::vec2 texCoord = tc ? 
			glm::vec2(tc->x, tc->y) : glm::vec2(0.0f, 0.0f);
		glm::vec3 tanSpace = ts ?
			glm::vec3(ts->x, ts->y, ts->z) : glm::vec3(0.0f, 0.0f, 0.0f);

		VertexPNTS vertex(pos, normal, texCoord, tanSpace);
		targetMesh->pushVertexPNTS(vertex);
	}

	// retrieve index data
	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j) {
			targetMesh->pushIndex(face.mIndices[j]);
		}
	}

	// retrieve texture data
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

		std::cout << "converting aiMesh: retrieving textures..." << std::endl;
		std::cout << "DIFFUSE: " << mat->GetTextureCount(aiTextureType_DIFFUSE)
			<< "x" << std::endl;
		std::cout << "SPECULAR: " << mat->GetTextureCount(aiTextureType_SPECULAR)
			<< "x" << std::endl;
		std::cout << "SHININESS: " << mat->GetTextureCount(aiTextureType_SHININESS)
			<< "x" << std::endl;
		std::cout << "NORMALS: " << mat->GetTextureCount(aiTextureType_NORMALS)
			<< "x" << std::endl;
		std::cout << "DISPLACEMENT: " << mat->GetTextureCount(aiTextureType_DISPLACEMENT)
			<< "x" << std::endl; 
		std::cout << "HEIGHT: " << mat->GetTextureCount(aiTextureType_HEIGHT)
			<< "x" << std::endl;
		std::cout << "UNKNOWN: " << mat->GetTextureCount(aiTextureType_UNKNOWN)
			<< "x" << std::endl; 

		retrieveTextures(mat, aiTextureType_DIFFUSE, targetMesh);
		retrieveTextures(mat, aiTextureType_SPECULAR, targetMesh);
		retrieveTextures(mat, aiTextureType_SHININESS, targetMesh);
		retrieveTextures(mat, aiTextureType_NORMALS, targetMesh);
		retrieveTextures(mat, aiTextureType_DISPLACEMENT, targetMesh);
		retrieveTextures(mat, aiTextureType_HEIGHT, targetMesh);
		
		// TODO: multiple same maps?
	}

	return targetMesh;
}


void Model::retrieveTextures(const aiMaterial *mat, const aiTextureType type, Mesh *target)
{
	std::cout << "converting textures..." << std::endl;

	for (int i = 0; i < mat->GetTextureCount(type); ++i) {

		aiString str;
		mat->GetTexture(type, i, &str);
		// convert to relative path if is absolute
		std::string current(str.C_Str());
		std::size_t absolute = current.find_last_of('/');
		if (absolute != std::string::npos) {
			current = current.substr(absolute + 1);
		}

		bool found = false;
		for (int j = 0; j < textures_loaded.size(); ++j) {
			// found in cache
			if (std::string(textures_loaded[j]->getPath()) ==
				directory + current) {
				std::cout << "found in cache: " << current << std::endl;
				target->pushTexture(textures_loaded[j]);
				found = true;
				break;
			}
		}

		// not found in cache
		if (!found) {
			std::cout << "not in cache: " << current << std::endl;

			TEXTURE_TYPE t;
			if (type == aiTextureType_DIFFUSE)
				t = TEXTURE_TYPE::DIFFUSE;
			else if(type == aiTextureType_SPECULAR || type == aiTextureType_SHININESS)
				t = TEXTURE_TYPE::SPECULAR;
			else if (type == aiTextureType_NORMALS || type == aiTextureType_DISPLACEMENT
					|| type == aiTextureType_HEIGHT)
				t = TEXTURE_TYPE::NORMAL;
			
			Texture *tex = new Texture((directory + current).c_str(), t);
			target->pushTexture(tex);
			textures_loaded.push_back(tex);
		}
	}
}
