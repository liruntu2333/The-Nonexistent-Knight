#pragma once

#include "main.h"

#define	USE_KEYBOARD				 
#define	USE_MOUSE					 
#define	USE_PAD						 

#define BUTTON_UP		0x00000001l	 
#define BUTTON_DOWN		0x00000002l	 
#define BUTTON_LEFT		0x00000004l	 
#define BUTTON_RIGHT	0x00000008l	 

#define BUTTON_X		0x00000010l	 
#define BUTTON_A		0x00000020l	 
#define BUTTON_B		0x00000040l	 
#define BUTTON_Y		0x00000080l	 
#define BUTTON_L		0x00000100l	 
#define BUTTON_R		0x00000200l	 
#define BUTTON_L2		0x00000400l	 
#define BUTTON_R2		0x00000800l	 
#define BUTTON_SELECT	0x00001000l	 
#define BUTTON_START	0x00002000l	 

#define GAMEPADMAX		4			 

enum {	 
	rgbButtons_X,
	rgbButtons_A,
	rgbButtons_B,
	rgbButtons_Y,
	rgbButtons_L,
	rgbButtons_R,
	rgbButtons_L2,
	rgbButtons_R2,
	rgbButtons_SELECT,
	rgbButtons_START,
};

HRESULT InitInput(HINSTANCE hInst, HWND hWnd);
void UninitInput(void);
void UpdateInput(void);

bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRepeat(int nKey);
bool GetKeyboardRelease(int nKey);

BOOL IsMouseLeftPressed(void);       
BOOL IsMouseLeftTriggered(void);     
BOOL IsMouseRightPressed(void);      
BOOL IsMouseRightTriggered(void);    
BOOL IsMouseCenterPressed(void);     
BOOL IsMouseCenterTriggered(void);   
long GetMouseX(void);                
long GetMouseY(void);                
long GetMouseZ(void);                

BOOL IsButtonPressed(int padNo, DWORD button);
BOOL IsButtonTriggered(int padNo, DWORD button);
