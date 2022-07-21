#pragma once

#include "main.h"
#include "renderer.h"

enum
{
	FADE_NONE = 0,		 
	FADE_IN,			 
	FADE_OUT,			 
	FADE_MAX
};

struct FADE
{
	D3DXVECTOR3		pos;		 
	float			w, h;		 
	int				texNo;		 

	int				flag;		   
	int				nextMode;	 
	D3DXCOLOR		color;		 
};

HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(int fade, int modeNext);
int GetFade(void);
