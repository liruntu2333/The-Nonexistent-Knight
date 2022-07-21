#pragma once

#include "main.h"
#include "renderer.h"

struct Terrain
{
	D3DXVECTOR3		pos;		
	float			w, h;		
	int				texNo;		

	BOOL			shake;		       
};

enum BLOCK_TYPE
{
	AIR,
	EARTH,
	GROUND,
};

#define BLK_LGTH				(10)			  

HRESULT InitTerrain(void);
void UninitTerrain(void);
void UpdateTerrain(void);
void DrawTerrain(void);

Terrain* GetTerrain(void);
int GetTerrain(float x, float y);
void SetTerrain(int X, int Y, int terrain);
D3DXVECTOR3 ReloacteObj(float x, float y, float w, float h);
void SetShake(int frame);