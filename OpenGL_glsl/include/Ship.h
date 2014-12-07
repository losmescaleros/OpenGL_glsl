#ifndef _SHIP_H
#define _SHIP_H
#include <main.h>
/**
* Enemy ship class
*/

class Ship
{
public:
	Ship();
	Ship(std::vector<Vertex> vertices, std::vector<GLuint> indices);

	std::vector<Vertex> GetVertices();

	std::vector<GLuint> GetIndices();

	glm::mat4 GetModelMatrix();

	void SetModelMatrix(const glm::mat4& mat);

	GLuint vaoId;
	GLuint vertexBufferId;
	GLuint indexBufferId;
	bool isRegistered;
protected:

	std::vector<Vertex> vertices;

	std::vector<GLuint> indices;

	bool isAlive;	

	glm::mat4 modelMatrix;
	
};

#endif