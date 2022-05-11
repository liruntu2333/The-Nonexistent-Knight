//=============================================================================
//
// [bg.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct BG
{
	D3DXVECTOR3		pos;		
	float			w, h;		
	int				texNo;		

	BOOL			shake;		// screen shake occurs by get hit, etc
};

enum TERRAIN
{
	AIR,
	EARTH,
	GROUND,
};

#define BLK_LGTH				(10)			// Block Length

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMap(void);
void UninitMap(void);
void UpdateMap(void);
void DrawMap(void);

BG* GetMap(void);
int GetTerrain(float x, float y);
void SetTerrain(int X, int Y, int terrain);
D3DXVECTOR3 ReloacteObj(float x, float y, float w, float h);
void SetShake(int frame);