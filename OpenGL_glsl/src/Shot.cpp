#include <Shot.h>

const float Shot::UPPER_BOUNDARY = 10;
const float Shot::MOVEMENT_SPEED = 200.0f;

Shot::Shot()
	: m_HitSomething(false)
{
	m_DeltaY = MOVEMENT_SPEED;
}

void Shot::Reinitialize(const glm::vec3& pos)
{
	m_Position = pos;
	m_HitSomething = false;
}

void Shot::MoveStep(float deltaTime)
{
	m_Position.y += (m_DeltaY * deltaTime);
	m_ViewDirty = true;
}