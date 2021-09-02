//
// @file				elevator.h
// @brief				An elevator object can moves around and 
//						modifies terrain.
// @copyright			2021 LI ZIZHEN liruntu2333@gmail.com
// @lisense				GPL
//
#pragma once

//
// MACROS
//
#define ELEV_MAX				(1)

//
// Structrue Declarations
//
struct ELEVATOR
{
	D3DXVECTOR3		pos;		// ポリゴンの座標
	float			w, h;		// 幅と高さ
	int				texNo;		// 使用しているテクスチャ番号

	int				vertSpd;
};

//
// Function Prototype Declarations
//
HRESULT InitElev(void);
void UninitElev(void);
void UpdateElev(void);
void DrawElev(void);

ELEVATOR* GetElev(void);