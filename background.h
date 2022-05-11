//
//	background.h
//	author : LI ZIZHEN liruntu2333@gmail.com
//
#pragma once

#include "main.h"

struct Background
{
	D3DXVECTOR3		pos;
	float			w, h;
	int				texNo;

	BOOL			shake;		// screen shake occurs by get hit, etc
};

//*****************************************************************************
// FUNCTION DECLARATIONS
//*****************************************************************************
HRESULT InitBackground(void);
void UninitBackground(void);
void UpdateBackground(void);
void DrawBackground(void);

