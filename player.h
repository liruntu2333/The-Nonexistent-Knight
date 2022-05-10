//=============================================================================
//
// �v���C���[���� [player.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
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

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct EFFECT;
struct ELEVATOR;
struct ENEMY;

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
	int				orient;			// 0right 1down 2left 3up
	int				atkOrient;		// 0right 1down 2left 3up
	BOOL			atkDetect;		// 0 not yet 1 had
	BOOL			pryDetect;		// Parry Detect
	int				vertSpd;		// vertical movement per frame
	int				horzSpd;		// horizontal movement per frame, only used when stunned
	int				actCount;		// used for dash / attack / stun count down
	EFFECT* effect;			// initiated attack / heal effect
	ELEVATOR* elev;			// taken elevator
	int				godCount;		// god mode count down

	int				stamina;		// dash / attack need enough stamina to trigger
	int				health;			// when health reduces to 0, you die.
	int				money;			// defeat enemy get coin.
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

void HitPlayer(ENEMY* enmey, PLAYER* player, int damge, int orient);
