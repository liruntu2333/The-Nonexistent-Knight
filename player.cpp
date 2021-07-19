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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(800/8)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(800/8)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


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
		g_Player[i].use = TRUE;
		g_Player[i].pos = D3DXVECTOR3(TEXTURE_WIDTH/2, SCREEN_HEIGHT /2 , 0.0f);	// ���S�_����\��
		g_Player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].w   = TEXTURE_WIDTH;
		g_Player[i].h   = TEXTURE_HEIGHT;
		g_Player[i].texNo = 1;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;

		g_Player[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// �ړ���

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
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = g_Player[i].pos;

			// �A�j���[�V����  
			g_Player[i].countAnim += 1.0f;
			if (g_Player[i].countAnim > ANIM_WAIT)
			{
				g_Player[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �L�[���͂ňړ� 
			{
				float speed = g_Player[i].move.x;

				if (GetKeyboardPress(DIK_C))
				{
					speed *= 4;
				}


				if (GetKeyboardPress(DIK_DOWN))
				{
					g_Player[i].pos.y += speed;
				}
				else if (GetKeyboardPress(DIK_UP))
				{
					g_Player[i].pos.y -= speed;
				}

				if (GetKeyboardPress(DIK_RIGHT))
				{
					g_Player[i].pos.x += speed;
				}
				else if (GetKeyboardPress(DIK_LEFT))
				{
					g_Player[i].pos.x -= speed;
				}

				// �Q�[���p�b�h�łňړ�����
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					g_Player[i].pos.y += speed;

				}
				else if (IsButtonPressed(0, BUTTON_UP))
				{
					g_Player[i].pos.y -= speed;
				}

				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].pos.x += speed;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					g_Player[i].pos.x -= speed;
				}

				// MAP�O�`�F�b�N
				BG *bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��

				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;

				// �o���b�g����
				if (GetKeyboardPress(DIK_SPACE))
				{

				}
				if (IsButtonTriggered(0, BUTTON_B))
				{


				}

				if (GetKeyboardTrigger(DIK_S))
				{
					SaveData();
				}

			}
		}

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Player No%d  X:%f Y:%f\n", i, g_Player[i].pos.x, g_Player[i].pos.y);
#endif

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
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Player[i].w;		// �v���C���[�̕\����
			float ph = g_Player[i].h;		// �v���C���[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			//float tx = g_Player[i].patternAnim * tw;	// �e�N�X�`���̍���X���W
			//float ty = g_Player[i].patternAnim * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Player[i].rot.z);

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




