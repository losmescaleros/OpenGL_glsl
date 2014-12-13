#ifndef _SHOT_H
#define _SHOT_H
#include <ModelInstance.h>
#include <Game.h>

class Shot : public ModelInstance
{
public:
	Shot();

	void Reinitialize(const glm::vec3& pos);

	void MoveStep(float deltaTime);

	

	void CollidedWith(ModelInstance* otherModel);

	Game * m_Game;

	bool m_HitSomething;

	static const float UPPER_BOUNDARY;
	static const float MOVEMENT_SPEED;

protected:

};

#endif