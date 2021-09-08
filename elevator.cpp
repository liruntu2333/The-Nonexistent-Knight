//
// @file				elevator.cpp
// @brief				An elevator object that can move around and 
//						modifies terrain.
// @copyright			2021 LI ZIZHEN liruntu2333@gmail.com
// @lisense				GPL
//

#include "main.h"
#include "renderer.h"
#include "map.h"
#include "sprite.h"
#include "elevator.h"


//
//	Macros
//
#define TEXTURE_MAX				(1)

#define ELEV_WIDTH				(260)
#define ELEV_HEIGHT				(60)
#define ELEV_SPD				(3)

#define	ELEV_TOP				(250)
#define ELEV_BTM				(600)

//
//	Function Prototype Declarations
//

//
//	Global Variables
//
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/elevator.png"
};

static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

static ELEVATOR	g_Elevator;

//
// @brief	initiate elevator object
// @param	
// @return	
//
HRESULT InitElev(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// Initiate elevator.
	g_Elevator.w = ELEV_WIDTH;
	g_Elevator.h = ELEV_HEIGHT;
	g_Elevator.pos = D3DXVECTOR3(250.0f, ELEV_TOP, 0.0f);
	g_Elevator.texNo = 0;

	int SPX = (int)(g_Elevator.pos.x / BLK_LGTH); // start point x
	int SPY = (int)(g_Elevator.pos.y / BLK_LGTH); // start point y
	for (int j = 1; j < ELEV_HEIGHT / BLK_LGTH; j++)
	{
		for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
		{
			SetTerrain(SPX + i, SPY + j, EARTH);
		}
	}
	for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	{
		SetTerrain(SPX + i, SPY, GROUND);
	}

	g_Elevator.vertSpd = ELEV_SPD;

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//
// @brief	destruct elevator object
// @param	
// @return	
//
void UninitElev(void)
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

//
// @brief	update elevator object
// @param	
// @return	
//
void UpdateElev(void)
{
	// Destruct old elevator in terrain.
	int SPX = (int)(g_Elevator.pos.x / BLK_LGTH); // start point x
	int SPY = (int)(g_Elevator.pos.y / BLK_LGTH); // start point y
	for (int j = 0; j < ELEV_HEIGHT / BLK_LGTH; j++)
	{
		for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
		{
			SetTerrain(SPX + i, SPY + j, AIR);
		}
	}
	// Construct new elevator in terrain.
	g_Elevator.pos.y += g_Elevator.vertSpd;
	SPY = (int)(g_Elevator.pos.y / BLK_LGTH);

	for (int j = 1; j < ELEV_HEIGHT / BLK_LGTH; j++)
	{
		for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
		{
			SetTerrain(SPX + i, SPY + j, EARTH);
		}
	}
	for (int i = 0; i < ELEV_WIDTH / BLK_LGTH; i++)
	{
		SetTerrain(SPX + i, SPY, GROUND);
	}

	// Reverse elevator's speed when reaches the top/bottom
	if (g_Elevator.pos.y >= ELEV_BTM || g_Elevator.pos.y <= ELEV_TOP)
	{
		g_Elevator.vertSpd = -g_Elevator.vertSpd;
	}

#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Elevator X:%f Y:%f\n",
		g_Elevator.pos.x,
		g_Elevator.pos.y);
#endif
}

//
// @brief	draw elevator object
// @param	
// @return	
//
void DrawElev(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// Draw elevator
	{
		BG* s_Map = GetMap();
		float ex = g_Elevator.pos.x - s_Map->pos.x;	// プレイヤーの表示位置X
		float ey = g_Elevator.pos.y - s_Map->pos.y;	// プレイヤーの表示位置Y

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Elevator.texNo]);

		SetSpriteLTColor(g_VertexBuffer,
			ex, ey, g_Elevator.w, g_Elevator.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}

//
// @brief	get g_Elevator addres
// @param	
// @return	
//
ELEVATOR* GetElev(void)
{
	return &g_Elevator;
}
