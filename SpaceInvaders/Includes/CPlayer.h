//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CPLAYER_H_
#define _CPLAYER_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include "Bullet.h"

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class CPlayer
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION 
	{ 
		DIR_FORWARD	 = 1, 
		DIR_BACKWARD	= 2, 
		DIR_LEFT		= 4, 
		DIR_RIGHT	   = 8, 
	};

	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	int playerLives;
	int playerScore = 0;

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CPlayer(const BackBuffer *pBackBuffer, int x);
	virtual ~CPlayer();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update( float dt );
	void					Draw();
	void					Move(ULONG ulDirection);
	void					MoveEnemies();
	Vec2&					Position();
	Vec2&					Velocity();

	void					Explode();
	bool					AdvanceExplosion();
	int                     getHeight();
	void					SetPosition(Vec2 position);
	void                    RotateLeft();
	DIRECTION               rotateDirection;
	bool                    Collision(CPlayer* p1, CPlayer* p2);
	int                     GetLives();
	void                    DecreaseLives();
	void					SetLives(int lives);
	void					SetScore(int score);
	int						GetScore();
	void					IncreaseScore(int score);
	void                    IncreaseLives(int score1);
	Sprite*                  m_pSprite;
	void					MoveStar();
	


private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	//Sprite*					m_pSprite;
	ESpeedStates			m_eSpeedState;
	float					m_fTimer;
	
	bool					m_bExplosion;
	AnimatedSprite*			m_pExplosionSprite;
	int						m_iExplosionFrame;
	const BackBuffer*       mBackBuffer;
	
private:
	
	float						add = 0.1;
	float                       add_s =0.1;
	float                       add1 = 0.1;
	float                       add2 = 0.1;
};

#endif // _CPLAYER_H_