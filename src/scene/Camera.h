#pragma once

#include "SceneObject.h"
#include "GLM/mat4x4.hpp"
#include "GLM/trigonometric.hpp"
#include <vector>


enum class PROJECTION_TYPE
{
    PERSPECTIVE, ORTHOGONAL
};


class Camera : public SceneObject
{
public:
    Camera();
	Camera(PROJECTION_TYPE type, std::vector<float> boundary,
		glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		float yaw = 0, float pitch = 0,
        float fov = glm::radians(60.0f));

    void zoom(int direction);
    void pan(int xRight, int yDown);
    void move(const char dir);
    void lookAround(int xRight, int yDown);

    glm::mat4 getViewMat();
    glm::mat4 getProjMat();

	void setPos(glm::vec3 pos);
	void setFov(float deg) { fov = glm::radians(deg); }
	void setPitch(float deg) { pitch = deg; }
	void setYaw(float deg) { yaw = deg; }
	inline float getPitch() { return pitch; }
	inline float getYaw() { return yaw; }

	glm::vec3 getDirection(const char dir);
	glm::vec3 getPos();

	void loadControlPts(const char *path, const int samples);
	inline bool hasCurvePts() { return curve_pts.size() != 0; }
	inline std::vector<glm::vec3>& getCurvePts() { return curve_pts; }
    
private:
    PROJECTION_TYPE proj_type;

    // used in PERSPECTIVE
    float fov;  // rads

    // means near/far in PERSPECTIVE, or zNear/zFar in ORTHOGONAL
    float zNear;
    float zFar;

    // used in ORTHOGONAL
    float left;
    float right;
    float bottom;
    float top;

	// use to calculate front position
	float yaw;
	float pitch;

	// curve control points
	std::vector<glm::vec3> control_pts;
	std::vector<glm::vec3> curve_pts;
};