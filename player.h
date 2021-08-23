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
#define PLAYER_MAX		(1)		// �v���C���[��Max�l��



//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct EFFECT;

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
	BOOL			atkDetected;	// 0 not yet 1 had
	int				verticalSpeed;	// vertical movement per frame
	int				actCount;		// used for dash / attack count down
	EFFECT*			atk;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);



