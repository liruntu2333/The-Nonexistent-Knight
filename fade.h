//=============================================================================
//
// �t�F�[�h���� [fade.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// �t�F�[�h�̏��
enum
{
	FADE_NONE = 0,		// �����Ȃ����
	FADE_IN,			// �t�F�[�h�C������
	FADE_OUT,			// �t�F�[�h�A�E�g����
	FADE_MAX
};


struct FADE
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	int				flag;		// FADE_IN / FADE_OUT
	int				nextMode;	// �J�ڂ����̃��[�h
	D3DXCOLOR		color;		// �t�F�[�h�̃J���[�i���l�j

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(int fade, int modeNext);
int GetFade(void);



