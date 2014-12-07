#include <OpenGL_Shaders.h>
#include <Ship.h>

Ship::Ship()
	: vertices(0),
	indices(0),
	isAlive(true),
	isRegistered(false),
	modelMatrix(1)
{

}

Ship::Ship(std::vector<Vertex> vert , std::vector<GLuint> ind)
	:vertices(vert),
	indices(ind), 
	isAlive(true),
	isRegistered(false),
	modelMatrix(1)
{

}

std::vector<Vertex> Ship::GetVertices()
{
	return vertices;
}

std::vector<GLuint> Ship::GetIndices()
{
	return indices;
}

glm::mat4 Ship::GetModelMatrix()
{
	return modelMatrix;
}

void Ship::SetModelMatrix(const glm::mat4& mat)
{
	modelMatrix = mat;
}