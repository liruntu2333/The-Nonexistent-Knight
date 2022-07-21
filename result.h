#pragma once

#include "main.h"
#include "renderer.h"

struct RESULT
{
	D3DXVECTOR3		pos;		 
	float			w, h;		 
	int				texNo;		 
};

HRESULT InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);

void SetResult(int score);
