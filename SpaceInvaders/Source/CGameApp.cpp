//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"
#include<fstream>
#include "CPlayer.h"

using namespace std;

extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	Player1         = NULL;
	m_pEnemy        = NULL;
	m_pEnemy2       = NULL;
	m_pEnemy3       = NULL;
	star1           = NULL;
	star2           = NULL;
	star3           = NULL;
	m_LastFrameRate = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= 800;
	USHORT			Height			= 600;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;

			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 250, NULL);

					m_pPlayer->Explode();
					m_pPlayer->DecreaseLives();

				break;
			case 0x51:
				fTimer = SetTimer(m_hWnd, 2, 250, NULL);
		
					Player1->Explode();
					Player1->DecreaseLives();

				break;
			case 'S':
				SaveGame(m_pPlayer, Player1);
				break;
			case 'L':
				LoadGame(m_pPlayer, Player1);
				break;
			case 'O':
				m_pPlayer->RotateLeft();
				break;
			case 'R':
				Player1->RotateLeft();
				break;

			}

			break;


		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
					fTimer = SetTimer(m_hWnd, 1, 100, NULL);
				
				if( !Player1->AdvanceExplosion())
				{
					fTimer = SetTimer(m_hWnd, 1, 100, NULL);
				}

				if (!m_pEnemy->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}

				if (!m_pEnemy2->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}

				if (!m_pEnemy3->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !Player1->AdvanceExplosion())
				{
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !m_pEnemy->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !m_pEnemy2->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !m_pEnemy3->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!Player1->AdvanceExplosion() && !m_pEnemy->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!Player1->AdvanceExplosion() && !m_pEnemy2->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!Player1->AdvanceExplosion() && !m_pEnemy3->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !Player1->AdvanceExplosion() && !m_pEnemy->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !Player1->AdvanceExplosion() && !m_pEnemy2->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}
				if (!m_pPlayer->AdvanceExplosion() && !Player1->AdvanceExplosion() && !m_pEnemy3->AdvanceExplosion()) {
					fTimer = SetTimer(m_hWnd, 1, 50, NULL);
				}


			}

			break;

		case WM_COMMAND:
			break;


		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer,1);
	Player1= new CPlayer(m_pBBuffer,1);
	m_pEnemy = new CPlayer(m_pBBuffer,2);
	m_pEnemy2 = new CPlayer(m_pBBuffer,2);
	m_pEnemy3 = new CPlayer(m_pBBuffer,2);
	star1 = new CPlayer(m_pBBuffer, 3);
	star2 = new CPlayer(m_pBBuffer, 3);
	star3 = new CPlayer(m_pBBuffer, 3);

	m_pBullet = {};

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(100, 400);
	Player1->Position() = Vec2(300, 400);

	m_pEnemy->Position() = Vec2(100, 100);
	m_pEnemy2->Position() = Vec2(150, 150);
	m_pEnemy3->Position() = Vec2(200, 200);

	star1->Position() = Vec2(200, 350);
	star2->Position() = Vec2(250, 450);
	star3->Position() = Vec2(150, 500);
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}

	if (Player1 != NULL)
	{
		delete Player1;
		Player1 = NULL;
	}

	if (m_pEnemy != NULL)
	{
		delete m_pEnemy;
		m_pEnemy = NULL;
	}
	if (m_pEnemy2 != NULL)
	{
		delete m_pEnemy2;
		m_pEnemy2 = NULL;
	}
	if (m_pEnemy3 != NULL)
	{
		delete m_pEnemy3;
		m_pEnemy3 = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s  Lives: % d - % d    Score : % d - % d"), FrameRate, m_pPlayer->GetLives(), Player1->GetLives(), m_pPlayer->GetScore(), Player1->GetScore() );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	if (!m_pPlayer->GetLives() || !Player1->GetLives()) {
		if (m_pPlayer->GetScore() > Player1->GetScore())
		{
			MessageBox(m_hWnd, "First Player Wins", "Game over", MB_OK);
			PostQuitMessage(0);
		}
		else if (m_pPlayer->GetScore() < Player1->GetScore())
		{
			MessageBox(m_hWnd, "Second Player Wins", "Game over", MB_OK);
			PostQuitMessage(0);
		}
		else
		{
			MessageBox(m_hWnd, "Tie", "Game over", MB_OK);
			PostQuitMessage(0);
		}
	}

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput( )
{
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0;
	ULONG		Direction2 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;
	ULONG       LaunchBullet = 0;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys

	if ( pKeyBuffer[ VK_UP	] & 0xF0 ) Direction |= CPlayer::DIR_FORWARD;
	if ( pKeyBuffer[ VK_DOWN  ] & 0xF0 ) Direction |= CPlayer::DIR_BACKWARD;
	if ( pKeyBuffer[ VK_LEFT  ] & 0xF0 ) Direction |= CPlayer::DIR_LEFT;
	if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 ) Direction |= CPlayer::DIR_RIGHT;


	if (pKeyBuffer[0x57] & 0xF0) Direction2 |= CPlayer::DIRECTION::DIR_FORWARD;
	if (pKeyBuffer[0x53] & 0xF0) Direction2 |= CPlayer::DIRECTION::DIR_BACKWARD;
	if (pKeyBuffer[0x41] & 0xF0) Direction2 |= CPlayer::DIRECTION::DIR_LEFT;
	if (pKeyBuffer[0x44] & 0xF0) Direction2 |= CPlayer::DIRECTION::DIR_RIGHT;
	
	// Move the player
	m_pPlayer->Move(Direction);
	Player1->Move(Direction2);
	m_pEnemy->MoveEnemies();
	m_pEnemy2->MoveEnemies();
	m_pEnemy3->MoveEnemies();
	star1->MoveStar();
	star2->MoveStar();
	star3->MoveStar();

	static UINT fTimer;


	if (m_pPlayer->Collision(m_pPlayer, star1))
	{
		fTimer = SetTimer(m_hWnd, 1, 50, NULL);

		m_pPlayer->IncreaseLives(1);
		
		star1->Explode();
		
		star1->AdvanceExplosion();
		fTimer = SetTimer(m_hWnd, 1, 50, NULL);

		star1->Position() = Vec2(rand()%500+100, rand()%500+100);

	}

	if (m_pPlayer->Collision(m_pPlayer, star2))
	{
		fTimer = SetTimer(m_hWnd, 1, 50, NULL);

		m_pPlayer->IncreaseLives(1);
	
		star2->Explode();

		star2->AdvanceExplosion();
		fTimer = SetTimer(m_hWnd, 1, 50, NULL);

		star2->Position() = Vec2(rand()%500+100, rand()%500+100);
		
	}


	if (m_pPlayer->Collision(m_pPlayer, star3))
	{
		fTimer = SetTimer(m_hWnd, 1, 50, NULL);

		m_pPlayer->IncreaseLives(1);

		star3->Explode();

		star3->AdvanceExplosion();
		fTimer = SetTimer(m_hWnd, 1, 50, NULL);

		 star3->Position() = Vec2(rand()%500+100, rand()%500+100);

	}




	if (pKeyBuffer['B'] & 0xF0)
	{
		__int64 m_CurrentTime = timeGetTime();
		if (m_CurrentTime - m_BulletTime >= 300)  
		{
			m_BulletTime = m_CurrentTime;
			m_pBullet.push_back(new Bullet(m_pBBuffer));
			m_pBullet.back()->Position() = Vec2(m_pPlayer->Position().x, m_pPlayer->Position().y - m_pPlayer->getHeight() / 2);
		}
	}

	
	for (auto i = 0; i < m_pBullet.size(); i++)
	{
		if (m_pBullet[i]->Hit)
		{
			delete m_pBullet[i];
			m_pBullet.erase(m_pBullet.begin() + i);
			continue;
		}
		m_pBullet[i]->Move(CPlayer::DIR_FORWARD);
	}
	

	if (pKeyBuffer['V'] & 0xF0)
	{
		__int64 m_CurrentTime = timeGetTime();
		if (m_CurrentTime - m_BulletTime >= 300)
		{
			m_BulletTime = m_CurrentTime;
			m_pBullet.push_back(new Bullet(m_pBBuffer));
			m_pBullet.back()->Position() = Vec2(Player1->Position().x, Player1->Position().y - Player1->getHeight() / 2);
		}
	}


	for (auto i = 0; i < m_pBullet.size(); i++)
	{
		if (m_pBullet[i]->Hit)
		{
			delete m_pBullet[i];
			m_pBullet.erase(m_pBullet.begin() + i);
			continue;
		}
		m_pBullet[i]->Move(CPlayer::DIR_FORWARD);
	}




		__int64 m_CurrentTime = timeGetTime();
		if (m_CurrentTime - m_BulletTime >= rand() + 2000)
		{
			m_BulletTime = m_CurrentTime;
			m_pBulletEnemy.push_back(new Bullet(m_pBBuffer));
			m_pBulletEnemy.back()->Position() = Vec2(m_pEnemy2->Position().x, m_pEnemy2->Position().y - m_pEnemy2->getHeight() / 2);
		}


		if (m_CurrentTime - m_BulletTime >= rand() + 2000)
		{
			m_BulletTime = m_CurrentTime;
			m_pBulletEnemy.push_back(new Bullet(m_pBBuffer));
			m_pBulletEnemy.back()->Position() = Vec2(m_pEnemy3->Position().x, m_pEnemy3->Position().y - m_pEnemy3->getHeight() / 2);
		}




			if (m_CurrentTime - m_BulletTime >= rand()+2000)
			{
				m_BulletTime = m_CurrentTime;
				m_pBulletEnemy.push_back(new Bullet(m_pBBuffer));
				m_pBulletEnemy.back()->Position() = Vec2(m_pEnemy->Position().x, m_pEnemy->Position().y - m_pEnemy->getHeight() / 2);
			}


		for (auto i = 0; i < m_pBulletEnemy.size(); i++)
			
		{
			if (m_pBulletEnemy[i]->Hit)
			{
				delete m_pBulletEnemy[i];
				m_pBulletEnemy.erase(m_pBulletEnemy.begin() + i);
				continue;
			}
			m_pBulletEnemy[i]->MoveDown(CPlayer::DIR_BACKWARD);
		}


	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
	Player1->Update(m_Timer.GetTimeElapsed());
	for (auto i = 0; i < m_pBullet.size(); i++)
		m_pBullet[i]->Update(m_Timer.GetTimeElapsed());
	for (auto i = 0; i < m_pBulletEnemy.size(); i++)
		m_pBulletEnemy[i]->Update(m_Timer.GetTimeElapsed());

	m_pEnemy->Update(m_Timer.GetTimeElapsed());
	m_pEnemy2->Update(m_Timer.GetTimeElapsed());
	m_pEnemy3->Update(m_Timer.GetTimeElapsed());
	star1->Update(m_Timer.GetTimeElapsed());
	star2->Update(m_Timer.GetTimeElapsed());
	star3->Update(m_Timer.GetTimeElapsed());
}

