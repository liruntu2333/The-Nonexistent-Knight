//=============================================================================
//
// スコア処理 [score.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_MAX			(99999)		// スコアの最大値
#define SCORE_DIGIT			(5)			// 桁数

#define	SCORE_DX			(500.0f)	// 表示位置
#define	SCORE_DY			(20.0f)		//
#define	SCORE_DW			(16.0f)		//
#define	SCORE_DH			(32.0f)		//

struct SCORE
{
	D3DXVECTOR3		pos;		// ポリゴンの座標
	float			w, h;		// 幅と高さ
	int				texNo;		// 使用しているテクスチャ番号

	int				score;		// 得点
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

int GetScore(void);
void AddScore(int add);
void SetScore(int score);
