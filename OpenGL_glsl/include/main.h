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

#endif