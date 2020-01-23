#pragma once

#include "Light.h"
#include "GLM/vec3.hpp"

class DirectionalLight : public Light
{
public:
	DirectionalLight(const glm::vec3 color, const float intensity,
		const float sphere_d,
		const float front_spectrum_factor);

	void bind(ShaderProgram& program, int index);

	void setTranslation(glm::vec3 trans);
	void calLightSpaceMat();

private:
	float sphere_d;
	float front_spectrum_factor;
};