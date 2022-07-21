#pragma once

#include "main.h"
#include "renderer.h"

#define BULLET_MAX		(10)		 

struct BULLET
{
	D3DXVECTOR3		pos;			 
	D3DXVECTOR3		rot;			 
	BOOL			use;			   
	float			w, h;			 
	float			countAnim;		 
	int				patternAnim;	 
	int				texNo;			 
	D3DXVECTOR3		move;			 

	int				time;
};

HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET* GetBullet(void);

void DestructBullet(BULLET* bp);
