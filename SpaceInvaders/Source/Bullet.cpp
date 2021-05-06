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
#include "Bullet.h"

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
Bullet::Bullet(const BackBuffer* pBackBuffer)
{
	m_pSprite = new Sprite("data/upBullet.bmp", "data/upBulletMask.bmp");
	//m_pSprite = new Sprite("data/upBullet.bmp", RGB(0xff, 0x00, 0xff));
	m_pSprite->setBackBuffer(pBackBuffer);
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite = new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 4);
	m_pExplosionSprite->setBackBuffer(pBackBuffer);
	m_bExplosion = false;
	m_iExplosionFrame = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
Bullet::~Bullet()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
}

void Bullet::Update(float dt)
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

}

void Bullet::Draw()
{
	if (!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();
}

void Bullet::Move(ULONG ulDirection)
{
	int height = 0, width = 0;
	RECT rect;
	if (GetClientRect(GetActiveWindow(), &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 >= 0)
		m_pSprite->mPosition.y -= 0.75;
	else
		this->Hit = true;
}

void Bullet::MoveDown(ULONG ulDirection)
{
	int height = 0, width = 0;
	RECT rect;
	if (GetClientRect(GetActiveWindow(), &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	if (m_pSprite->mPosition.y - m_pSprite->height() / 2 >= 0)
		m_pSprite->mPosition.y += 2;
	else
		this->Hit = true;
}


Vec2& Bullet::Position()
{
	return m_pSprite->mPosition;
}

Vec2& Bullet::Velocity()
{
	return m_pSprite->mVelocity;
}

void Bullet::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool Bullet::AdvanceExplosion()
{
	if (m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if (m_iExplosionFrame == m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0, 0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}