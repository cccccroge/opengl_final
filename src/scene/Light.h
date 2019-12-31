#pragma once

#include "SceneObject.h"
#include "GLM/vec3.hpp"
#include "../render/ShaderProgram.h"
#include <vector>

class Light : public SceneObject
{

public:
	Light(const glm::vec3 position, const glm::vec3 color, 
		const float intensity, std::vector<float> spectrum);

	virtual void bind(ShaderProgram& program, int index) = 0;

	void setYaw(float deg);
	void setPitch(float deg);
	void setNear(float near_plane);
	void setFar(float far_plane);
	void calDirection();

	virtual void setTranslation(glm::vec3 trans) = 0;
	virtual void calLightSpaceMat() = 0;

	inline glm::vec3 getDirection() { return direction; }
	inline float getYaw() { return yaw; }
	inline float getPitch() { return pitch; }
	inline float getIntensity() { return intensity; }
	inline glm::vec3 getColor() { return color; }
	inline void setIntensity(float val) { intensity = val; }
	inline void setColor(glm::vec3 col) { color = col; }
	inline std::vector<glm::mat4>& getLightSpaceMat() { return lightSpaceMat; }
	inline float getFarPlane() {  return far_plane; }

protected:
	glm::vec3 direction;	// get from yaw & pitch
	glm::vec3 color;
	float intensity;

	float yaw;		// 0~360 in degrees 
	float pitch;	// 0~180 in degrees

	// used in shadow program, convert world space to light space
	// we will have 6 of them if we want to render point light's shadow
	// otherwise there should be one matrix (for directional light)
	std::vector<glm::mat4> lightSpaceMat;

	float near_plane;
	float far_plane;

	float left;
	float right;
	float bottom;
	float top;
};