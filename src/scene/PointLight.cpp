#include "PointLight.h"
#include <string>
#define GLM_FORCE_SWIZZLE
#include "GLM/glm.hpp"



PointLight::PointLight(const glm::vec3 position,
	const glm::vec3 color, const float intensity,
	const glm::vec3 attenuation, std::vector<float> spectrum) :
	Light(position, color, intensity, spectrum),
	att_constant(attenuation[0]),
	att_linear(attenuation[1]),
	att_quadratic(attenuation[2])
{

}

void PointLight::bind(ShaderProgram& program, int index)
{
	program.bind();

	std::string i = std::to_string(index);
	std::string pre = "pointLights[" + i + "].";

	std::string str = pre + "position";
	program.setUniformVec3(str.c_str(), getPosition());
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