//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_MAX		(2)		// �G�l�~�[��Max�l��
#define	ENEMY_HEALTH_MAX					(20)


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct	LINEAR_INTERPOLATION		// ���`��ԃf�[�^�̍\����
{
	D3DXVECTOR3		pos;			// ���W
	D3DXVECTOR3		rot;			// ��]��
	D3DXVECTOR3		scl;			// �g�嗦
	float			time;			// �ړ��t���[����
};

struct EFFECT;

struct ENEMY
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	D3DXVECTOR3		scl;			// �|���S���̊g�嗦
	BOOL			use;			// true:�g���Ă���  false:���g�p
	float			w, h;			// ���ƍ���
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�
	//D3DXVECTOR3		move;			// �ړ����x

	int				state;			// current movement
	int				orient;			// 0left 1right
	int				atkOrient;		// 0right 1down 2left 3up
	BOOL			atkDetect;		// 0 not yet 1 had
	BOOL			slashed;		// 0 not yet 1 had
	int				vertSpd;		// vertical movement per frame
	int				horzSpd;		// horizontal movement per frame, only used when stunned
	int				actCount;		// used for dash / attack / STUN count down
	EFFECT*         effect;			// initiated effect
	EFFECT*			rddot;			// red dot


	int				stamina;		// dash / attack need enough stamina to trigger
	int				health;			// when health reduces to 0, enemy die.

	//float			time;			// ���`��ԗp
	//int				moveTblNo;		// �s���f�[�^�̃e�[�u���ԍ�
	//int				tblMax;			// ���̃e�[�u���̃f�[�^��
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

void HitEnemy(ENEMY* enemy, int damge, int orient);


