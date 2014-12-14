#include <OpenGL_Shaders.h>
#include <Barrier.h>
#include <AlienShot.h>

const int Barrier::MAX_SHOTS = 2;

Barrier::Barrier()
	: m_ShotsTaken(0)
{

}

void Barrier::CollidedWith(ModelInstance* otherModel)
{

}