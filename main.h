#pragma once

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			  
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800		 
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)

#include "debugproc.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#define SCREEN_WIDTH	(1280)				 
#define SCREEN_HEIGHT	(720)				 
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	 
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	 

#define	NUM_VERTEX		(4)					 

enum
{
	MODE_TITLE = 0,							 
	MODE_TUTORIAL,							 
	MODE_GAME,								 
	MODE_RESULT,							 
	MODE_END
};

long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(int mode);
int GetMode(void);

void SetLoadGame(BOOL flg);
void SetSlowMotion(int frame);
void SetStunFrame(void);

DWORD GetFrameCount(void);