bool Collision1(Bullet* p1, CPlayer* p2);


//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{

	m_pBBuffer->reset();
	DrawBackground();

	m_pPlayer->Draw();
	Player1->Draw();

	for (auto i = 0; i < m_pBullet.size(); i++)
		m_pBullet[i]->Draw();
	for (auto i = 0; i < m_pBulletEnemy.size(); i++)
		m_pBulletEnemy[i]->Draw();

	if (m_pPlayer->GetLives()) m_pPlayer->Draw();
	if (Player1->GetLives()) Player1->Draw();

	m_pEnemy->Draw();
	m_pEnemy2->Draw();
	m_pEnemy3->Draw();

	star1->Draw();
	star2->Draw();
	star3->Draw();

	for (auto it : m_pBullet)
	{
		if (Collision1(it, m_pEnemy))
		{
			
			m_pEnemy->Explode();
			m_pPlayer->IncreaseScore(1);
			
			m_pEnemy->Position() = Vec2(700, 100);
			break;
			
		}
	}
	// Player1 attacks the Enemy2
	for (auto it : m_pBullet)
	{
		if (Collision1(it, m_pEnemy2))
		{
			m_pEnemy2->Explode();
			m_pPlayer->IncreaseScore(1);
		
			 m_pEnemy2->Position() = Vec2(750, 150);

			break;
		}
	}
	// Player1 attacks the Enemy3
	for (auto it : m_pBullet)
	{
		if (Collision1(it, m_pEnemy3))
		{
			m_pEnemy3->Explode();
			m_pPlayer->IncreaseScore(1);
			
			m_pEnemy3->Position() = Vec2(650, 200);
			break;
		}
	}


	for (auto it : m_pBulletEnemy)
	{
		if (Collision1(it, m_pPlayer))
		{

			m_pPlayer->Explode();
			m_pPlayer->DecreaseLives();
			m_pPlayer->Position() = Vec2(rand()%500+100, rand() % 500 + 100);
			break;

		}
	}

	m_pPlayer->AdvanceExplosion();
	m_pEnemy->AdvanceExplosion();
	m_pEnemy2->AdvanceExplosion();
	m_pEnemy3->AdvanceExplosion();

	star1->AdvanceExplosion();
	star2->AdvanceExplosion();
	star3->AdvanceExplosion();

	m_pBBuffer->present();
}



