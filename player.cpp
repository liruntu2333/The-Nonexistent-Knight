//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : 
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"
#include "file.h"
#include "effect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(800/8)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(800/8)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(5)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define ILLUSION_MAX				(20)

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

//#define PLAYER_FALLING_SPEED		(6)
//#define PLAYER_JUMPING_SPEED		(12)
#define RUN_SPEED					(8.0f)
#define GRAVITATIONAL_CONST			(1)
#define JUMP_SPEED					(-25)
#define FALL_LIMIT					(30)
#define DASH_SPEED					(10)
#define DASH_FRAME					(20)
#define ATK_FRAME					(15)
#define ATK_DETECTION				(3)
#define ATK_END						(9)
#define	ILLUSION_LIFE_SPAN			(30)
#define ILLUSION_GEN_LAG			(4)

enum State
{
	STAND,
	RUN,
	DASH,
	ATTACK,
	FALL,
	JUMP,
	BIG_JUMP,
};

enum Orientation
{
	RIGHT,
	DOWN,
	LEFT,
	UP,
};

enum DETECTION
{
	LIGHT_ATK,
	DETECTION_MAX,
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
struct ILLUSION
{
	PLAYER illusion;
	int life;
};


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/runningman000.png",
	"data/TEXTURE/runningman002.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];	// �v���C���[�\����

static ILLUSION	g_Illusion[ILLUSION_MAX];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;

		s_Player->use = TRUE;
		s_Player->pos = D3DXVECTOR3(TEXTURE_WIDTH/2 + 50, SCREEN_HEIGHT /2 , 0.0f);	// ���S�_����\��
		s_Player->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		s_Player->w   = TEXTURE_WIDTH;
		s_Player->h   = TEXTURE_HEIGHT;
		s_Player->texNo = 1;

		s_Player->countAnim = 0;
		s_Player->patternAnim = 0;

