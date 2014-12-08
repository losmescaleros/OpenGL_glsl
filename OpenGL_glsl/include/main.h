#ifndef _MAIN_H
#define _MAIN_H
#include <OpenGL_Shaders.h>

#define ENEMY_SHIPS_ROWS 1
#define ENEMY_SHIPS_COLUMNS 1

typedef struct
{
	float height;
	float width;
} 
HitBox2d;

// A vertex contains a 3D position and RGBA color
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
};

struct Material
{
	glm::vec4 diffuse;
	glm::vec4 ambient;
	glm::vec4 specular;
	glm::vec4 emissive;
	float shininess;
};

struct Mesh
{
	GLuint vao;
	unsigned int indexCount;
	unsigned int indexOffset;
	Material * material;
	int numFaces;
};

struct Light
{
	glm::vec4 position;
	glm::vec3 intensity;
};

#endif