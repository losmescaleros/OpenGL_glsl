#include <OpenGL_Shaders.h>
#include <Barrier.h>

Barrier::Barrier()
	:position(0),
	isAlive(true),
	hitPoints(0)
{

}

Barrier::Barrier(float width, float height, HitBox2d& hitBox, glm::vec3 pos, int hitPoints)
	:position(pos),
	hitBox(hitBox),
	width(width),
	height(height),
	hitPoints(hitPoints)
{

}

glm::vec3 Barrier::GetPosition()
{
	return position;
}

void Barrier::SetPosition(const glm::vec3& pos)
{
	position = pos;
}