//=============================================================================
//
// �v���C���[���� [ENEMY.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "map.h"
#include <math.h>
#include "player.h"
#include "bullet.h"
#include "collision.h"
#include "score.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAP_WIDTH				(800/16)	// �L�����T�C�Y
#define MAP_HEIGHT				(800/16)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define PI							acos(-1)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetBullet(D3DXVECTOR3 pos);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bullet00.png"
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �v���C���[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
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


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;
		g_Bullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���S�_����\��
		g_Bullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].w = MAP_WIDTH;
		g_Bullet[i].h = MAP_HEIGHT;
		g_Bullet[i].texNo = 0;
	

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// �ړ���

	}

	g_Load = TRUE;	// �f�[�^�̏��������s����

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet()
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
void UpdateBullet(void)
{
	//if (GetKeyboardTrigger(DIK_K))
	//{
	//	SetBullet(GetPlayer()->pos);
	//}


	for (int i = 0; i < BULLET_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Bullet[i].use == TRUE)
		{
			// �A�j���[�V����  
			g_Bullet[i].countAnim += 1.0f;
			if (g_Bullet[i].countAnim > ANIM_WAIT)
			{
				g_Bullet[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//static float s_enemy_move_x = 4.0f;
			//static float s_enemy_move_y = 4.0f;
			//g_Bullet[i].pos.x += s_enemy_move_x;
			//g_Bullet[i].pos.y += s_enemy_move_y;
			//if (g_Bullet[i].pos.x < 0 || g_Bullet[i].pos.x >(SCREEN_WIDTH - TEXTURE_WIDTH))
			//	s_enemy_move_x = -s_enemy_move_x;
			//if (g_Bullet[i].pos.y < 0 || g_Bullet[i].pos.y >(SCREEN_HEIGHT - TEXTURE_HEIGHT))
			//	s_enemy_move_y = -s_enemy_move_y;


			//	float angle = atan2(GetPlayer()->pos.y - g_Bullet[0].pos.y, GetPlayer()->pos.x - g_Bullet[0].pos.x) + D3DX_PI;


			g_Bullet[i].pos += g_Bullet[i].move;

			PLAYER* player = GetPlayer();
			if (g_Bullet[i].pos.x > player->pos.x + SCREEN_WIDTH)
			{
				DestructBullet(g_Bullet + i);
			}


			for (int j = 0; j < ENEMY_MAX; j++)
			{
				ENEMY* ep = GetEnemy() + j;
				if (ep->use)
				{
					if (BBCollision(&(g_Bullet + i)->pos, &ep->pos, g_Bullet[i].w, ep->w, g_Bullet[i].h, ep->h))
					{
						//DestructEnemy(ep);
						break;
					}
				}
			}

		}

#ifdef _DEBUG
		// �f�o�b�O�\��
		//PrintDebugProc("Bullet No%d  X:%f Y:%f\n", i, g_Bullet[i].pos.x, g_Bullet[i].pos.y);
#endif

	}

}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
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

	BG *bg = GetMap();
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �v���C���[�̕\����
			float ph = g_Bullet[i].h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = g_Bullet[i].patternAnim * tw;	// �e�N�X�`���̍���X���W
			//float ty = g_Bullet[i].patternAnim * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �v���C���[�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET* GetBullet(void)
{

	return &g_Bullet[0];
}


void SetBullet(D3DXVECTOR3 pos)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)
		{
			g_Bullet[i].use = TRUE;
			g_Bullet[i].pos = pos + D3DXVECTOR3(MAP_WIDTH, 0.0F, 0.0F);
			PlaySound(SOUND_LABEL_SE_laser000);
			return;
		}
	}


}

void DestructBullet(BULLET* bp)
{
	if (bp->use) bp->use = FALSE;
	return;
}

