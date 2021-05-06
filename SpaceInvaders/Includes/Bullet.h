//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _BULLET_H_
#define _BULLET_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class Bullet
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION
	{
		DIR_FORWARD = 1,
		DIR_BACKWARD = 2,
		DIR_LEFT = 4,
		DIR_RIGHT = 8,
	};

	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	Bullet(const BackBuffer* pBackBuffer);
	virtual ~Bullet();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update(float dt);
	void					Draw();
	void					Move(ULONG ulDirection);
	void					MoveDown(ULONG ulDirection);
	Vec2& Position();
	Vec2& Velocity();
	bool                    Hit = false;
	void					Explode();
	bool					AdvanceExplosion();
	Sprite*                  m_pSprite;

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	//Sprite*                  m_pSprite;
	ESpeedStates			m_eSpeedState;
	float					m_fTimer;

	bool					m_bExplosion;
	AnimatedSprite* m_pExplosionSprite;
	int						m_iExplosionFrame;
};

#endif // _BULLET_H_
