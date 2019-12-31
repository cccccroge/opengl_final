#pragma once

#include "Light.h"
#include "GLM/vec3.hpp"

class SpotLight : public Light
{
public:
	SpotLight(const glm::vec3 position,
		const glm::vec3 color, const float intensity,
		const glm::vec2 cutoff,
		const glm::vec3 attenuation, std::vector<float> spectrum);

	void bind(ShaderProgram& program, int index);

	inline void setAttConstant(const float val) { att_constant = val; }
	inline void setAttLinear(const float val) { att_linear = val; }
	inline void setAttQuadratic(const float val) { att_quadratic = val; }
	inline void setCutoff(const float deg) { cutoff = deg; }
	inline void setOuterCutoff(const float deg) { outerCutoff = deg; }
	inline float getAttConstant() { return att_constant; }
	inline float getAttLinear() { return att_linear; }
	inline float getAttQuadratic() { return att_quadratic; }
	inline float getCutoff() { return cutoff; }
	inline float getOuterCutoff() { return outerCutoff; }

private:
	float att_constant;
	float att_linear;
	float att_quadratic;

	float cutoff;		// in degrees
	float outerCutoff;	// in degrees
};