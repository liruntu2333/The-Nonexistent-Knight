//=============================================================================
//
// BG��ʏ��� [bg.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct BG
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�

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
// �v���g�^�C�v�錾
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