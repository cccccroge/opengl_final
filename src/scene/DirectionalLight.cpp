#include "DirectionalLight.h"
#include <string>
#include "GLM/ext/matrix_transform.hpp"
#include "GLM/ext/matrix_clip_space.hpp"
#include "../init.h"


DirectionalLight::DirectionalLight(const glm::vec3 position,
	const glm::vec3 color, const float intensity,
	std::vector<float> spectrum) :
	Light(position, color, intensity, spectrum)
{
	left = spectrum[2];
	right = spectrum[3];
	bottom = spectrum[4];
	top = spectrum[5];
	calLightSpaceMat();
}

void DirectionalLight::bind(ShaderProgram& program, int index)
{
	program.bind();

	std::string i = std::to_string(index);
	std::string pre = "direcitonalLights[" + i + "]." ;
	
	std::string str = pre + "direction";
	program.setUniformVec3(str.c_str(), direction);
	/*str = pre + "position";
	program.setUniformVec3(str.c_str(), getPosition());*/
	str = pre + "color";
	program.setUniformVec3(str.c_str(), color);
	str = pre + "intensity";
	program.setUniform1f(str.c_str(), intensity);
}

void DirectionalLight::setTranslation(glm::vec3 trans)
{
	if (parent != NULL) {
		translation_p = glm::translate(glm::mat4(1.0), trans);
	}
	else {
		translation = glm::translate(glm::mat4(1.0), trans);
	}
	calLightSpaceMat();
}

void DirectionalLight::calLightSpaceMat()
{
	glm::vec3 pos = getPosition();
	glm::vec3 dir = getDirection();
	glm::vec3 lookto = pos + dir;

	glm::mat4 lightV = glm::lookAt(pos, lookto, UP_VECTOR);
	glm::mat4 lightP = glm::ortho(left, right, bottom, top, near_plane, far_plane);
	glm::mat4 lightSpaceMatrix = lightP * lightV;

	if (lightSpaceMat.size() == 0)
		lightSpaceMat.push_back(lightSpaceMatrix);
	else
		lightSpaceMat[0] = lightSpaceMatrix;
}