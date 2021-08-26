//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : 
//
//=============================================================================
#include "score.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCORE_DW)	// �����̃T�C�Y
#define MAP_HEIGHT				(SCORE_DH)	// 
#define TEXTURE_MAX					(1)			// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/number16x32.png",
};

static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
static SCORE	g_Score;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
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


	// �ϐ��̏�����
	g_Score.w = TEXTURE_WIDTH;
	g_Score.h = MAP_HEIGHT;
	g_Score.pos = D3DXVECTOR3(SCORE_DX, SCORE_DY, 0.0f);
	g_Score.texNo = 0;
	g_Score.score = 0;	// ���_��������

	g_Load = TRUE;		// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
{
	//// ���_�o�b�t�@�ݒ�
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//// �}�g���N�X�ݒ�
	//SetWorldViewProjection2D();

	//// �v���~�e�B�u�g�|���W�ݒ�
	//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//// �}�e���A���ݒ�
	//MATERIAL material;
	//ZeroMemory(&material, sizeof(material));
	//material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//SetMaterial(material);

	//// �e�N�X�`���ݒ�
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Score.texNo]);

	//// ��������������
	//int number = g_Score.score;

	//for (int i = 0; i < SCORE_DIGIT; i++)
	//{
	//	// ����\�����錅�̐���
	//	float x = (float)(number % 10);

	//	// ���̌���
	//	number /= 10;
	//	
	//	// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
	//	float px = g_Score.pos.x - g_Score.w*i;	// �X�R�A�̕\���ʒuX
	//	float py = g_Score.pos.y;				// �X�R�A�̕\���ʒuY
	//	float pw = g_Score.w;					// �X�R�A�̕\����
	//	float ph = g_Score.h;					// �X�R�A�̕\������

	//	float tw = 1.0f / 10;					// �e�N�X�`���̕�
	//	float th = 1.0f / 1;					// �e�N�X�`���̍���
	//	float tx = x * tw;						// �e�N�X�`���̍���X���W
	//	float ty = 0.0f;						// �e�N�X�`���̍���Y���W

	//	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//	SetSpriteLTColor(g_VertexBuffer,
	//		px, py, pw, ph,
	//		tx, ty, tw, th,
	//		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//	// �|���S���`��
	//	GetDeviceContext()->Draw(4, 0);

	//}

}


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddScore(int add)
{
	g_Score.score += add;
	if (g_Score.score > SCORE_MAX)
	{
		g_Score.score = SCORE_MAX;
	}

}


int GetScore(void)
{
	return g_Score.score;
}


void SetScore(int score)
{
	g_Score.score = score;
}