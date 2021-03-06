#include "Vertex.h"


VertexPNT::VertexPNT(const glm::vec3 &pos, const glm::vec3 &normal, 
	const glm::vec2 &texCoord) : position(pos), normal(normal),
    texture_coordinate(texCoord)
{
    
}

VertexPNTS::VertexPNTS(const glm::vec3 &pos, const glm::vec3 &normal,
	const glm::vec2 &texCoord, const glm::vec3 &tanSpace) : 
	position(pos), normal(normal),
	texture_coordinate(texCoord), tangent_space(tanSpace)
{

}

VertexP2T::VertexP2T(const glm::vec2 &pos2, const glm::vec2 &texCoord) :
    position2d(pos2), texture_coordinate(texCoord)
{

}

VertexP::VertexP(const glm::vec3 &pos) : position(pos)
{

}