#ifndef _ALIEN_H
#define _ALIEN_H
#include <ModelInstance.h>
#include <Game.h>
class Alien : public ModelInstance
{
public:
	Alien();

	void Translate(const glm::vec3& delta, bool local = true);

	void DoLogic();

	void MoveStep(float deltaTime);

	bool m_HitWall;

	static const float LEFT_BOUNDARY;
	static const float RIGHT_BOUNDARY;
	static const float MOVEMENT_SPEED;
	static const float LOWER_BOUNDARY;
	static const float DOWNWARD_STEP;
	static const float STEP_MOVEMENT_SCALAR;

	Game* m_Game;

protected:
	bool m_Alive;
};

#endif