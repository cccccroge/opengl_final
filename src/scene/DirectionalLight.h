#pragma once

#include "Light.h"
#include "GLM/vec3.hpp"

class DirectionalLight : public Light
{
public:
	DirectionalLight(const glm::vec3 position, 
		const glm::vec3 color, const float intensity);

	void bind(ShaderProgram& program, int index);

};