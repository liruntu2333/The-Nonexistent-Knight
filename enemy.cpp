//=============================================================================
//
// �v���C���[���� [ENEMY.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include <math.h>
#include "player.h"
#include "collision.h"
#include "score.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(800/8)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(800/8)	// 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define PI							acos(-1)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DestructEnemy(ENEMY* ep);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static LINEAR_INTERPOLATION g_MoveTbl0[] = {
	//���W									��]��							�g�嗦							����
	{ D3DXVECTOR3(50.0f,  50.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
	{ D3DXVECTOR3(250.0f,  50.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 3.14f),	D3DXVECTOR3(0.0f, 0.0f, 1.0f),	0.05f },
	{ D3DXVECTOR3(250.0f, 250.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(2.0f, 2.0f, 1.0f),	0.005f },
};


static LINEAR_INTERPOLATION g_MoveTbl1[] = {
	//���W									��]��							�g�嗦							����
	{ D3DXVECTOR3(1700.0f,   0.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
	{ D3DXVECTOR3(1700.0f,  SCREEN_HEIGHT, 0.0f),D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(2.0f, 2.0f, 1.0f),	0.01f },
};


static LINEAR_INTERPOLATION g_MoveTbl2[] = {
	//���W									��]��							�g�嗦							����
	{ D3DXVECTOR3(3000.0f, 100.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),		D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
	{ D3DXVECTOR3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
};

static LINEAR_INTERPOLATION* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy01.png",
	"data/TEXTURE/enemy02.png",
	"data/TEXTURE/enemy03.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];	// �v���C���[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = TRUE;
		g_Enemy[i].pos = D3DXVECTOR3(50.0f + i * 150, 100.0f, 0.0f);	// ���S�_����\��
		g_Enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w = TEXTURE_WIDTH;
		g_Enemy[i].h = TEXTURE_HEIGHT;
		g_Enemy[i].texNo = i;

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_Enemy[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);

		// �s���p�^�[����������
		g_Enemy[i].time = 0.0f;			// ���`��ԗp
		g_Enemy[i].moveTblNo = 0;			// �f�[�^�e�[�u��
		g_Enemy[i].tblMax = sizeof(g_MoveTbl0) / sizeof(LINEAR_INTERPOLATION);// ���`��ԗp

	}

	// �Q�Ԗڂ̓z�̍s���p�^�[�����Z�b�g
	g_Enemy[1].moveTblNo = 1;				// �f�[�^�e�[�u��
	g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(LINEAR_INTERPOLATION);	// ���`��ԗp

	// �R�Ԗڂ̓z�̍s���p�^�[�����Z�b�g
	g_Enemy[2].moveTblNo = 2;				// �f�[�^�e�[�u��
	g_Enemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(LINEAR_INTERPOLATION);	// ���`��ԗp

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Enemy[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = g_Enemy[i].pos;

			// �A�j���[�V����  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//static float s_enemy_move_x = 4.0f;
			//static float s_enemy_move_y = 4.0f;
			//g_Enemy[i].pos.x += s_enemy_move_x;
			//g_Enemy[i].pos.y += s_enemy_move_y;
			//if (g_Enemy[i].pos.x < 0 || g_Enemy[i].pos.x >(SCREEN_WIDTH - TEXTURE_WIDTH))
			//	s_enemy_move_x = -s_enemy_move_x;
			//if (g_Enemy[i].pos.y < 0 || g_Enemy[i].pos.y >(SCREEN_HEIGHT - TEXTURE_HEIGHT))
			//	s_enemy_move_y = -s_enemy_move_y;

			//static float s_enemy_move_x_1 = 2.0f;
			//static float s_enemy_move_y_2 = 4.0f;
			//static double s_enemy_move_t_3 = 0.0;
			//D3DXVECTOR3 pos = GetPlayer()->pos - g_Enemy[0].pos;
			////	float angle = atan2(GetPlayer()->pos.y - g_Enemy[0].pos.y, GetPlayer()->pos.x - g_Enemy[0].pos.x) + D3DX_PI;

			//switch (i)
			//{
			//case 0:
			//	//g_Enemy[i].pos.x += s_enemy_move_x_1;
			//	//if (g_Enemy[i].pos.x < 0 || g_Enemy[i].pos.x >(SCREEN_WIDTH - TEXTURE_WIDTH / 2))
			//	//	s_enemy_move_x_1 = -s_enemy_move_x_1;
			//	g_Enemy[i].pos += pos * 0.01f;
			//	//	g_Enemy[i].pos.x += static_cast<float> (cos(angle) * s_enemy_move_x_1);
			//	//	g_Enemy[i].pos.y += static_cast<float> (sin(angle) * s_enemy_move_x_1);

			//	break;
			//case 1:
			//	g_Enemy[i].pos.y += s_enemy_move_y_2;
			//	if (g_Enemy[i].pos.y - TEXTURE_HEIGHT / 2 < 0 || g_Enemy[i].pos.y > (SCREEN_HEIGHT - TEXTURE_HEIGHT / 2))
			//		s_enemy_move_y_2 = -s_enemy_move_y_2;
			//	break;
			//case 2:
			//	g_Enemy[i].pos.x = 500.0f + 100.0f * cos(s_enemy_move_t_3);
			//	g_Enemy[i].pos.y = 300.0f + 100.0f * sin(s_enemy_move_t_3);
			//	s_enemy_move_t_3 += PI / 30;
			//	break;
			//default:
			//	break;
			//}

			// �ړ����� 
			{
				// �s���e�[�u���ɏ]���č��W�ړ��i���`��ԁj
				int nowNo = (int)g_Enemy[i].time;			// �������ł���e�[�u���ԍ������o���Ă���
				int maxNo = g_Enemy[i].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
				int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
				LINEAR_INTERPOLATION* tbl = g_MoveTblAdr[g_Enemy[i].moveTblNo];	// �s���e�[�u���̃A�h���X���擾
				D3DXVECTOR3	pos = tbl[nextNo].pos - tbl[nowNo].pos;	// XYZ�ړ��ʂ��v�Z���Ă���
				D3DXVECTOR3	rot = tbl[nextNo].rot - tbl[nowNo].rot;	// XYZ��]�ʂ��v�Z���Ă���
				D3DXVECTOR3	scl = tbl[nextNo].scl - tbl[nowNo].scl;	// XYZ�g�嗦���v�Z���Ă���
				float nowTime = g_Enemy[i].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���
				pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
				rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
				scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

				// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
				g_Enemy[i].pos = tbl[nowNo].pos + pos;

				// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
				g_Enemy[i].rot = tbl[nowNo].rot + rot;

				// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
				g_Enemy[i].scl = tbl[nowNo].scl + scl;
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frame���g�Ď��Ԍo�ߏ���������
				g_Enemy[i].time += tbl[nowNo].time;			// ���Ԃ�i�߂Ă���
				if ((int)g_Enemy[i].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Enemy[i].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
				}
			}

			BG* bg = GetBG();

			if (g_Enemy[i].pos.x < 0.0f)
			{
				g_Enemy[i].pos.x = 0.0f;
			}

			if (g_Enemy[i].pos.x > bg->w)
			{
				g_Enemy[i].pos.x = bg->w;
			}

			if (g_Enemy[i].pos.y < 0.0f)
			{
				g_Enemy[i].pos.y = 0.0f;
			}

			if (g_Enemy[i].pos.y > bg->h)
			{
				g_Enemy[i].pos.y = bg->h;
			}

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				if ((GetPlayer() + j)->use)
				{
					if (BBCollision(&(g_Enemy + i)->pos, &(GetPlayer() + j)->pos,
						(g_Enemy + i)->w, (GetPlayer() + j)->w,
						(g_Enemy + i)->h, (GetPlayer() + j)->h))
					{
						DestructEnemy(g_Enemy + i);
					}
				}
			}
		}

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Enemy No%d  X:%f Y:%f\n", i, g_Enemy[i].pos.x, g_Enemy[i].pos.y);
#endif

	}

}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].pos.x - bg->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[i].pos.y - bg->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Enemy[i].w;		// �G�l�~�[�̕\����
			float ph = g_Enemy[i].h;		// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f;	// �e�N�X�`���̕�
			//float th = 1.0f;	// �e�N�X�`���̍���
			//float tx = 0.0f;	// �e�N�X�`���̍���X���W
			//float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �v���C���[�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}

void DestructEnemy(ENEMY* ep)
{
	AddScore(100);
	PlaySound(SOUND_LABEL_SE_hit000);
	if (ep->use) ep->use = FALSE;
	return;
}