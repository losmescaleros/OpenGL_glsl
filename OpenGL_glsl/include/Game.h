#ifndef _GAME_H
#define _GAME_H
#include <map>
#include <ModelInstance.h>
class Game
{
public:
	Game();

	float m_AlienDirection;

	float m_AlienMovementSpeed;

	bool m_GameOver;

	bool m_AlienHitWall;

	bool m_GameLogicRequired;

	std::map<std::string, ModelInstance*> m_Instances;
};

#endif