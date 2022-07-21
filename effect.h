#pragma once

#include "main.h"
#include "renderer.h"

struct EFFECT
{
	D3DXVECTOR3		pos;			 
	D3DXVECTOR3		rot;			 
	BOOL			use;			   
	float			w, h;			 
	float			countAnim;		 
	int				patternAnim;	 
	int				texNo;			 

	float			vertSpd;
	float			horzSpd;
	int				bounce;			  

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

HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

EFFECT* SetEffect(float X, float Y, int type, int orient);
