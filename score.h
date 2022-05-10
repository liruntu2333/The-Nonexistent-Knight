//=============================================================================
//
// �X�R�A���� [score.h]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_MAX			(99999)		// �X�R�A�̍ő�l
#define SCORE_DIGIT			(5)			// ����

#define	SCORE_DX			(500.0f)	// �\���ʒu
#define	SCORE_DY			(20.0f)		//
#define	SCORE_DW			(16.0f)		//
#define	SCORE_DH			(32.0f)		//

struct SCORE
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	int				score;		// ���_
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

int GetScore(void);
void AddScore(int add);
void SetScore(int score);
