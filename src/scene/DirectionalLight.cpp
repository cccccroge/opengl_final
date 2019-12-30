#include "DirectionalLight.h"
#include <string>

DirectionalLight::DirectionalLight(const glm::vec3 position,
	const glm::vec3 color, const float intensity) : 
	Light(position, color, intensity)
{

}

void DirectionalLight::bind(ShaderProgram& program, int index)
{
	program.bind();

	std::string i = std::to_string(index);
	std::string pre = "direcitonalLights[" + i + "]." ;
	
	std::string str = pre + "direction";
	program.setUniformVec3(str.c_str(), direction);
	str = pre + "color";
	program.setUniformVec3(str.c_str(), color);
	str = pre + "intensity";
	program.setUniform1f(str.c_str(), intensity);
}