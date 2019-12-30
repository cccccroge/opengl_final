#pragma once

#include "SceneObject.h"
#include "GLM/vec3.hpp"
#include "Camera.h"
#include "../render/ShaderProgram.h"

class Light : public SceneObject
{

public:
	Light(const glm::vec3 position, const glm::vec3 color, 
		const float intensity);

	virtual void bind(ShaderProgram& program, int index) = 0;

	void setYaw(float deg);
	void setPitch(float deg);

	void calDirection();

	inline glm::vec3 getDirection() { return direction; }
	inline float getYaw() { return yaw; }
	inline float getPitch() { return pitch; }
	inline float getIntensity() { return intensity; }
	inline glm::vec3 getColor() { return color; }
	inline void setIntensity(float val) { intensity = val; }
	inline void setColor(glm::vec3 col) { color = col; }

protected:
	glm::vec3 direction;	// get from yaw & pitch
	glm::vec3 color;
	float intensity;

	float yaw;		// 0~360 in degrees 
	float pitch;	// 0~180 in degrees

	Camera* camera;	// TODO: for light casters
};