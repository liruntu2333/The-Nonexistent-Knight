//=============================================================================
//
// BG画面処理 [bg.h]
// Author : 
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
	D3DXVECTOR3		pos;		// ポリゴンの座標
	float			w, h;		// 幅と高さ
	int				texNo;		// 使用しているテクスチャ番号

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