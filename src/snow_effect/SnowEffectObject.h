#pragma once

#include "../init.h"
#include "GLM/glm.hpp"
#include "ModelSnow.h"
#include "Accumulation.h"


class SnowEffectObject
{
public:
	SnowEffectObject(const glm::vec3 offset, const float gravity, 
		const glm::vec2 windspeed, const float density);

	void initialize();
	void draw();


	Accumulation *acm;
	glm::mat4 ModelMatrices[NUMSNOWFLAKES];
	vector<glm::vec3> positions;
	vector<glm::vec3> masses;

	ShaderSnow *instancedShader;
	ShaderSnow *colourShader;
	GLuint colourShaderColourID;
	GLuint colourShaderViewID;
	GLuint colourShaderProjectionID;
	GLuint colourShaderModelID;
	ShaderSnow* heightShader;
	GLuint heightShaderModelID;
	GLuint heightShaderViewID;
	GLuint heightShaderProjectionID;

	GLuint ubo;
	GLuint Matrices_binding;
	GLuint vao;

	ModelSnow* snowflake;

private:
	glm::vec3 offset;
	float gravity;
	glm::vec2 windspeed;
	float density;

public:
	inline void setGravity(const float val) { gravity = val; }	// 0.1~10
	inline void setWindSpeed(const glm::vec2 speed) { windspeed = speed; }	// -10~10
	inline void setDensity(const float val) { density = val; }	// 1~5 or more?
};