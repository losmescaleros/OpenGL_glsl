#ifndef _ALIEN_H
#define _ALIEN_H
#include <main.h>
#include <ModelInstance.h>

class Alien : public ModelInstance
{
public:
	Alien();

	void Translate(const glm::vec3& delta, bool local = true);

	static const float LEFT_BOUNDARY;
	static const float RIGHT_BOUNDARY;
	static const float MOVEMENT_SPEED;
	static const float LOWER_BOUNDARY;

protected:
	bool m_Alive;
};

#endif