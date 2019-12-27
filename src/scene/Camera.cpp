#include "Camera.h"
#include "../init.h"

#include "GLM/ext/matrix_transform.hpp"
#include "GLM/ext/matrix_clip_space.hpp"
#include "GLM/gtx/projection.hpp"
#include "../global.h"
#include <iostream>
#include <math.h>


Camera::Camera() : SceneObject(glm::vec3(0.0f, 0.0f, 0.0f))
{
    
}


Camera::Camera(PROJECTION_TYPE type, std::vector<float> boundary, 
    glm::vec3 position, float _yaw, float _pitch, float fov) :
        proj_type(type), yaw(_yaw), pitch(_pitch),
        fov(glm::radians(fov)), zNear(boundary[0]), zFar(boundary[1]), 
        SceneObject()
{
    if (proj_type == PROJECTION_TYPE::ORTHOGONAL) {
        left = boundary[2];
        right = boundary[3];
        bottom = boundary[4];
        top = boundary[5];
    }

    // translate to position
    translate(position);
}


void Camera::zoom(int direction)
{
    // move camera toward its face direction
	glm::vec3 dir = (float)direction * getDirection('f');
	glm::vec3 mov = SCALE_SENSITIVITY * dir;
	translate(mov);
}


void Camera::pan(int xRight, int yDown)
{
    // move camera toward the vectors orthogonal to its face direction
    glm::vec3 mov = -PAN_SENSITIVITY * 
        ((float)xRight * getDirection('r') + 
		 (float)yDown * -getDirection('u'));
    translate(mov);
}


void Camera::move(const char dir)
{
    glm::vec3 mov;

    if (dir == 'r' || dir == 'l') {
        glm::vec3 right = getDirection('r');
        mov = (dir == 'r') ? FIRST_PERSON_MOVE_SPEED * right
            : -FIRST_PERSON_MOVE_SPEED * right;
    }
    else if (dir == 'f' || dir == 'b') {
        glm::vec3 front = getDirection('f');
        mov = (dir == 'f') ? FIRST_PERSON_MOVE_SPEED * front
            : -FIRST_PERSON_MOVE_SPEED * front;
    }
    else if (dir == 'u' || dir == 'd') {
        glm::vec3 world_up = UP_VECTOR;
        mov = (dir == 'u') ? FIRST_PERSON_MOVE_SPEED * world_up
            : -FIRST_PERSON_MOVE_SPEED * world_up;
    }

    translate(mov);
}



void Camera::lookAround(int xRight, int yDown)
{
    // rotate camera at its postion
    float dx_right = FIRST_PERSON_ROTATE_SPEED * (float)xRight;
    float dy_down = FIRST_PERSON_ROTATE_SPEED * (float)yDown;
    
	yaw += dx_right;

	pitch -= dy_down;
	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;
}


glm::mat4 Camera::getViewMat()
{
	glm::vec3 cam_pos = getPos();
	glm::vec3 look_pos = cam_pos + getDirection('f');

    return glm::lookAt(cam_pos, look_pos, UP_VECTOR);
}


glm::mat4 Camera::getProjMat()
{
    if (proj_type == PROJECTION_TYPE::PERSPECTIVE) {
        float ratio = global::renderWidth / global::renderHeight;
        return glm::perspective(fov, ratio, zNear, zFar);
    }
    else if (proj_type == PROJECTION_TYPE::ORTHOGONAL) {
        return glm::ortho(left, right, bottom, top, zNear, zFar);
    }
    else {
        std::cout << "Can't generate correct projection matrix" << std::endl;
        return glm::mat4(1.0f);
    }
}


void Camera::setPos(glm::vec3 pos)
{
	glm::vec3 offset = pos - getPos();
	translate(offset);
}


glm::vec3 Camera::getDirection(const char dir)
{
	glm::vec3 direction;

	if (dir == 'f') {
		glm::vec3 front(
			cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
			sin(glm::radians(pitch)),
			cos(glm::radians(pitch)) * sin(glm::radians(yaw))
		);
		direction = glm::normalize(front);
	}
	else if (dir == 'r') {
		direction = glm::normalize(glm::cross(getDirection('f'), UP_VECTOR));
	}
	else if (dir == 'u') {
		direction = glm::normalize(
			glm::cross(getDirection('r'), getDirection('f')));
	}

	return direction;
}


glm::vec3 Camera::getPos()
{
    return glm::vec3(getModelMat()[3]);
}

