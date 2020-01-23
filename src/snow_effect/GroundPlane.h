#pragma once
#include "GLEW/glew.h"
#include "freeGLUT/freeglut.h"
#include "freeGLUT/freeglut_ext.h"
#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
using namespace glm;
#include <iostream>
#include <fstream>
#include <GLM/gtc/type_ptr.hpp> // glm::value_ptr

class GroundPlane
{
public:
	GroundPlane(GLuint _programID);
	~GroundPlane();

	GLuint Draw();
private:
	unsigned char * textureDataBuffer;
	GLuint TextureID;
	GLuint programID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint VertexArrayID;
	vec3 position;
	vec3 scale;
	GLuint modelMatrixID;
	GLuint ModelMatrixID;

	GLuint ColourID;

};

