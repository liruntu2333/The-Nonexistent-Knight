#pragma once

#include "main.h"
#include "renderer.h"

#define SCORE_MAX			(99999)		 
#define SCORE_DIGIT			(5)			 

#define	SCORE_DX			(500.0f)	 
#define	SCORE_DY			(20.0f)		
#define	SCORE_DW			(16.0f)		
#define	SCORE_DH			(32.0f)		

struct SCORE
{
	D3DXVECTOR3		pos;		 
	float			w, h;		 
	int				texNo;		 

	int				score;		 
};

HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

int GetScore(void);
void AddScore(int add);
void SetScore(int score);
