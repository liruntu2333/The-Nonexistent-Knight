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
#define MAP_WIDTH				(1600)	// �w�i�T�C�Y
#define MAP_HEIGHT				(900)	// 
#define ELEV_WIDTH				(260)
#define ELEV_HEIGHT				(60)
#define ELEV_SPD				(1)

#define TEXTURE_MAX				(2)				// �e�N�X�`���̐�
#define BLK_LGTH				(10)			// Block Length
#define ELEV_MAX				(1)
#define	ELEV_TOP				(200)
#define ELEV_BTM				(700)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
struct ELEVATOR
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	int				vrtSpd;
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/map.jpg",
	"data/TEXTURE/elevator.png"
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;

// Data storaged in 2D array. 0 stands for air, 1 earth, 2 ground suface.
static int g_TerrainDT[MAP_HEIGHT / BLK_LGTH][MAP_WIDTH / BLK_LGTH] = { {0} };

static ELEVATOR	g_Elevator;


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
	g_BG.w     = MAP_WIDTH;
	g_BG.h     = MAP_HEIGHT;
	g_BG.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;

	// Set data for Terrain.
	for (int i = 0; i < MAP_WIDTH / BLK_LGTH; i++)
	{
		g_TerrainDT[MAP_HEIGHT / BLK_LGTH - 1][i] = 1;
		g_TerrainDT[MAP_HEIGHT / BLK_LGTH - 2][i] = 1;
		g_TerrainDT[MAP_HEIGHT / BLK_LGTH - 3][i] = 2;
		g_TerrainDT[0][i] = 2;
	}

	for (int i = 0; i < 26; i++)
	{
		g_TerrainDT[70][i] = 2;
		for (int j = 71; j < MAP_HEIGHT / BLK_LGTH; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}

	//for (int i = 45; i < 45 + 26; i++)
	//{
	//	g_TerrainDT[63][i] = 2;
	//	for (int j = 64; j < 64 + 5; j++)
	//	{
	//		g_TerrainDT[j][i] = 1;
	//	}
	//}

	for (int i = 80; i < 80 + 26; i++)
	{
		g_TerrainDT[50][i] = 2;
		for (int j = 51; j < 51 + 5; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}
	for (int i = 115; i < 115 + 26; i++)
	{
		g_TerrainDT[40][i] = 2;
		for (int j = 41; j < 41 + 5; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}

	for (int i = 0; i < MAP_HEIGHT / BLK_LGTH; i++)
	{
		g_TerrainDT[i][MAP_WIDTH / BLK_LGTH] = 1;
	}



	//// Initiate elevator.
	//g_Elevator.w = ELEV_WIDTH;
	//g_Elevator.h = ELEV_HEIGHT;
	//g_Elevator.pos = D3DXVECTOR3(500.0f, 600.0f, 0.0f);
	//g_Elevator.texNo = 1;

	//int SPX = (int)(g_Elevator.pos.x - g_Elevator.w / 2) / BLK_LGTH; // start point x
	//int SPY = (int)(g_Elevator.pos.y - g_Elevator.h / 2) / BLK_LGTH; // start point y
	//for (int j = 1; j < ELEV_HEIGHT / BLK_LGTH; j++)
	//{
	//	for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	//	{
	//		g_TerrainDT[SPY + j][SPX + i] = 1;
	//	}
	//}
	//for (size_t i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	//{
	//	g_TerrainDT[SPY][i] = 2;
	//}

	//g_Elevator.vrtSpd = -BLK_LGTH;
	

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
	//int moveY = g_Elevator.vrtSpd / BLK_LGTH;		// movement in terrainDT

	//int SPX = (int)(g_Elevator.pos.x - g_Elevator.w / 2) / BLK_LGTH; // start point x
	//int SPY = (int)(g_Elevator.pos.y - g_Elevator.h / 2) / BLK_LGTH; // start point y
	//// Destruct old elevator in terrain.
	//for (int j = 0; j < ELEV_HEIGHT / BLK_LGTH; j++)
	//{
	//	for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	//	{
	//		g_TerrainDT[SPY + j][SPX + i] = 0;
	//	}
	//}
	//// Construct new elevator in terrain.
	//SPY += moveY;
	//for (int j = 1; j < ELEV_HEIGHT / BLK_LGTH; j++)
	//{
	//	for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	//	{
	//		g_TerrainDT[SPY + j][SPX + i] = 1;
	//	}
	//}
	//for (size_t i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	//{
	//	g_TerrainDT[SPY][i] = 2;
	//}

	//g_Elevator.pos.y += g_Elevator.vrtSpd;

	//// Reverse elevator's speed when reaches the top/bottom
	//if (g_Elevator.pos.y >= ELEV_BTM || g_Elevator.pos.y <= ELEV_TOP)
	//{
	//	g_Elevator.vrtSpd = -g_Elevator.vrtSpd;
	//}

#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("Elevator X:%f Y:%f vSpd\n",
		g_Elevator.pos.x,
		g_Elevator.pos.y,
		g_Elevator.vrtSpd);
#endif
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

	// Draw elevator
	{

		float ex = g_Elevator.pos.x - g_BG.pos.x;	// �v���C���[�̕\���ʒuX
		float ey = g_Elevator.pos.y - g_BG.pos.y;	// �v���C���[�̕\���ʒuY
		float ew = g_Elevator.w;		// �v���C���[�̕\����
		float eh = g_Elevator.h;		// �v���C���[�̕\������

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Elevator.texNo]);

		SetSpriteLTColor(g_VertexBuffer,
			g_Elevator.pos.x, g_Elevator.pos.y, g_Elevator.w, g_Elevator.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}

}


BG* GetBG(void)
{
	return &g_BG;
}

//
// @brief	get Enviroment Data storaged in 2D array
// @param	location X Y
// @return	0 air 1 bloack 2 surface that can stand on, etc.
//
int GetTerrain(float x, float y)
{
	return g_TerrainDT[(int)(y / BLK_LGTH)][(int)(x / BLK_LGTH)];
}

//
// @brief	relocate objects when collide to
//			environment, only change y
// @return leagal loaction in map
//
D3DXVECTOR3 ReloacteObj(float x, float y, float w, float h)
{
	int feetY = (int)((y + h/2) / BLK_LGTH);
	int feetX = (int)(x / BLK_LGTH);
	while (true)
	{
		if (g_TerrainDT[feetY][feetX] == 2)
			break;
		feetY--;
	}
	return D3DXVECTOR3(x, (float)feetY * BLK_LGTH - h / 2, 0.0f);
}