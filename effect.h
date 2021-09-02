//=============================================================================
//
// �^�C�g����ʏ��� [particle.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct EFFECT
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	BOOL			use;			// true:�g���Ă���  false:���g�p
	float			w, h;			// ���ƍ���
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�

	float			vertSpd;
	float			horzSpd;
	int				bounce;			// bounced time

	int				type;
};

enum EFFECT_TYPE
{
	PLAYER_BLADE,
	PLAYER_REFLECT,
	PLAYER_HIT,
	COIN,
	EFFECT_TYPE_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

EFFECT* SetEffect(float X, float Y, int type, int orient);
