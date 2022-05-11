//=============================================================================
//
// �v���C���[���� [ENEMY.cpp]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "terrain.h"
#include <math.h>
#include "player.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "effect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(800/8)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(800/8)	//
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l
#define PI							(acos(-1))

#define	RUN_SPEED					(4.0f)
#define BIG_STUN_FRAME				(30)
#define BIG_STUN_HSPD				(20)
#define PARRY_STUN_FRAME			(60)

#define PARRY_SM_FRAME				(60)	// slow motion mode triggered by sucessful parry
#define DEAD_FRAME					60

#define ENEMYRHT_TEX_NO				1
#define ENEMYLFT_TEX_NO				0

#define GET_HIT_PARTICLE			3

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DestructEnemy(ENEMY* ep);
void ChekHitPlayer(ENEMY* enemy);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = nullptr;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	// �e�N�X�`�����
//
//static LINEAR_INTERPOLATION g_MoveTbl0[] = {
//	//���W									��]��							�g�嗦							����
//	{ D3DXVECTOR3(50.0f,  50.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
//	{ D3DXVECTOR3(250.0f,  50.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 3.14f),	D3DXVECTOR3(0.0f, 0.0f, 1.0f),	0.05f },
//	{ D3DXVECTOR3(250.0f, 250.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(2.0f, 2.0f, 1.0f),	0.005f },
//};
//
//
//static LINEAR_INTERPOLATION g_MoveTbl1[] = {
//	//���W									��]��							�g�嗦							����
//	{ D3DXVECTOR3(1700.0f,   0.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
//	{ D3DXVECTOR3(1700.0f,  SCREEN_HEIGHT, 0.0f),D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(2.0f, 2.0f, 1.0f),	0.01f },
//};
//
//
//static LINEAR_INTERPOLATION g_MoveTbl2[] = {
//	//���W									��]��							�g�嗦							����
//	{ D3DXVECTOR3(3000.0f, 100.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),		D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
//	{ D3DXVECTOR3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
//};
//
//static LINEAR_INTERPOLATION* g_MoveTblAdr[] =
//{
//	g_MoveTbl0,
//	g_MoveTbl1,
//	g_MoveTbl2,
//
//};

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy02.png",
	"data/TEXTURE/enemy_reverse.png",
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
		g_Texture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			nullptr,
			nullptr,
			&g_Texture[i],
		nullptr);
	}

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		ENEMY* s_Enemy = g_Enemy + i;

		s_Enemy->use = TRUE;
		s_Enemy->pos = D3DXVECTOR3(TEXTURE_WIDTH / 2 + 500.0f + 100.0f * i, SCREEN_HEIGHT / 2, 0.0f);	// ���S�_����\��
		s_Enemy->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		s_Enemy->w = TEXTURE_WIDTH;
		s_Enemy->h = TEXTURE_HEIGHT;
		s_Enemy->texNo = ENEMYRHT_TEX_NO;

		s_Enemy->countAnim = 0;
		s_Enemy->patternAnim = 0;

		s_Enemy->state = STAND;
		s_Enemy->orient = RIGHT;
		s_Enemy->atkOrient = RIGHT;
		s_Enemy->atkDetect = TRUE;
		s_Enemy->vertSpd = 0;
		s_Enemy->horzSpd = 0;
		s_Enemy->actCount = 0;
		s_Enemy->effect = nullptr;
		s_Enemy->rddot = nullptr;
		s_Enemy->slashed = FALSE;

		s_Enemy->stamina = 0;
		s_Enemy->health = ENEMY_HEALTH_MAX;

		//// �s���p�^�[����������
		//g_Enemy[i].time = 0.0f;			// ���`��ԗp
		//g_Enemy[i].moveTblNo = 0;			// �f�[�^�e�[�u��
		//g_Enemy[i].tblMax = sizeof(g_MoveTbl0) / sizeof(LINEAR_INTERPOLATION);// ���`��ԗp
	}

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
		g_VertexBuffer = nullptr;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = nullptr;
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
		ENEMY* s_Enemy = g_Enemy + i;

		// Only proceed with active enemy.
		if (s_Enemy->use == TRUE)
		{
			if (s_Enemy->state < DASH || s_Enemy->state > ATTACK)
			{
				// Vertical speed effected by gravity when in the air.
				if (s_Enemy->state > ATTACK)
				{
					s_Enemy->pos.y += s_Enemy->vertSpd;
					s_Enemy->vertSpd += GRAVITATIONAL_CONST;
					if (s_Enemy->vertSpd > FALL_LIMIT)
					{
						s_Enemy->vertSpd = FALL_LIMIT;
					}
					if (s_Enemy->vertSpd >= 0)
					{
						s_Enemy->state = FALL;
					}
					// If there is obstacle above enemy, reverse vertical speed to plus.
					if (GetTerrain(s_Enemy->pos.x, s_Enemy->pos.y - s_Enemy->h / 2))
					{
						s_Enemy->pos.y += 5.0f;
						s_Enemy->vertSpd = 1;
					}
				}

				if (s_Enemy->orient == RIGHT)
				{
					// Move right.
					s_Enemy->pos.x += RUN_SPEED;
					if (GetTerrain(s_Enemy->pos.x + s_Enemy->w / 2, s_Enemy->pos.y))
					{
						s_Enemy->orient = LEFT;
						s_Enemy->texNo = ENEMYLFT_TEX_NO;
					}
				}
				else
				{
					// Move left.
					s_Enemy->pos.x -= RUN_SPEED;
					if (GetTerrain(s_Enemy->pos.x - s_Enemy->w / 2, s_Enemy->pos.y))
					{
						s_Enemy->orient = RIGHT;
						s_Enemy->texNo = ENEMYRHT_TEX_NO;
					}
				}

				// If there is obstacle under player, stand still & stop falling.
				if (GetTerrain(s_Enemy->pos.x, s_Enemy->pos.y + s_Enemy->h / 2))
				{
					if (s_Enemy->state == FALL)
					{
						s_Enemy->vertSpd = 0;
						s_Enemy->state = STAND;
						s_Enemy->pos = ReloacteObj(s_Enemy->pos.x, s_Enemy->pos.y, s_Enemy->w, s_Enemy->h);
					}
				}
				else if (s_Enemy->state <= RUN)
				{
					// Start to fall when walk across the edge.
					s_Enemy->state = FALL;
				}
			}

			// Stun process.
			if (s_Enemy->state == STUN)
			{
				if (s_Enemy->actCount--)
				{
					// red dot occured by parry stick with enemy
					if (s_Enemy->rddot)
					{
						s_Enemy->rddot->pos = s_Enemy->pos;
					}
					// Bounce to right.
					if (s_Enemy->horzSpd > 0)
					{
						if (!GetTerrain(s_Enemy->pos.x + s_Enemy->w / 2, s_Enemy->pos.y))
						{
							s_Enemy->pos.x += s_Enemy->horzSpd;
						}
					}
					// Bouce to left.
					else if (s_Enemy->horzSpd < 0)
					{
						if (!GetTerrain(s_Enemy->pos.x - s_Enemy->w / 2, s_Enemy->pos.y))
						{
							s_Enemy->pos.x += s_Enemy->horzSpd;
						}
					}

					s_Enemy->horzSpd = (int)(0.9f * s_Enemy->horzSpd);

					if (!GetTerrain(s_Enemy->pos.x, s_Enemy->pos.y + s_Enemy->h / 2))
					{
						s_Enemy->pos.y += s_Enemy->vertSpd;
						s_Enemy->vertSpd += GRAVITATIONAL_CONST;
						if (s_Enemy->vertSpd > FALL_LIMIT)
						{
							s_Enemy->vertSpd = FALL_LIMIT;
						}
					}
				}
				else
				{
					s_Enemy->state = FALL;
					s_Enemy->slashed = FALSE;
					// At the end of stunning state, set red dot to false
					if (s_Enemy->rddot)
					{
						s_Enemy->rddot->use = FALSE;
						s_Enemy->rddot = nullptr;
						GetPlayer()->pryDetect = FALSE;
					}
				}
			}
			if (s_Enemy->state == DEAD)
			{
				if (s_Enemy->actCount == DEAD_FRAME)
				{
					SetStunFrame();
				}
				// Death process.
				if (!s_Enemy->actCount--)
				{
					s_Enemy->use = FALSE;
				}
				else
				{
					SetShake(10);
					SetEffect(s_Enemy->pos.x, s_Enemy->pos.y, MAGIC_CIRCLE, rand() % 4);
				}
			}

			if (s_Enemy->state != STUN && s_Enemy->state != DEAD)
			{
				// Check if Hit on Player.
				ChekHitPlayer(s_Enemy);
			}

			// Animation
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

#ifdef _DEBUG
			// �f�o�b�O�\��
			//PrintDebugProc("Enemy X:%f Y:%f\n", g_Enemy[i].pos.x, g_Enemy[i].pos.y);
#endif
		}
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

	Terrain* bg = GetTerrain();

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
	//	PlaySound(SOUND_LABEL_SE_hit000);
	if (ep->use) ep->use = FALSE;
	return;
}