		s_Player->state = STAND;
		s_Player->orient = RIGHT;
		s_Player->atkOrient = RIGHT;
		s_Player->atkDetected = TRUE;
		s_Player->verticalSpeed = 0;
		s_Player->actCount = 0;
		s_Player->atk = NULL;
	}

	for (int i = 0; i < ILLUSION_MAX; i++)
	{
		ILLUSION* s_Illusion = g_Illusion + i;

		s_Illusion->illusion.use = FALSE;
	}

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;
		// �����Ă�v���C���[��������������
		if (s_Player->use == TRUE)
		{
			// �A�j���[�V����  
			s_Player->countAnim += 1.0f;
			if (s_Player->countAnim > ANIM_WAIT)
			{
				s_Player->countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				s_Player->patternAnim = (s_Player->patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �L�[���͂ňړ� 
			{
				//if (GetKeyboardPress(DIK_C))
				//{
				//	speed *= 4;
				//}


				//if (GetKeyboardPress(DIK_DOWN))
				//{
				//	s_Player->pos.y += speed;
				//}
				//else if (GetKeyboardPress(DIK_UP))
				//{
				//	s_Player->pos.y -= speed;
				//}

				// While DASH ~ ATTACK, you can't do anything until act finished.
				if (s_Player->state < DASH || s_Player->state > ATTACK)
				{
					// Move right.
					if (GetKeyboardPress(DIK_D))
					{
						s_Player->orient = RIGHT;

						if (!GetBGData(s_Player->pos.x + s_Player->w / 2, s_Player->pos.y))
						{
							s_Player->pos.x += RUN_SPEED;
						}
					}
					// Move left.
					else if (GetKeyboardPress(DIK_A))
					{
						s_Player->orient = LEFT;
						if (!GetBGData(s_Player->pos.x - s_Player->w / 2, s_Player->pos.y))
						{
							s_Player->pos.x -= RUN_SPEED;
						}
					}
					// Dash trigger.
					if (GetKeyboardTrigger(DIK_LSHIFT))
					{
						s_Player->state = DASH;
						s_Player->actCount = DASH_FRAME;
					}
					// Attack trigger.
					else if (GetKeyboardTrigger(DIK_J))
					{
						s_Player->state = ATTACK;
						s_Player->actCount = ATK_FRAME;
					}
					// Jump only triggers when player's on the ground(STAND ~ RUN).
					if (s_Player->state < DASH && GetKeyboardTrigger(DIK_SPACE))
					{
						s_Player->state = JUMP;
						s_Player->verticalSpeed = JUMP_SPEED;
					}
					if (s_Player->state == JUMP && GetKeyboardPress(DIK_SPACE) &&
						s_Player->verticalSpeed <= JUMP_SPEED * 1 / 2 && s_Player->verticalSpeed >= JUMP_SPEED * 4 / 7)
					{
						s_Player->state = BIG_JUMP;
						s_Player->verticalSpeed += JUMP_SPEED / 5;
					}
				}

				//// �Q�[���p�b�h�łňړ�����
				//if (IsButtonPressed(0, BUTTON_DOWN))
				//{
				//	s_Player->pos.y += speed;

				//}
				//else if (IsButtonPressed(0, BUTTON_UP))
				//{
				//	s_Player->pos.y -= speed;
				//}

				// L&R Movement
				if (s_Player->state < DASH || s_Player->state > ATTACK)
				{
					if (IsButtonPressed(0, BUTTON_RIGHT))
					{
						s_Player->pos.x += RUN_SPEED;
					}
					else if (IsButtonPressed(0, BUTTON_LEFT))
					{
						s_Player->pos.x -= RUN_SPEED;
					}
				}

				// Vertical speed effected by gravity when in the air.
				if (s_Player->state > ATTACK)
				{
					s_Player->pos.y += s_Player->verticalSpeed;
					s_Player->verticalSpeed += GRAVITATIONAL_CONST;
					if (s_Player->verticalSpeed > FALL_LIMIT)
					{
						s_Player->verticalSpeed = FALL_LIMIT;
					}
					if (s_Player->verticalSpeed >= 0)
					{
						s_Player->state = FALL;
					}
					// If there is block above player, reverse vertical speed to plus.
					if (GetBGData(s_Player->pos.x, s_Player->pos.y - s_Player->h / 2))
					{
						s_Player->pos.y += 5.0f;
						s_Player->verticalSpeed = 1;
					}
				}

				// If there is block under player, stand still & stop falling.
				if (GetBGData(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
				{
					if (s_Player->state == FALL)
					{
						s_Player->verticalSpeed = 0;
						s_Player->state = STAND;
						s_Player->pos = ReloacteObj(s_Player->pos.x, s_Player->pos.y, s_Player->w, s_Player->h);
					}
				}
				else if (s_Player->state == RUN || s_Player->state == STAND)
				{
					// Start to fall when walk across the edge.
					s_Player->state = FALL;
				}

				// Dash process.
				if (s_Player->state == DASH)
				{
					// Copy player's data to illusion array. Illusion generates every 10 frames.
					if ((s_Player->actCount % ILLUSION_GEN_LAG) == 0) // Check if it's time to generate illusion
					{
						for (int i = 0; i < ILLUSION_MAX; i++)
						{
							if (!(g_Illusion + i)->illusion.use)
							{
								(g_Illusion + i)->illusion = *s_Player;
								(g_Illusion + i)->life = ILLUSION_LIFE_SPAN;
								g_Illusion->illusion.use = TRUE;
								break;
							}
						}
					}

					switch (s_Player->orient)
					{
					case LEFT:								// Dash to left
						if (!GetBGData(s_Player->pos.x - s_Player->w / 2, s_Player->pos.y))
						{
							s_Player->pos.x -= DASH_SPEED;
						}
						break;					
					case RIGHT:								// Dash to right
						if (!GetBGData(s_Player->pos.x + s_Player->w / 2, s_Player->pos.y))
						{
							s_Player->pos.x += DASH_SPEED;
						}
						break;
					default:
						break;
					}

					// While dashing, dashCount degresses per frame until reaches 0
					if (!--s_Player->actCount) 
					{
						s_Player->state = FALL;
					}
				}

				// Attack process.
				if (s_Player->state == ATTACK)
				{
					if (s_Player->actCount == ATK_FRAME)
					{
						// Check atk direction.
						if (GetKeyboardPress(DIK_W))
						{
							s_Player->atkOrient = UP;
						}
						else if (GetKeyboardPress(DIK_S))
						{
							s_Player->atkOrient = DOWN;
						}
						else if (GetKeyboardPress(DIK_D))
						{
							s_Player->atkOrient = RIGHT;
						}
						else if (GetKeyboardPress(DIK_A))
						{
							s_Player->atkOrient = LEFT;
						}
						else 
						{
							s_Player->atkOrient = s_Player->orient;
						}
						s_Player->atk = SetEffect(s_Player->pos.x, s_Player->pos.y, 
							PLAYER_BLADE, s_Player->atkOrient);
						s_Player->verticalSpeed = 0;
						s_Player->atkDetected = FALSE;
					}
					// Attack dectecting process, hit detection should do only once.
					if (!s_Player->atkDetected && 
						s_Player->actCount > ATK_DETECTION && s_Player->actCount <= ATK_END)
					{
						EFFECT* s_Effect = s_Player->atk;
						float cof = -1.0f;
						float radius = s_Effect->w / 2;
						// Math use to find hit point in enviroment.
						const float rcosrot = radius * (float)cos(s_Effect->rot.z);
						const float rsinrot = radius * (float)sin(s_Effect->rot.z);
						while (cof <= 1.0f)
						{
							// Check if hit on undestroyable object (enviroment bloack).
							if (GetBGData(s_Effect->pos.x + cof * rcosrot, s_Effect->pos.y + cof * rsinrot))
							{
								// Reflect effect.
								SetEffect(s_Player->pos.x + cof * rcosrot, s_Player->pos.y + cof * rsinrot,
									PLAYER_REFLECT, s_Player->atkOrient);
								switch (s_Player->atkOrient)
								{
								case RIGHT:
									break;
								case DOWN:
									if (!GetBGData(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
									{
										s_Player->verticalSpeed = -20;
										s_Player->state = BIG_JUMP;
									}
									break;
								case LEFT:
									s_Player->verticalSpeed = 0;
									s_Player->atk->use = FALSE;
									break;
								case UP:
									s_Player->verticalSpeed = 0;
									break;
								default:
									break;
								}
								s_Player->atkDetected = TRUE;
								break;
							}
							cof += 0.5f;
						}
					}
					
					// While dashing, dashCount degresses per frame until reaches 0
					if (!--s_Player->actCount)
					{
						s_Player->atkOrient = s_Player->orient;
						s_Player->atk = NULL;
						s_Player->state = FALL;
					}
				}

				// Illusion's life decrease to 0
				for (int i = 0; i < ILLUSION_MAX; i++)
				{
					if ((g_Illusion + i)->illusion.use)
					{
						if (!--(g_Illusion + i)->life)
							(g_Illusion + i)->illusion.use = FALSE;
					}
				}

				//// MAP�O�`�F�b�N

				//if (s_Player->pos.x < 0.0f + TEXTURE_WIDTH/2)
				//{
				//	s_Player->pos.x = 0.0f + TEXTURE_WIDTH / 2;
				//}

				//if (s_Player->pos.x > bg->w - TEXTURE_WIDTH/2)
				//{
				//	s_Player->pos.x = bg->w - TEXTURE_WIDTH / 2;
				//}

				//if (s_Player->pos.y < 0.0f + TEXTURE_HEIGHT/2)
				//{
				//	s_Player->pos.y = 0.0f + TEXTURE_HEIGHT / 2;

				//}

				//if (s_Player->pos.y > bg->h - TEXTURE_HEIGHT/2)
				//{
				//	s_Player->pos.y = bg->h - TEXTURE_HEIGHT / 2;
				//	s_Player->falling = FALSE;
				//}

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��


				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				BG* s_BG = GetBG();
				s_BG->pos.x = s_Player->pos.x - PLAYER_DISP_X;
				if (s_BG->pos.x < 0) s_BG->pos.x = 0;
				if (s_BG->pos.x > s_BG->w - SCREEN_WIDTH) s_BG->pos.x = s_BG->w - SCREEN_WIDTH;

				s_BG->pos.y = s_Player->pos.y - PLAYER_DISP_Y;
				if (s_BG->pos.y < 0) s_BG->pos.y = 0;
				if (s_BG->pos.y > s_BG->h - SCREEN_HEIGHT) s_BG->pos.y = s_BG->h - SCREEN_HEIGHT;

				// �o���b�g����
				if (GetKeyboardPress(DIK_SPACE))
				{


				}
				if (IsButtonTriggered(0, BUTTON_B))
				{


				}

				if (GetKeyboardTrigger(DIK_1))
				{
					SaveData();
				}
			}

#ifdef _DEBUG
			// �f�o�b�O�\��
			PrintDebugProc("X:%f Y:%f vS: %d BGd: %d Orient: %d State: %d \n", 
				s_Player->pos.x, 
				s_Player->pos.y, 
				s_Player->verticalSpeed, 
				GetBGData(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2),
				s_Player->orient, 
				s_Player->state);
#endif

		}

	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;
		if (s_Player->use == TRUE && s_Player->state != DASH)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Player->texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = s_Player->pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = s_Player->pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = s_Player->w;		// �v���C���[�̕\����
			float ph = s_Player->h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(s_Player->patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(s_Player->patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = s_Player->patternAnim * tw;	// �e�N�X�`���̍���X���W
			//float ty = s_Player->patternAnim * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				s_Player->rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	for (int i = 0; i < ILLUSION_MAX; i++)
	{
		ILLUSION* s_Illusion = g_Illusion + i;
		if (s_Illusion->illusion.use)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Illusion->illusion.texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = s_Illusion->illusion.pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = s_Illusion->illusion.pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = s_Illusion->illusion.w;		// �v���C���[�̕\����
			float ph = s_Illusion->illusion.h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(s_Illusion->illusion.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(s_Illusion->illusion.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = s_Player->patternAnim * tw;	// �e�N�X�`���̍���X���W
			//float ty = s_Player->patternAnim * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f / ILLUSION_LIFE_SPAN * s_Illusion->life),
				s_Illusion->illusion.rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �v���C���[�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER *GetPlayer(void)
{

	return &g_Player[0];
}




