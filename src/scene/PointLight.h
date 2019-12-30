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

	inline void setAttConstant(const float val) { att_constant = val; }
	inline void setAttLinear(const float val) { att_linear = val; }
	inline void setAttQuadratic(const float val) { att_quadratic = val; }
	inline float getAttConstant() { return att_constant; }
	inline float getAttLinear() { return att_linear; }
	inline float getAttQuadratic() { return att_quadratic; }

private:
	float att_constant;
	float att_linear;
	float att_quadratic;
};