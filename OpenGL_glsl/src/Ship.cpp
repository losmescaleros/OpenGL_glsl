#include <Ship.h>
#include <Alien.h>

const float Ship::LEFT_BOUNDARY = -7.0f;
const float Ship::RIGHT_BOUNDARY = 7.0f;
const float Ship::MOVEMENT_SPEED = 2.0f;

Ship::Ship()
	: m_Alive(true)
{

}

void Ship::Translate(const glm::vec3& delta, bool local /* = true */)
{	
	if (local)
	{
		m_Position += m_Rotation * delta;
	}
	else
	{
		m_Position += delta;
	}

	if (m_Position.x < LEFT_BOUNDARY)
	{
		m_Position.x = LEFT_BOUNDARY;
	}

	if (m_Position.x > RIGHT_BOUNDARY)
	{
		m_Position.x = RIGHT_BOUNDARY;
	}

	m_ViewDirty = true;
}

void Ship::CollidedWith(ModelInstance* otherModel)
{
	Alien* alien = dynamic_cast<Alien*>(otherModel);
	if (alien != 0)
	{
		m_Game->m_GameOver = true;
	}
}