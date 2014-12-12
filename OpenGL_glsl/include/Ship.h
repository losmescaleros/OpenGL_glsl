#ifndef _SHIP_H
#define _SHIP_H
#include <main.h>
#include <ModelInstance.h>
/**
* Enemy ship class
*/

class Ship : public ModelInstance
{
public:
	Ship();

	void Translate(const glm::vec3& delta, bool local = true);

	static const float LEFT_BOUNDARY;
	static const float RIGHT_BOUNDARY;
	static const float MOVEMENT_SPEED;

protected:
	bool m_Alive;

	
};

#endif