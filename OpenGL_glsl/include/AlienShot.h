#ifndef _ALIEN_SHOT_H
#define _ALIEN_SHOT_H
#include <ModelInstance.h>
#include <Game.h>

class AlienShot : public ModelInstance
{
public:
	AlienShot();

	void Reinitialize(const glm::vec3& pos);

	void MoveStep(float deltaTime);

	void CollidedWith(ModelInstance* otherModel);

	Game* m_Game;

	bool m_HitSomething;

	static const float LOWER_BOUNDARY;
	static const float MOVEMENT_SPEED;
};

#endif