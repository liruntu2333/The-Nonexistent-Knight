//=============================================================================
//
// Main���� [main.cpp]
// Author : GP11A132 35 lizizhen
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "title.h"
#include "map.h"
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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME			"AppClass"				// �E�C���h�E�̃N���X��
#define WINDOW_NAME			"Nonexistent Knight"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
bool CheckGameover();


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int g_Mode = MODE_TITLE;						 // �N�����̉�ʂ�ݒ�
BOOL g_LoadGame = FALSE;					// NewGame
unsigned int g_FPS = 60;
int g_SMcount = 0;							// use to count down slow motion

DWORD g_dwFrameCount = 0;



//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	//UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
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
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// �E�B���h�E�̍����W
						CW_USEDEFAULT,																		// �E�B���h�E�̏���W
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// �E�B���h�E����
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
						NULL,
						NULL,
						hInstance,
						NULL);

	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Display in window mode?", "Display mode", MB_YESNOCANCEL | MB_ICONQUESTION);

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

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;
	
	// �E�C���h�E�̕\��(Init()�̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// �V�X�e���������擾

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / g_FPS))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;		// �����񐔂̃J�E���g�����Z
				g_dwFrameCount++;
			}
		}
	}
	
	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
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

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_�����O�����̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	// �J���������̏�����
	InitCamera();

	InitInput(hInstance, hWnd);

	InitSound(hWnd);

	InitFade();

	// �ŏ��̃��[�h���Z�b�g���āA���̒���Init����
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �Q�[���I�ȕ��͂��̒���Uninit����
	SetMode(MODE_END);	// ������SetMode�̂܂܂ŁI

	UninitFade();

	UninitSound();

	UninitInput();

	// �J�����̏I������
	UninitCamera();

	// �����_�����O�̏I������
	UninitRenderer();

}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// �J�����̍X�V����
	UpdateCamera();

	UpdateInput();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V
		UpdateTitle();
		break;

	case MODE_TUTORIAL:		// �^�C�g����ʂ̍X�V
		UpdateTutorial();
		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V
		if (g_SMcount != 0)
		{
			g_SMcount--;
			if (g_SMcount == 0)
			{
				g_FPS = 60;
			}
		}
		UpdateMap();
		UpdateElev();
		UpdatePlayer();
		UpdateEffect();
		UpdateBullet();
		UpdateEnemy();
		CheckGameover();
		break;

	case MODE_RESULT:
		UpdateResult();
	}

	UpdateFade();

#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("SMcount: %d \n",g_SMcount);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// �J�������Z�b�g
	SetCamera();

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);
	
	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��
		DrawTitle();
		break;

	case MODE_TUTORIAL:		// �^�C�g����ʂ̕`��
		DrawTutorial();
		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��
		DrawMap();
		DrawElev();

		DrawEnemy();
		DrawBullet();
		DrawScore();
		DrawEffect();
		DrawPlayer();


		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		// ���[�h��������t���O��Clear����
		}
		break;

	case MODE_RESULT:
		DrawResult();
		break;
	}

	DrawFade();

#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();

}


//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ
	
	// �^�C�g���̏I������
	UninitTitle();
	UninitElev();
	UninitMap();
	UninitEffect();
	UninitPlayer();
	UninitEnemy();
	UninitBullet();
	UninitScore();
	UninitResult();
	UninitTutorial();
	StopSound();

	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_DrunkStar);
		break;

	case MODE_TUTORIAL:
		InitTutorial();
		PlaySound(SOUND_LABEL_BGM_DrunkStar);
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����
		InitMap();
		InitElev();
		InitEffect();
		InitPlayer();
		InitEnemy();
		InitBullet();
		InitScore();
		PlaySound(SOUND_LABEL_BGM_EoS);
		break;

	case MODE_RESULT:
		InitResult();
		PlaySound(SOUND_LABEL_BGM_DrunkStar);
		break;
		
		// �Q�[���I�����̏���
	case MODE_END:
		StopSound();
		break;
	}
}

bool CheckGameover()
{

	ENEMY* enemy = GetEnemy();

	bool flag = false;
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
		SetResult(GetScore());
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

DWORD GetFrameCount(void)
{
	return g_dwFrameCount;
}

//=============================================================================
// �j���[�Q�[�������[�h�Q�[�������Z�b�g����
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}


//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// �}�E�X���W�̎擾
//=============================================================================
long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char *GetDebugStr(void)
{
	return g_DebugStr;
}
#endif