void CGameApp::DrawBackground()
{
	static int currentY = m_imgBackground.Height();

	static size_t lastTime = ::GetTickCount();
	 size_t currentTime = ::GetTickCount();

	if (currentTime - lastTime > 100) 
	{
		lastTime = currentTime;
		currentY -= 10;
		if (currentY < 0)
			currentY = m_imgBackground.Height();
	}

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, currentY);

}



void CGameApp::SaveGame(CPlayer* m_pPlayer, CPlayer* Player1) 
{ 
	ofstream fout("test.out");
	
	fout << m_pPlayer->GetLives() << '\n' << Player1->GetLives() << '\n';
	fout <<  m_pPlayer->Position().x << " " << m_pPlayer->Position().y << '\n';
	fout << Player1->Position().x << " " << Player1->Position().y << '\n';
	fout << m_pPlayer->GetScore() << '\n' << Player1->GetScore() << '\n';

	::MessageBox(m_hWnd, "Game saved", "Save", MB_OK);
}


//-------------------------------------------------------------
// Load previously saved game
//-------------------------------------------------------------
void CGameApp::LoadGame(CPlayer* m_pPlayer, CPlayer* Player1) 
{ 
	::MessageBox(m_hWnd, "Loading game", "Load", MB_OK);
	ifstream fin("test.out");
	int live1, live2;
	int score1, score2;
	Vec2 currentPosition1, currentPosition2;
	fin >> live1 >> live2;
	fin >> currentPosition1.x >> currentPosition1.y;
	fin >> currentPosition2.x >> currentPosition2.y;
	fin >> score1 >> score2;

	m_pPlayer->SetLives(live1);
	Player1->SetLives(live2);

	m_pPlayer->SetScore(score1);
	Player1->SetScore(score2);
	
	m_pPlayer->SetPosition(currentPosition1);
	Player1->SetPosition(currentPosition2);
	::MessageBox(m_hWnd, "Game loaded", "Load", MB_OK);
}

bool Collision1(Bullet* p1, CPlayer* p2)
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
