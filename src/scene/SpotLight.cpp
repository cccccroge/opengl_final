#include "SpotLight.h"
#include "GLM/trigonometric.hpp"

SpotLight::SpotLight(const glm::vec3 position,
	const glm::vec3 color, const float intensity,
	const glm::vec2 cutoff,
	const glm::vec3 attenuation, std::vector<float> spectrum) :
	Light(position, color, intensity, spectrum),
	cutoff(cutoff[0]), outerCutoff(cutoff[1]),
	att_constant(attenuation[0]),
	att_linear(attenuation[1]),
	att_quadratic(attenuation[2])
{

}

void SpotLight::bind(ShaderProgram& program, int index)
{
	program.bind();

	std::string i = std::to_string(index);
	std::string pre = "spotLights[" + i + "].";

	std::string str = pre + "position";
	program.setUniformVec3(str.c_str(), getPosition());
	str = pre + "direction";
	program.setUniformVec3(str.c_str(), direction);
	str = pre + "cutoff";
	program.setUniform1f(str.c_str(), cos(glm::radians(cutoff)));
	str = pre + "outerCutoff";
	program.setUniform1f(str.c_str(), cos(glm::radians(outerCutoff)));

	str = pre + "color";
	program.setUniformVec3(str.c_str(), color);
	str = pre + "intensity";
	program.setUniform1f(str.c_str(), intensity);

	str = pre + "att_constant";
	program.setUniform1f(str.c_str(), att_constant);
	str = pre + "att_linear";
	program.setUniform1f(str.c_str(), att_linear);
	str = pre + "att_quadratic";
	program.setUniform1f(str.c_str(), att_quadratic);
}