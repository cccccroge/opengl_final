#pragma once

#include "GLM/vec3.hpp"
#include "GLM/vec2.hpp"

// TODO: how to bind with layout together?
class VertexPNT
{
public:
	VertexPNT(const glm::vec3 &pos, const glm::vec3 &normal, 
		const glm::vec2 &texCoord);

private:
    glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_coordinate;
};

class VertexP2T
{
public:
	VertexP2T(const glm::vec2 &pos2, const glm::vec2 &texCoord);
	
private:
	glm::vec2 position2d;
	glm::vec2 texture_coordinate;
};

class VertexP
{
public:
	VertexP(const glm::vec3 &pos);

private:
	glm::vec3 position;
};

