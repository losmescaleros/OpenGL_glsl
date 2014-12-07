#ifndef _BARRIER_H
#define _BARRIER_H
#include <main.h>

class Barrier{
public:
	Barrier();
	Barrier(float width, float height, HitBox2d& hitBox, glm::vec3 pos, int hitPoints);
	glm::vec3 GetPosition();
	void SetPosition(const glm::vec3& pos);

protected:
	glm::vec3 position;
	HitBox2d hitBox;
	int hitPoints;
	bool isAlive;
	float width;
	float height;
};
#endif