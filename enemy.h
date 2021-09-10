//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_MAX		(2)		// エネミーのMax人数
#define	ENEMY_HEALTH_MAX					(20)


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

struct EFFECT;

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
	//D3DXVECTOR3		move;			// 移動速度

	int				state;			// current movement
	int				orient;			// 0left 1right
	int				atkOrient;		// 0right 1down 2left 3up
	BOOL			atkDetect;		// 0 not yet 1 had
	BOOL			slashed;		// 0 not yet 1 had
	int				vertSpd;		// vertical movement per frame
	int				horzSpd;		// horizontal movement per frame, only used when stunned
	int				actCount;		// used for dash / attack / STUN count down
	EFFECT*         effect;			// initiated effect
	EFFECT*			rddot;			// red dot


	int				stamina;		// dash / attack need enough stamina to trigger
	int				health;			// when health reduces to 0, enemy die.

	//float			time;			// 線形補間用
	//int				moveTblNo;		// 行動データのテーブル番号
	//int				tblMax;			// そのテーブルのデータ数
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

void HitEnemy(ENEMY* enemy, int damge, int orient);


