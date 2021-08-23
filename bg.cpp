//=============================================================================
//
// BG画面処理 [bg.cpp]
// Author : 
//
//=============================================================================
#include "bg.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(1600)	// 背景サイズ
#define TEXTURE_HEIGHT				(900)	// 
#define TEXTURE_MAX					(2)				// テクスチャの数
#define BLOCK_LENGTH				(10)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/earth.png",
	"data/TEXTURE/map.jpg",
};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BG	g_BG;

static int g_BGData[TEXTURE_HEIGHT / BLOCK_LENGTH][TEXTURE_WIDTH / BLOCK_LENGTH] = { {0} };


// 初期化処理
//=============================================================================
HRESULT InitBG(void)
{
	ID3D11Device *pDevice = GetDevice();

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


	// 変数の初期化
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
	

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateBG(void)
{


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			-g_BG.pos.x, -g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
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