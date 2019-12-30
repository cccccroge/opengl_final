#include "Light.h"

Light::Light(const glm::vec3 position, const glm::vec3 direction, const glm::vec3 color,
	const float intensity) : SceneObject(position), camera(NULL),
	direction(direction), color(color), intensity(intensity)
{

}