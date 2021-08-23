//=============================================================================
//
// BG��ʏ��� [bg.cpp]
// Author : 
//
//=============================================================================
#include "bg.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(1600)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(900)	// 
#define TEXTURE_MAX					(2)				// �e�N�X�`���̐�
#define BLOCK_LENGTH				(10)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/earth.png",
	"data/TEXTURE/map.jpg",
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;

static int g_BGData[TEXTURE_HEIGHT / BLOCK_LENGTH][TEXTURE_WIDTH / BLOCK_LENGTH] = { {0} };


// ����������
//=============================================================================
HRESULT InitBG(void)
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
	g_BG.w     = TEXTURE_WIDTH;
	g_BG.h     = TEXTURE_HEIGHT;
	g_BG.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 1;

	for (int i = 0; i < TEXTURE_WIDTH / BLOCK_LENGTH; i++)
	{
		g_BGData[TEXTURE_HEIGHT / BLOCK_LENGTH - 1][i] = 1;
		g_BGData[TEXTURE_HEIGHT / BLOCK_LENGTH - 2][i] = 1;
		g_BGData[TEXTURE_HEIGHT / BLOCK_LENGTH - 3][i] = 2;
		g_BGData[0][i] = 2;
	}

	for (int i = 0; i < 26; i++)
	{
		g_BGData[70][i] = 2;
		for (int j = 71; j < TEXTURE_HEIGHT / BLOCK_LENGTH; j++)
		{
			g_BGData[j][i] = 2;
		}
	}

	for (int i = 45; i < 45 + 26; i++)
	{
		g_BGData[63][i] = 2;
		for (int j = 64; j < 64 + 5; j++)
		{
			g_BGData[j][i] = 1;
		}
	}

	for (int i = 80; i < 80 + 26; i++)
	{
		g_BGData[50][i] = 2;
		for (int j = 51; j < 51 + 5; j++)
		{
			g_BGData[j][i] = 1;
		}
	}
	for (int i = 115; i < 115 + 26; i++)
	{
		g_BGData[40][i] = 2;
		for (int j = 41; j < 41 + 5; j++)
		{
			g_BGData[j][i] = 1;
		}
	}

	for (int i = 0; i < TEXTURE_HEIGHT / BLOCK_LENGTH; i++)
	{
		g_BGData[i][TEXTURE_WIDTH / BLOCK_LENGTH] = 1;
		g_BGData[i][TEXTURE_WIDTH / BLOCK_LENGTH] = 1;
	}
	

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			-g_BG.pos.x, -g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


BG* GetBG(void)
{
	return &g_BG;
}

//
// @brief	
//
//
int GetBGData(float x, float y)
{
	return g_BGData[(int)(y / BLOCK_LENGTH)][(int)(x / BLOCK_LENGTH)];
}

//
// @brief	relocate objects when collide to environment
// @ret leagal loaction in map
//
D3DXVECTOR3 ReloacteObj(float x, float y, float w, float h)
{
	float legalX = x;
	float legalY = y;
	while (true)
	{
		if (GetBGData(legalX, legalY + h / 2) == 2)
			break;
		legalY -= BLOCK_LENGTH;
	}
	return D3DXVECTOR3(legalX, legalY, 0.0f);
}