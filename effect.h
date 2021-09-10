//=============================================================================
//
// タイトル画面処理 [particle.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
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
	PLAYER_BLADE2,
	PLAYER_REFLECT,
	PLAYER_HIT,
	PLAYER_HIT2,
	PLAYER_PARRY,
	PLAYER_SLASH,
	RED_DOT,
	PLAYER_HEAL,
	BLOOD_SPLASH,
	DUST_CIRCLE,
	DUST,
	MAGIC_CIRCLE,
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
