#pragma once

#include "Light.h"
#include "GLM/vec3.hpp"

class PointLight : public Light
{
public:
	PointLight(const glm::vec3 position,
		const glm::vec3 color, const float intensity, 
		const glm::vec3 attenuation = glm::vec3(1.0, 0.5, 0.5));

	void bind(ShaderProgram& program, int index);

private:
	float att_constant;
	float att_linear;
	float att_quadratic;
};