#ifndef _GAME_H
#define _GAME_H

class Game
{
public:
	Game();

	float m_AlienDirection;

	float m_AlienMovementSpeed;

	bool m_GameOver;

	bool m_AlienHitWall;

	bool m_GameLogicRequired;
};

#endif