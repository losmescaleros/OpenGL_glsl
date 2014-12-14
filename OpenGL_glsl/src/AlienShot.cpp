#include <AlienShot.h>
#include <Ship.h>
#include <Barrier.h>

const float AlienShot::LOWER_BOUNDARY = -5.0f;
const float AlienShot::MOVEMENT_SPEED = 20.0f;

AlienShot::AlienShot()
	:m_HitSomething(false)
{
	m_DeltaY = MOVEMENT_SPEED;
}

void AlienShot::Reinitialize(const glm::vec3& pos)
{
	m_Position = pos;
	m_HitSomething = false;
}

void AlienShot::MoveStep(float deltaTime)
{
	m_Position.y -= m_DeltaY * deltaTime;
	m_ViewDirty = true;
}

void AlienShot::CollidedWith(ModelInstance* otherModel)
{
	if (!m_HitSomething)
	{
		
		Ship* ship = dynamic_cast<Ship*>(otherModel);
		Barrier* barrier = dynamic_cast<Barrier*>(otherModel);
		if (barrier != 0)
		{
			m_HitSomething = true;
			m_Game->m_Instances.erase(m_Name);
			barrier->m_ShotsTaken++;
			if (barrier->m_ShotsTaken == Barrier::MAX_SHOTS)
			{
				m_Game->m_Instances.erase(otherModel->m_Name);
			}
		}
		else if (ship != 0)
		{
			m_Game->m_Instances.erase(otherModel->m_Name);
			m_Game->m_Instances.erase(m_Name);
			m_Game->m_GameOver = true;
			m_HitSomething = true;
		}

		
		
	}
}