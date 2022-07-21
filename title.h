#pragma once

#include "main.h"
#include "renderer.h"

struct TITLE
{
	D3DXVECTOR3		pos;		 
	float			w, h;		 
	int				texNo;		 
	float offset;
};

HRESULT InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);
