//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include <list>
#include "CGameApp.h"

extern CGameApp g_App;

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer* pBackBuffer, int x) : rotateDirection(DIRECTION::DIR_FORWARD), playerLives(10)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	if (x == 1) {
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		m_pSprite->setBackBuffer(pBackBuffer);
	}
	else if (x == 2) {
		//m_pSprite = new Sprite("data/planeimgandmaskk.bmp", RGB(0xff, 0x00, 0xff));
		m_pSprite = new Sprite("data/enemymask.bmp", RGB(0xff, 0x00, 0xff));
		//m_pSprite = new Sprite("data/enemy.bmp", "data/enemymask.bmp");
		m_pSprite->setBackBuffer(pBackBuffer);
	}
	else
	{
		//m_pSprite = new Sprite("data/star.bmp", "data/starmask.bmp");
		m_pSprite = new Sprite("data/starmask.bmp", RGB(0xff, 0x00, 0xff));
		m_pSprite->setBackBuffer(pBackBuffer);
	}
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 15);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
}

void CPlayer::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{
	if(!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}
int CPlayer::getHeight()
{
	return m_pSprite->height();
}

void CPlayer::Move(ULONG ulDirection)
{
	if( ulDirection & CPlayer::DIR_LEFT )
		m_pSprite->mVelocity.x -= 3.1;
	if (m_pSprite->mPosition.x < m_pSprite->width() / 2)
		m_pSprite->mVelocity.x = 0;

	if( ulDirection & CPlayer::DIR_RIGHT )
		m_pSprite->mVelocity.x += 3.1;
	if (m_pSprite->mPosition.x > 785 - m_pSprite->width() / 2)
	{
		m_pSprite->mVelocity.x = 0;
		m_pSprite->mPosition.x = 785 - m_pSprite->width() / 2;
	}

	if( ulDirection & CPlayer::DIR_FORWARD )
		m_pSprite->mVelocity.y -= 3.1;
	if (m_pSprite->mPosition.y < m_pSprite->height() / 2)
		m_pSprite->mVelocity.y = 0;

	if( ulDirection & CPlayer::DIR_BACKWARD )
        m_pSprite->mVelocity.y += 3.1;
	if (m_pSprite->mPosition.y > 560 - m_pSprite->height() / 2)
	{
		m_pSprite->mVelocity.y = 0;
		m_pSprite->mPosition.y = 560 - m_pSprite->height() / 2;
	}
}

void CPlayer::MoveEnemies() {
	this->m_pSprite->mPosition.x += this->add;

	if (this->m_pSprite->mPosition.x- m_pSprite->width()/2 <= 0 || this->m_pSprite->mPosition.x+m_pSprite->width() / 2 >= 790) {
		this->add *= -1;
	}
	

}

void CPlayer::MoveStar()
{
	this->m_pSprite->mPosition.x += this->add2;
	this->m_pSprite->mPosition.y += this->add1;

	if (this->m_pSprite->mPosition.x - m_pSprite->width() / 2 <= 0 || this->m_pSprite->mPosition.x + m_pSprite->width() / 2 >= 780) {
		this->add2 *= -1;
	}
	//if (this->m_pSprite->mPosition.y - m_pSprite->height() / 2 <= 0 || this->m_pSprite->mPosition.y + m_pSprite->height() / 2 >= 560) {
		//this->add1 *= -1;
	//}
}


Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void CPlayer::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

void CPlayer::SetPosition(Vec2 currentPosition) 
{
	m_pSprite->mPosition = currentPosition;
}


bool CPlayer::Collision(CPlayer* p1, CPlayer* p2)
{
	RECT r;
	r.left = p1->m_pSprite->mPosition.x - p1->m_pSprite->width() / 2;
	r.right = p1->m_pSprite->mPosition.x + p1->m_pSprite->width() / 2;
	r.top = p1->m_pSprite->mPosition.y - p1->m_pSprite->height() / 2;
	r.bottom = p1->m_pSprite->mPosition.y + p1->m_pSprite->height() / 2;

	RECT r2;
	r2.left = p2->m_pSprite->mPosition.x - p2->m_pSprite->width() / 2;
	r2.right = p2->m_pSprite->mPosition.x + p2->m_pSprite->width() / 2;
	r2.top = p2->m_pSprite->mPosition.y - p2->m_pSprite->height() / 2;
	r2.bottom = p2->m_pSprite->mPosition.y + p2->m_pSprite->height() / 2;


	if (r.right > r2.left && r.left < r2.right && r.bottom>r2.top && r.top < r2.bottom) 
	{
		return true;
	}
	if (r.left > r2.right && r.right < r2.left && r.bottom>r2.top && r.top < r2.bottom) 
	{
		return true;
	}

	return false;

}


void CPlayer::RotateLeft()
{
	auto position = m_pSprite->mPosition;
	auto velocity = m_pSprite->mVelocity;

	switch (rotateDirection)
	{
	case CPlayer::DIR_FORWARD:
		m_pSprite = new Sprite("data/PlaneImgAndMaskLeft.bmp", RGB(0xff, 0x00, 0xff));

		rotateDirection = CPlayer::DIR_LEFT;
		break;
	case CPlayer::DIR_BACKWARD:
		m_pSprite = new Sprite("data/PlaneImgAndMaskRight.bmp", RGB(0xff, 0x00, 0xff));
		rotateDirection = CPlayer::DIR_RIGHT;
		break;
	case CPlayer::DIR_LEFT:
		rotateDirection = CPlayer::DIR_BACKWARD;
		m_pSprite = new Sprite("data/planeimgandmaskk.bmp", RGB(0xff, 0x00, 0xff));
		break;
	case CPlayer::DIR_RIGHT:
		m_pSprite = new Sprite("data/planeimgandmask.bmp", RGB(0xff, 0x00, 0xff));
		rotateDirection = CPlayer::DIR_FORWARD;
		break;
	}
	
	m_pSprite->mPosition = position;
	m_pSprite->mVelocity = velocity;
	m_pSprite->setBackBuffer(g_App.m_pBBuffer);
}

int CPlayer::GetLives()
{
	return this->playerLives;
}

void CPlayer::DecreaseLives()
{
	--this->playerLives;
}

void CPlayer::SetLives(int currentLive) {
	this->playerLives = currentLive;
}


void CPlayer::SetScore(int score) {
	this->playerScore = score;
}


int CPlayer::GetScore() {
	return this->playerScore;
}

void CPlayer::IncreaseScore(int score)
{
	this->playerScore += score;
}

void CPlayer::IncreaseLives(int score1)
{
	++this->playerLives;
}




