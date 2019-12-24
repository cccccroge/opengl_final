#pragma once

#include "GLM/mat4x4.hpp"
#include "GLEW/glew.h"
#include <vector>


class SceneObject
{
public:
    SceneObject(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f)); 
	glm::mat4 getModelMat();

	void translate(float x, float y, float z);
    void translate(glm::vec3 vec);
	void rotate(float deg, std::vector<float> axis);
    void rotate(float deg, glm::vec3 axis);
    void rotate(glm::mat4 rot);
    void rotatePivot(float deg, glm::vec3 axis, glm::vec3 pivot);
	void scale(float x, float y, float z);
    void resetTransformation();

    void setParent(SceneObject &object);
    void clearParent();

    inline glm::mat4 getOriginal() { return original; }
    inline glm::mat4 getTranslation() { return translation; }
    inline glm::mat4 getRotation() { return rotation; }
    inline glm::mat4 getScaling() { return scaling; }
    inline SceneObject* getParent() { return parent; }

protected:
    glm::mat4 original; // TODO: maybe split to world and local space?

protected:
    glm::mat4 translation;
    glm::mat4 rotation;
    glm::mat4 scaling;

    /* if has parent, use these three so that we can keep original space
       to adjust its child transformation, to prevent chain adjust which
       is not correct. */
    glm::mat4 translation_p;
    glm::mat4 rotation_p;
    glm::mat4 scaling_p;

    SceneObject *parent;
};