#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "title.h"
#include "background.h"
#include "terrain.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "score.h"
#include "result.h"
#include "sound.h"
#include "fade.h"
#include "tutorial.h"
#include "file.h"
#include "effect.h"
#include "elevator.h"
#include "ui.h"

#define CLASS_NAME			"AppClass"				 
#define WINDOW_NAME			"Nonexistent Knight"		 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
bool CheckGameover();

long g_MouseX = 0;
long g_MouseY = 0;

#ifdef _DEBUG
int		g_CountFPS;							 
char	g_DebugStr[2048] = WINDOW_NAME;		 

#endif

int g_Mode = MODE_TITLE;						  
BOOL g_LoadGame = FALSE;					 
unsigned int g_FPS = 60;
int g_SMcount = 0;							      

DWORD g_dwFrameCount = 0;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};
	HWND		hWnd;
	MSG			msg;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		 
		CW_USEDEFAULT,																		 
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									 
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	 
		NULL,
		NULL,
		hInstance,
		NULL);

	BOOL mode = TRUE;

	int id = MessageBox(nullptr, "Display in window mode?", "Display mode", MB_YESNOCANCEL | MB_ICONQUESTION);

	switch (id)
	{
	case IDYES:
	{
		mode = TRUE;
		break;
	}
	case IDNO:
	{
		mode = FALSE;
		break;
	}
	case IDCANCEL:
	default:
	{
		return -1;
		break;
	}
	}

	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	timeBeginPeriod(1);	 
	dwExecLastTime = dwFPSLastTime = timeGetTime();	 
	dwCurrentTime = dwFrameCount = 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (1)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ 
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					 

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	 
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				 
				dwFrameCount = 0;							 
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / g_FPS))	 
			{
				dwExecLastTime = dwCurrentTime;	 

#ifdef _DEBUG	 
				wsprintf(g_DebugStr, WINDOW_NAME);
#endif

				Update();			 
				Draw();				 

#ifdef _DEBUG	 
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;		 
				g_dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				 

	UnregisterClass(CLASS_NAME, wcex.hInstance);

	Uninit();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:					 
			DestroyWindow(hWnd);		 
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitCamera();

	InitInput(hInstance, hWnd);

	InitSound(hWnd);

	InitFade();

	SetMode(g_Mode);	 

	return S_OK;
}

void Uninit(void)
{
	SetMode(MODE_END);	 

	UninitFade();

	UninitSound();

	UninitInput();

	UninitCamera();

	UninitRenderer();
}

void Update(void)
{
	UpdateCamera();

	UpdateInput();

	switch (g_Mode)
	{
	case MODE_TITLE:		 
		UpdateTitle();
		break;

	case MODE_TUTORIAL:		 
		UpdateTutorial();
		break;

	case MODE_GAME:			 
		if (g_SMcount != 0)
		{
			g_SMcount--;
			if (g_SMcount == 0)
			{
				g_FPS = 60;
			}
		}
		UpdateBackground();
		UpdateTerrain();
		UpdateElev();
		UpdatePlayer();
		UpdateEffect();
		UpdateBullet();
		UpdateEnemy();
		UpdateUI();
		CheckGameover();
		break;

	case MODE_RESULT:
		UpdateResult();
	}

	UpdateFade();

#ifdef _DEBUG
#endif
}

void Draw(void)
{
	Clear();

	SetCamera();

	SetDepthEnable(false);

	switch (g_Mode)
	{
	case MODE_TITLE:		 
		DrawTitle();
		break;

	case MODE_TUTORIAL:		 
		DrawTutorial();
		break;

	case MODE_GAME:			 
		DrawBackground();
		DrawTerrain();
		DrawElev();

		DrawEnemy();
		DrawBullet();
		DrawEffect();
		DrawPlayer();
		DrawUI();

		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		 
		}
		break;

	case MODE_RESULT:
		DrawResult();
		break;
	}

	DrawFade();

#ifdef _DEBUG
	DrawDebugProc();
#endif

	Present();
}

void SetMode(int mode)
{
	UninitBackground();
	UninitTitle();
	UninitElev();
	UninitTerrain();
	UninitEffect();
	UninitPlayer();
	UninitEnemy();
	UninitBullet();
	UninitScore();
	UninitResult();
	UninitTutorial();
	UninitUI();
	StopSound();

	g_Mode = mode;	 

	switch (g_Mode)
	{
	case MODE_TITLE:
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_DrunkStar);
		break;

	case MODE_TUTORIAL:
		InitTutorial();
		PlaySound(SOUND_LABEL_BGM_DrunkStar);
		break;

	case MODE_GAME:
		InitBackground();
		InitTerrain();
		InitElev();
		InitEffect();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitScore();
		InitUI();
		PlaySound(SOUND_LABEL_BGM_EoS);
		break;

	case MODE_RESULT:
		InitResult();
		PlaySound(SOUND_LABEL_BGM_DrunkStar);
		break;

	case MODE_END:
		StopSound();
		break;
	}
}

bool CheckGameover()
{
	ENEMY* enemy = GetEnemy();

	bool flag = true;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if ((enemy + i)->use)
		{
			flag = false;
			break;
		}
	}
	if (flag)
	{
		SetFade(FADE_OUT, MODE_RESULT);
		return true;
	}
	return false;
}

void SetSlowMotion(int frame)
{
	g_FPS = 30;
	g_SMcount = frame;
}

void SetStunFrame(void)
{
	g_FPS = 3;
	g_SMcount = 1;
}

DWORD GetFrameCount(void)
{
	return g_dwFrameCount;
}

void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}

int GetMode(void)
{
	return g_Mode;
}

long GetMousePosX(void)
{
	return g_MouseX;
}

long GetMousePosY(void)
{
	return g_MouseY;
}

#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif