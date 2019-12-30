#include "Light.h"
#include "GLM/gtx/rotate_vector.hpp"
#include <iostream>

Light::Light(const glm::vec3 position, const glm::vec3 color,
	const float intensity) : SceneObject(position), camera(NULL),
	direction(glm::vec3(0, -1, 0)), color(color), intensity(intensity), 
	yaw(0), pitch(0)
{

}

void Light::setYaw(float deg)
{
	yaw = deg;
	calDirection();
}

void Light::setPitch(float deg)
{
	pitch = deg;
	calDirection();
}

void Light::calDirection()
{
	glm::vec3 default_dir(0, -1, 0);

	default_dir = glm::rotate(default_dir, glm::radians(pitch), glm::vec3(1, 0, 0));
	default_dir = glm::rotate(default_dir, glm::radians(yaw), glm::vec3(0, 1, 0));

	direction = default_dir;
}