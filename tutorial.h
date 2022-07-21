#pragma once

#include "main.h"
#include "renderer.h"

struct TUTORIAL
{
	D3DXVECTOR3		pos;		 
	float			w, h;		 
	int				texNo;		 
	float offset;
};

HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);
