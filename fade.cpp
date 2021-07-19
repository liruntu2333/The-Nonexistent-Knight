//=============================================================================
//
// �t�F�[�h���� [fade.cpp]
// Author : 
//
//=============================================================================
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�

#define	FADE_RATE					(0.02f)			// �t�F�[�h�W��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static void SetVertex(float X, float Y, float Width, float Height, float U, float V, float UW, float VH);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/fade_black.png",
};

static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
static FADE		g_Fade;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitFade(void)
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
	g_Fade.w = TEXTURE_WIDTH;
	g_Fade.h = TEXTURE_HEIGHT;
	g_Fade.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Fade.texNo = 0;


	g_Fade.flag  = FADE_IN;
	g_Fade.nextMode = MODE_END;
	g_Fade.color = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFade(void)
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
void UpdateFade(void)
{

	if (g_Fade.flag != FADE_NONE)
	{// �t�F�[�h������
		if (g_Fade.flag == FADE_OUT)
		{// �t�F�[�h�A�E�g����
			g_Fade.color.a += FADE_RATE;		// ���l�����Z���ĉ�ʂ������Ă���
			if (g_Fade.color.a >= 1.0f)
			{
				// �t�F�[�h�C�������ɐ؂�ւ�
				g_Fade.color.a = 1.0f;
				SetFade(FADE_IN, g_Fade.nextMode);

				// ���[�h��ݒ�
				SetMode(g_Fade.nextMode);
			}

		}
		else if (g_Fade.flag == FADE_IN)
		{// �t�F�[�h�C������
			g_Fade.color.a -= FADE_RATE;		// ���l�����Z���ĉ�ʂ𕂂��オ�点��
			if (g_Fade.color.a <= 0.0f)
			{
				// �t�F�[�h�����I��
				g_Fade.color.a = 0.0f;
				SetFade(FADE_NONE, g_Fade.nextMode);
			}

		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFade(void)
{
	if (g_Fade.flag == FADE_NONE) return;	// �t�F�[�h���Ȃ��̂Ȃ�`�悵�Ȃ�

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

	// �t�F�[�h�p�|���S����`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Fade.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetVertex(g_Fade.pos.x, g_Fade.pos.y, g_Fade.w, g_Fade.h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


}


//=============================================================================
// ���_�f�[�^�ݒ�
//=============================================================================
static void SetVertex(float X, float Y, float Width, float Height, float U, float V, float UW, float VH)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

	vertex[0].Position = D3DXVECTOR3(X, Y, 0.0f);
	vertex[0].Diffuse  = g_Fade.color;
	vertex[0].TexCoord = D3DXVECTOR2(U, V);

	vertex[1].Position = D3DXVECTOR3(X + Width, Y, 0.0f);
	vertex[1].Diffuse  = g_Fade.color;
	vertex[1].TexCoord = D3DXVECTOR2(U + UW, V);

	vertex[2].Position = D3DXVECTOR3(X, Y + Height, 0.0f);
	vertex[2].Diffuse  = g_Fade.color;
	vertex[2].TexCoord = D3DXVECTOR2(U, V + VH);

	vertex[3].Position = D3DXVECTOR3(X + Width, Y + Height, 0.0f);
	vertex[3].Diffuse  = g_Fade.color;
	vertex[3].TexCoord = D3DXVECTOR2(U + UW, V + VH);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

}


//=============================================================================
// �t�F�[�h�̏�Ԑݒ�
//=============================================================================
void SetFade(int fade, int nextMode)
{
	g_Fade.flag = fade;
	g_Fade.nextMode = nextMode;
}

//=============================================================================
// �t�F�[�h�̏�Ԏ擾
//=============================================================================
int GetFade(void)
{
	return g_Fade.flag;
}



