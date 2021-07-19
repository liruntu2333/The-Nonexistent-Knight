//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_MAX		(3)		// �G�l�~�[��Max�l��


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
	D3DXVECTOR3		move;			// �ړ����x

	float			time;			// ���`��ԗp
	int				moveTblNo;		// �s���f�[�^�̃e�[�u���ԍ�
	int				tblMax;			// ���̃e�[�u���̃f�[�^��
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

void DestructEnemy(ENEMY* ep);

