//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_MAX		(3)		// エネミーのMax人数


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct	LINEAR_INTERPOLATION		// 線形補間データの構造体
{
	D3DXVECTOR3		pos;			// 座標
	D3DXVECTOR3		rot;			// 回転量
	D3DXVECTOR3		scl;			// 拡大率
	float			time;			// 移動フレーム数
};


struct ENEMY
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	D3DXVECTOR3		scl;			// ポリゴンの拡大率
	BOOL			use;			// true:使っている  false:未使用
	float			w, h;			// 幅と高さ
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	int				texNo;			// テクスチャ番号
	D3DXVECTOR3		move;			// 移動速度

	float			time;			// 線形補間用
	int				moveTblNo;		// 行動データのテーブル番号
	int				tblMax;			// そのテーブルのデータ数
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

void DestructEnemy(ENEMY* ep);

