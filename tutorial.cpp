//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "tutorial.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define MAP_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �e�N�X�`���̃t�@�C����
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/bg000.jpg",
	"data/TEXTURE/tex.png",
};


static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
static TUTORIAL	g_Tutorial;



// ����������
//=============================================================================
HRESULT InitTutorial(void)
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


	// �ϐ��̏�����
	g_Tutorial.w = TEXTURE_WIDTH;
	g_Tutorial.h = MAP_HEIGHT;
	g_Tutorial.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Tutorial.texNo = 0;
	//g_Tutorial.move = 3.7f;
	g_Tutorial.offset = 0.0f;

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
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
void UpdateTutorial(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}

	/*static int flag = 0;
	if (g_Tutorial.pos.x < ( SCREEN_WIDTH - g_Tutorial.w ) && !flag)
	{
		g_Tutorial.pos.x += 4.0f;
	}
	else if (g_Tutorial.pos.x == SCREEN_WIDTH - g_Tutorial.w)
	{
		flag = 1;
	}
	if (flag && g_Tutorial.pos.x > 0)
	{
		g_Tutorial.pos.x -= 4.0f;
	}
	else
	{
		flag = 0;
	}*/

	//g_Tutorial.pos.x += g_Tutorial.move;
	//if (g_Tutorial.pos.x < 0 || g_Tutorial.pos.x > (SCREEN_WIDTH - g_Tutorial.w)) 
	//	g_Tutorial.move = -1 * g_Tutorial.move;



}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Tutorial.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,								//�`��buffer
			g_Tutorial.pos.x, g_Tutorial.pos.y, g_Tutorial.w, g_Tutorial.h,			//�\�����W
			g_Tutorial.offset, 0.0f, 1.0f, 1.0f,										//�摜���W
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));							//�F�̊e�v�f

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}





