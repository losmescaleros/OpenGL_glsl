#ifndef _OPEN_GL_SHADERS_H
#define _OPEN_GL_SHADERS_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

// Include glew before freeglut
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

// Only use radians with glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif