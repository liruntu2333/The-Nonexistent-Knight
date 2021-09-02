//=============================================================================
//
// �v���C���[���� [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PLAYER_MAX					(1)		// �v���C���[��Max�l��
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
// �\���̒�`
//*****************************************************************************
struct EFFECT;
struct ELEVATOR;

struct PLAYER
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	BOOL			use;			// true:�g���Ă���  false:���g�p
	float			w, h;			// ���ƍ���
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�

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
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void HitPlayer(PLAYER* player, int damge, int orient);