//
// @brief	Do attack detection on player.
// @param	Enemy pointer and attack effect used as collision box. If effect == NULL,
//			enemy itself presents as collision box.
//
void ChekHitPlayer(ENEMY* enemy)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = GetPlayer() + i;
		if (s_Player->use)
		{
			// This damage is occured by melee attacks which has an effect BB.
			if (enemy->effect)
			{
				if (BBCollision(&enemy->effect->pos, &s_Player->pos,
					enemy->effect->w, s_Player->w,
					enemy->effect->h, s_Player->h))
				{
					if (s_Player->pos.y + s_Player->h / 2 < enemy->effect->pos.y)
					{
						enemy->atkOrient = UP;
					}
					else if (s_Player->pos.y - s_Player->h / 2 > enemy->effect->pos.y)
					{
						enemy->atkOrient = DOWN;
					}
					if (s_Player->pos.x + s_Player->w / 2 < enemy->effect->pos.x)
					{
						enemy->atkOrient = LEFT;
					}
					else if (s_Player->pos.x - s_Player->w / 2 > enemy->effect->pos.x)
					{
						enemy->atkOrient = RIGHT;
					}
					HitPlayer(enemy, s_Player, 1, enemy->atkOrient);
				}
			}
			else
			{
				// This damage is occured by enemy collision
				// attacks which doesn't have effect BB.
				if (BBCollision(&enemy->pos, &s_Player->pos,
					enemy->w, s_Player->w,
					enemy->h, s_Player->h))
				{
					if (s_Player->pos.y + s_Player->h / 2 < enemy->pos.y)
					{
						enemy->atkOrient = UP;
					}
					else if (s_Player->pos.y - s_Player->h / 2 > enemy->pos.y)
					{
						enemy->atkOrient = DOWN;
					}
					else if (s_Player->pos.x + s_Player->w / 2 < enemy->pos.x)
					{
						enemy->atkOrient = LEFT;
					}
					else if (s_Player->pos.x - s_Player->w / 2 > enemy->pos.x)
					{
						enemy->atkOrient = RIGHT;
					}
					HitPlayer(enemy, s_Player, 1, enemy->atkOrient);
				}
			}
		}
	}
}

