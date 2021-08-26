//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "file.h"
#include "main.h"

#include <math.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
static const float TEXTURE_WIDTH			= SCREEN_WIDTH;// �w�i�T�C�Y
static const float MAP_HEIGHT			= SCREEN_HEIGHT;// 
static const int TEXTURE_MAX				= 4;// �e�N�X�`���̐�
											  
static const int LOGO_MAX					= 2;
static const float LOGO_WIDTH				= TEXTURE_WIDTH / 2;
static const float LOGO_HEIGHT				= MAP_HEIGHT / 2;
static const int LOGO_DIVIDE_X				= 100;
static const int LOGO_DIVIDE_Y				= 50;
											  
static const float MOSAIC_WIDTH				= 100.0f;
static const float MOSAIC_HEIGHT			= 100.0f;
static const int MOSAIC_DIVIDE_X			= 50;
static const int MOSAIC_DIVIDE_Y			= 50;
static const int MOSAIC_SAMPLING_INTVL		= 3;

#define M_PI 3.14159265358979323846
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

// �e�N�X�`���̃t�@�C����
static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/bg000.jpg",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/title_logo.png",
};


static BOOL		g_Load = FALSE;		// ���������s�������̃t���O
static TITLE	g_Title, g_Logo[LOGO_MAX], g_Mosaic;

static double effect_dx, effect_dy, effect_dt; // display x y for effect
static const double effect_ddt = 2 * M_PI / 100;	

// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	g_Title.w     = TEXTURE_WIDTH;
	g_Title.h     = MAP_HEIGHT;
	g_Title.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Title.texNo = 1;
	//g_Title.move = 3.7f;
	g_Title.offset = 0.0f;
	
	for (size_t i = 0; i < LOGO_MAX; i++)
	{
		g_Logo[i].w = LOGO_WIDTH;
		g_Logo[i].h = LOGO_HEIGHT;
		g_Logo[i].pos = D3DXVECTOR3(TEXTURE_WIDTH / 4, MAP_HEIGHT / LOGO_MAX * i, 0.0f);
		g_Logo[i].texNo = 3;
	}

	g_Mosaic.w = MOSAIC_WIDTH;
	g_Mosaic.h = MOSAIC_HEIGHT;
	g_Mosaic.pos = g_Logo[1].pos;
	g_Mosaic.texNo = 3;


	effect_dx = 100.0f;
	effect_dy = 100.0f;
	effect_dt = 0.0f;

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}

	g_Title.offset += 0.001f;

	float speed = 4.0f;

	if (GetKeyboardPress(DIK_DOWN))
	{
		effect_dy += speed;
		g_Mosaic.pos.y += speed;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		effect_dy -= speed;
		g_Mosaic.pos.y -= speed;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		effect_dx += speed;
		g_Mosaic.pos.x += speed;
	}
	if (GetKeyboardPress(DIK_LEFT))
	{
		effect_dx -= speed;
		g_Mosaic.pos.x -= speed;
	}

	effect_dt += effect_ddt;

	if (GetKeyboardTrigger(DIK_L))
	{
		SetLoadGame(TRUE);
		SetFade(FADE_OUT, MODE_GAME);
	}

	/*static int flag = 0;
	if (g_Title.pos.x < ( SCREEN_WIDTH - g_Title.w ) && !flag)
	{
		g_Title.pos.x += 4.0f;
	}
	else if (g_Title.pos.x == SCREEN_WIDTH - g_Title.w)
	{
		flag = 1;
	}
	if (flag && g_Title.pos.x > 0)
	{
		g_Title.pos.x -= 4.0f;
	}
	else
	{
		flag = 0;
	}*/

	//g_Title.pos.x += g_Title.move;
	//if (g_Title.pos.x < 0 || g_Title.pos.x > (SCREEN_WIDTH - g_Title.w)) 
	//	g_Title.move = -1 * g_Title.move;



}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// 
	//	draw background
	//
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,								//�`��buffer
			g_Title.pos.x, g_Title.pos.y, g_Title.w, g_Title.h,			//�\�����W
			g_Title.offset, 0.0f, 1.0f, 1.0f,										//�摜���W
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));							//�F�̊e�v�f

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//
	// draw additive color mixing effect
	//
	SetBlendState(BLEND_MODE_ADD);
	//SetBlendState(BLEND_MODE_SUBTRACT);
	for (int i = 0; i < 30; i++)
	{
		GetDeviceContext()->PSGetShaderResources(0, 1, &g_Texture[2]);

		float dx = (float)effect_dx;
		float dy = (float)effect_dy;
		float sx = (float)(rand() % 100);
		float sy = (float)(rand() % 100);

		SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 0.3f, 0.3f, 0.5f));

		GetDeviceContext()->Draw(4, 0);
	}
	SetBlendState(BLEND_MODE_ALPHABLEND);

	//
	// draw title logo No.0
	//
	static const float Logo_div_xw = g_Logo[0].w / LOGO_DIVIDE_X;
	static const float Logo_div_yh = g_Logo[0].h / LOGO_DIVIDE_Y;

	static const float Logo_div_uw = 1.0f / LOGO_DIVIDE_X;
	static const float Logo_div_vh = 1.0f / LOGO_DIVIDE_Y;

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Logo[0].texNo]);
	for (int i = 0; i < LOGO_DIVIDE_Y; i++)
	{
		double phase_x = 2 * M_PI / LOGO_DIVIDE_Y * i + effect_dt;
		
		for (int j = 0; j < LOGO_DIVIDE_X; j++)
		{
			double phase_y = 2 * M_PI / LOGO_DIVIDE_X * j + effect_dt;

			SetSpriteLTColor(g_VertexBuffer,							// buffer
				(float) (g_Logo[0].pos.x + sin(phase_x) * effect_dx / 10 + Logo_div_xw * j),			// X
				(float) (g_Logo[0].pos.y + sin(phase_y) * effect_dx / 10 + Logo_div_yh * i),			// Y
				Logo_div_xw,										// Width
				Logo_div_yh,										// Height
				Logo_div_uw * j,								// U
				Logo_div_vh * i,								// V
				Logo_div_uw,									// UW
				Logo_div_vh,									// VH
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.2f - (FLOAT) effect_dy / 500.0f));	//�F�̊e�v�f
			GetDeviceContext()->Draw(4, 0);

		}

		// apply horizontal wave to title logo
		//// �e�N�X�`���ݒ�
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Logo[i].texNo]);

		//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetSpriteLTColor(g_VertexBuffer,							// buffer
		//	g_Logo[i].pos.x + sin(phase_x) * effect_dx / 10,			// X
		//	g_Logo[i].pos.y + g_Logo[i].h * i / LOGO_DIVIDE_Y,			// Y
		//	g_Logo[i].w,												// Width
		//	g_Logo[i].h / LOGO_DIVIDE_Y,								// Height
		//	0.0f,													// U
		//	1.0f / LOGO_DIVIDE_Y * i,								// V
		//	1.0f,													// UW
		//	1.0f / LOGO_DIVIDE_Y,									// VH
		//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.2f - effect_dy / 500.0f ));	//�F�̊e�v�f

		//// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);
	}

	//
	// draw Title logo No.1
	// 	   
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Logo[1].texNo]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(g_VertexBuffer,										//�`��buffer
		g_Logo[1].pos.x, g_Logo[1].pos.y, g_Logo[1].w, g_Logo[1].h,			//�\�����W
		g_Logo[1].offset, 0.0f, 1.0f, 1.0f,									//�摜���W
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));									//�F�̊e�v�f

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	//
	// draw mosaic
	//

	static const float Mosaic_div_xw = g_Mosaic.w / MOSAIC_DIVIDE_X;
	static const float Mosaic_div_yh = g_Mosaic.h / MOSAIC_DIVIDE_Y;

	static const float Mosaic_div_uw = 1.0f * MOSAIC_WIDTH / LOGO_WIDTH / MOSAIC_DIVIDE_X;
	static const float Mosaic_div_vh = 1.0f * MOSAIC_HEIGHT / LOGO_HEIGHT / MOSAIC_DIVIDE_Y ;

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Mosaic.texNo]);
	for (int i = 0; i < MOSAIC_DIVIDE_Y; i++)
	{
		float Mosaic_y = g_Mosaic.pos.y + i * Mosaic_div_yh;
		int v_range = i / MOSAIC_SAMPLING_INTVL;

		for (int j = 0; j < MOSAIC_DIVIDE_X; j++)
		{
			float Mosaic_x = g_Mosaic.pos.x + j * Mosaic_div_xw;

			if (Mosaic_x >= g_Logo[1].pos.x && Mosaic_x <= g_Logo[1].pos.x + g_Logo[1].w &&
				Mosaic_y >= g_Logo[1].pos.y && Mosaic_y <= g_Logo[1].pos.y + g_Logo[1].h)
			{
				int u_range = j / MOSAIC_SAMPLING_INTVL;

				float Mosaic_u = (g_Mosaic.pos.x - g_Logo[1].pos.x) / LOGO_WIDTH +		// left side coordinate of the whole mosaic block		
					u_range * MOSAIC_SAMPLING_INTVL * Mosaic_div_uw +					// horizontal offset before sampling block				
					rand() % MOSAIC_SAMPLING_INTVL * Mosaic_div_uw;						// random sampling in u
				float Mosaic_v = (g_Mosaic.pos.y - g_Logo[1].pos.y) / LOGO_HEIGHT +		// top side coordinate of the whole mosaic block		
					v_range * MOSAIC_SAMPLING_INTVL * Mosaic_div_vh +					// vertical offset before sampling block				
					rand() % MOSAIC_SAMPLING_INTVL * Mosaic_div_vh;						// random sampling in v

				SetSpriteLTColor(g_VertexBuffer,						// buffer
					Mosaic_x,											// X
					Mosaic_y,											// Y
					Mosaic_div_xw,										// Width
					Mosaic_div_yh,										// Height
					Mosaic_u,											// U
					Mosaic_v,											// V
					Mosaic_div_uw,										// UW
					Mosaic_div_vh,										// VH
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));					//�F�̊e�v�f
				GetDeviceContext()->Draw(4, 0);

			}
		}
	}
}





