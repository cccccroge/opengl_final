#pragma once

#include "../render/ShaderProgram.h"
#include "wave.h"

#define MESH_RESOLUTION 64

class WaterEffectObject
{
public:
	WaterEffectObject(const glm::vec3 offset, const float speed, const float scale);

	void initialize();
	void draw();

	inline void setOffset(const glm::vec3 newOffset) { offset = newOffset; }


public:
	ShaderProgram* program_ocean;
	int N;
	int M;
	int indexSize;
	GLuint surfaceVAO;
	GLuint surfaceVBO;
	GLuint EBO;

	Wave *wave_model;

	float L_x;
	float L_z;
	float A;
	float V; // Wind speed
	vec2 omega; // Wind direction

	glm::vec3 lampPos;
	glm::vec3 sundir;

	float waterTime;
	float modelScale;

private:
	void initBufferObjects();

	float speed;
	float scale;
	glm::vec3 offset;
};