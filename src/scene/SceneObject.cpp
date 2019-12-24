#include "SceneObject.h"
#include "GLM/ext/matrix_transform.hpp"
#include "GLM/geometric.hpp"


SceneObject::SceneObject(glm::vec3 pos/* = glm::vec3(0.0f, 0.0f, 0.0f)*/) : 
	original(glm::mat4(1.0f)), 
    translation(glm::mat4(1.0f)),
	rotation(glm::mat4(1.0f)),
    scaling(glm::mat4(1.0f)),
	translation_p(glm::mat4(1.0f)),
	rotation_p(glm::mat4(1.0f)),
    scaling_p(glm::mat4(1.0f)),
	parent(NULL)
{
	// initial postion provided
	if (!glm::all(glm::equal(pos, glm::vec3(0.0f, 0.0f, 0.0f)))) {
		original = glm::translate(original, pos);
	}
}


glm::mat4 SceneObject::getModelMat()
{
	if (parent != NULL) {
		return parent->getModelMat() * (translation_p * original) * scaling_p * rotation_p;
	}
	else {
		return (translation * original) * scaling * rotation;
	}
}


void SceneObject::translate(float x, float y, float z)
{
	translate(glm::vec3(x, y, z));
}


void SceneObject::translate(glm::vec3 vec)
{
	if (parent != NULL) {
		translation_p = glm::translate(translation_p, vec);
	}
	else {
		translation = glm::translate(translation, vec);
	}
}


void SceneObject::rotate(float deg, std::vector<float> axis)
{
	rotate(deg, glm::vec3(axis[0], axis[1], axis[2]));
}


void SceneObject::rotate(float deg, glm::vec3 axis)
{
	glm::vec3 axis_normalized = glm::normalize(axis);

	if (parent != NULL) {
		rotation_p = glm::rotate(rotation_p, glm::radians(deg), axis_normalized);
	}
	else {
		rotation = glm::rotate(rotation, glm::radians(deg), axis_normalized);
	}
}


void SceneObject::rotate(glm::mat4 rot)
{
	if (parent != NULL) {
		rotation_p = rot * rotation_p;
	}
	else {
		rotation = rot * rotation;
	}
}


void SceneObject::rotatePivot(float deg, glm::vec3 axis, glm::vec3 pivot)
{
	glm::vec3 axis_normalized = glm::normalize(axis);

	glm::vec3 offset = glm::vec3(getModelMat()[3]) - pivot;
	glm::mat4 rotTran = glm::translate(glm::mat4(1.0f), offset);
	rotTran = glm::rotate(rotTran, glm::radians(deg), axis_normalized);
	rotTran = glm::translate(rotTran, -offset);

	glm::mat4 tran = glm::translate(glm::mat4(1.0f), glm::vec3(rotTran[3]));
	glm::mat4 rot = glm::translate(rotTran, glm::vec3(-rotTran[3]));

	if (parent != NULL) {
		translation_p = tran * translation_p;
		rotation_p = rot * rotation_p;
	} 
	else {
		translation = tran * translation;
		rotation = rot * rotation;
	}
}


void SceneObject::scale(float x, float y, float z)
{
	if (parent != NULL) {
		scaling_p = glm::scale(scaling_p, glm::vec3(x, y, z));
	}
	else {
		scaling = glm::scale(scaling, glm::vec3(x, y, z));
	}
}


void SceneObject::resetTransformation()
{
	translation = glm::mat4(1.0f);
	rotation = glm::mat4(1.0f);
	scaling = glm::mat4(1.0f);
}


void SceneObject::setParent(SceneObject &object)
{
	parent = &object;

	// use new set of transformation
	translation_p = glm::inverse(parent->getTranslation()) * translation;
	rotation_p = glm::inverse(parent->getRotation()) * rotation;
	scaling_p = glm::inverse(parent->getScaling()) * scaling;
}


void SceneObject::clearParent()
{
/* 	translation = translation_p;
	rotation = rotation_p;
	scaling = scaling_p; */

	translation_p = glm::mat4(1.0f);
	rotation_p = glm::mat4(1.0f);
	scaling_p = glm::mat4(1.0f);

	parent = NULL;
}