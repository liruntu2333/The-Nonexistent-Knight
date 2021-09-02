//=============================================================================
//
// タイトル画面処理 [particle.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct EFFECT
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	BOOL			use;			// true:使っている  false:未使用
	float			w, h;			// 幅と高さ
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	int				texNo;			// テクスチャ番号

	float			vertSpd;
	float			horzSpd;
	int				bounce;			// bounced time

	int				type;
};

enum EFFECT_TYPE
{
	PLAYER_BLADE,
	PLAYER_REFLECT,
	PLAYER_HIT,
	COIN,
	EFFECT_TYPE_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

EFFECT* SetEffect(float X, float Y, int type, int orient);
