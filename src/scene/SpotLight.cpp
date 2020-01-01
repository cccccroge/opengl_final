#include "SpotLight.h"
#include "GLM/trigonometric.hpp"
#include "GLM/ext/matrix_transform.hpp"
#include "GLM/ext/matrix_clip_space.hpp"
#include "../init.h"


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
	calLightSpaceMat();
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

void SpotLight::setTranslation(glm::vec3 trans)
{
	if (parent != NULL) {
		translation_p = glm::translate(glm::mat4(1.0), trans);
	}
	else {
		translation = glm::translate(glm::mat4(1.0), trans);
	}
	calLightSpaceMat();
}

void SpotLight::calLightSpaceMat()
{
	glm::vec3 pos = getPosition();
	glm::vec3 dir = getDirection();
	glm::vec3 lookto = pos + dir;
	glm::mat4 lightV = glm::lookAt(pos, lookto, UP_VECTOR);

	float ratio = DEPTH_MAP_RESOLUTION / DEPTH_MAP_RESOLUTION;
	glm::mat4 lightP = glm::perspective(glm::radians(outerCutoff * 2.0f), 
		ratio, near_plane, far_plane);
	glm::mat4 lightSpaceMatrix = lightP * lightV;

	if (lightSpaceMat.size() == 0)
		lightSpaceMat.push_back(lightSpaceMatrix);
	else
		lightSpaceMat[0] = lightSpaceMatrix;
}