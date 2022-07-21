#pragma once

#include "main.h"
#include "renderer.h"

#define ENEMY_MAX		(2)		 
#define	ENEMY_HEALTH_MAX					(20)

struct	LINEAR_INTERPOLATION		 
{
	D3DXVECTOR3		pos;			 
	D3DXVECTOR3		rot;			 
	D3DXVECTOR3		scl;			 
	float			time;			 
};

struct EFFECT;

struct ENEMY
{
	D3DXVECTOR3		pos;			 
	D3DXVECTOR3		rot;			 
	D3DXVECTOR3		scl;			 
	BOOL			use;			   
	float			w, h;			 
	float			countAnim;		 
	int				patternAnim;	 
	int				texNo;			 
	int				state;			  
	int				orient;			  
	int				atkOrient;		    
	BOOL			atkDetect;		     
	BOOL			slashed;		     
	int				vertSpd;		    
	int				horzSpd;		        
	int				actCount;		         
	EFFECT* effect;			  
	EFFECT* rddot;			  

	int				stamina;		        
	int				health;			       

};

HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);

void HitEnemy(ENEMY* enemy, int damge, int orient);
