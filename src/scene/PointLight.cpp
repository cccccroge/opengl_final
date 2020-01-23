#include "PointLight.h"
#include <string>
#include "GLM/glm.hpp"
#include "GLM/ext/matrix_transform.hpp"
#include "GLM/ext/matrix_clip_space.hpp"
#include "GLM/gtx/projection.hpp"
#include "../init.h"



PointLight::PointLight(const glm::vec3 position,
	const glm::vec3 color, const float intensity,
	const glm::vec3 attenuation, std::vector<float> spectrum) :
	Light(position, color, intensity, spectrum),
	att_constant(attenuation[0]),
	att_linear(attenuation[1]),
	att_quadratic(attenuation[2])
{
	calLightSpaceMat();
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

void PointLight::setTranslation(glm::vec3 trans)
{
	if (parent != NULL) {
		translation_p = glm::translate(glm::mat4(1.0), trans);
	}
	else {
		translation = glm::translate(glm::mat4(1.0), trans);
	}
	calLightSpaceMat();
}

void PointLight::calLightSpaceMat()
{
	float aspect = (float)DEPTH_MAP_RESOLUTION / (float)DEPTH_MAP_RESOLUTION;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), // has to be 90 degree to fit 6 faces
		aspect, near_plane, far_plane);
	glm::vec3 pos = getPosition();

	std::vector<glm::mat4> curMat;
	curMat.push_back(shadowProj *
		glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	curMat.push_back(shadowProj *
		glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	curMat.push_back(shadowProj *
		glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	curMat.push_back(shadowProj *
		glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	curMat.push_back(shadowProj *
		glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	curMat.push_back(shadowProj *
		glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	if (lightSpaceMat.size() == 6) {
		for (int i = 0; i < 6; ++i) {
			lightSpaceMat[i] = curMat[i];
		}
	}
	else {
		lightSpaceMat = curMat;
	}
}