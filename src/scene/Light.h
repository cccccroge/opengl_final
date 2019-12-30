#pragma once

#include "SceneObject.h"
#include "GLM/vec3.hpp"
#include "Camera.h"
#include "../render/ShaderProgram.h"

class Light : public SceneObject
{

public:
	Light(const glm::vec3 position, const glm::vec3 direction, const glm::vec3 color, 
		const float intensity);

	void bind(ShaderProgram& program, int index) {}

protected:
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;

	Camera* camera;	// TODO: for light casters
};