//
// @brief	Decrease ENEMY's health and switch state to STUN.
//			If damage == 0 means enemy get parried.
// @param	enemy pointer, dam, orient (not used when parrying)
// @return
//
void HitEnemy(ENEMY* enemy, int damage, int orient)
{
	enemy->state = STUN;

	if (!damage)
	{
		enemy->actCount = PARRY_STUN_FRAME;
		SetSlowMotion(PARRY_SM_FRAME);
		enemy->rddot = SetEffect(enemy->pos.x, enemy->pos.y, RED_DOT, RIGHT);
		return;
	}
	else
	{
		if (enemy->rddot)
		{
			enemy->rddot->use = FALSE;
			enemy->rddot = nullptr;
		}
	}
	enemy->actCount = BIG_STUN_FRAME;
	SetEffect(enemy->pos.x, enemy->pos.y, BLOOD_SPLASH, orient);
	switch (orient)
	{
	case RIGHT:
	case LEFT:

		enemy->state = STUN;
		if (orient % 4)
			enemy->horzSpd = -BIG_STUN_HSPD * damage;
		else
			enemy->horzSpd = BIG_STUN_HSPD * damage;
		break;

	case DOWN:
	case UP:
		enemy->horzSpd = 0;
		break;

	default:
		break;
	}
	if (damage == 2)
	{
		enemy->slashed = TRUE;
	}
	enemy->health -= damage;
	if (enemy->health <= 0)
	{
		enemy->health = 0;
		enemy->state = DEAD;
		enemy->actCount = DEAD_FRAME;
	}

	for (int i = 0; i < damage; i++)
	{
		SetEffect(enemy->pos.x, enemy->pos.y, COIN, RIGHT);
	}
	for (int i = 0; i < GET_HIT_PARTICLE; i++)
	{
		SetEffect(enemy->pos.x, enemy->pos.y, MAGIC_CIRCLE, orient);
	}
	PlaySound(SOUND_LABEL_SE_hit);
}