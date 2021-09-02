//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX					(1)		// プレイヤーのMax人数
#define GRAVITATIONAL_CONST			(1)
#define FALL_LIMIT					(30)

enum Orientation
{
	RIGHT,
	DOWN,
	LEFT,
	UP,
};

enum State
{
	RUN,
	STAND,
	STAND_ELEV,

	DASH,
	STUN,
	ATTACK,

	FALL,
	JUMP,
	BIG_JUMP,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct EFFECT;
struct ELEVATOR;

struct PLAYER
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	BOOL			use;			// true:使っている  false:未使用
	float			w, h;			// 幅と高さ
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	int				texNo;			// テクスチャ番号

	int				state;			// current movement
	int				orient;			// 0left 1right
	int				atkOrient;		// 0right 1down 2left 3up
	BOOL			atkDetect;		// 0 not yet 1 had
	int				vertSpd;		// vertical movement per frame
	int				horzSpd;		// horizontal movement per frame, only used when stunned
	int				actCount;		// used for dash / attack / stun count down
	EFFECT*			atk;			// initiated effect
	ELEVATOR*		elev;			// taken elevator
	int				godCount;		// god mode count down

	int				stamina;		// dash / attack need enough stamina to trigger
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void HitPlayer(PLAYER* player, int damge, int orient);

