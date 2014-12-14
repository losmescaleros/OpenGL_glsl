#ifndef _BARRIER_H
#define _BARRIER_H
#include <ModelInstance.h>
#include <Game.h>

class Barrier : public ModelInstance{
public:
	Barrier();

	void CollidedWith(ModelInstance* otherModel);

	int m_ShotsTaken;

	static const int MAX_SHOTS;

	Game* m_Game;

protected:
	
};
#endif