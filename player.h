#pragma once

#include "main.h"
#include "renderer.h"

#define PLAYER_MAX					(1)		 
#define GRAVITATIONAL_CONST			(1)
#define FALL_LIMIT					(30)
#define PLAYER_HEALTH_MAX					(7)
#define PLAYER_STAMINA_MAX					(100)

enum Orientation
{
	RIGHT,
	DOWN,
	LEFT,
	UP,
};

enum State
{
	STAND,
	RUN,

	DASH,
	HEAL,
	STUN,
	DEAD,
	PARRY,
	SLASH,
	ATTACK,

	FALL,
	JUMP,
	BIG_JUMP,
	STATE_MAX
};

struct EFFECT;
struct ELEVATOR;
struct ENEMY;

struct PLAYER
{
	D3DXVECTOR3		pos;			 
	D3DXVECTOR3		rot;			 
	BOOL			use;			   
	float			w, h;			 
	float			countAnim;		 
	int				patternAnim;	 
	int				texNo;			 

	int				state;			  
	int				orient;			    
	int				atkOrient;		    
	BOOL			atkDetect;		     
	BOOL			pryDetect;		  
	int				vertSpd;		    
	int				horzSpd;		        
	int				actCount;		         
	EFFECT* effect;			     
	ELEVATOR* elev;			  
	int				godCount;		    

	int				stamina;		        
	int				health;			       
	int				money;			    
};

HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

void HitPlayer(ENEMY* enmey, PLAYER* player, int damge, int orient);
