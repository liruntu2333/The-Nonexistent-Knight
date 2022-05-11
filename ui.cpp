//
// @file				ui.cpp
// @brief				Set & present UI at game mode.
// @copyright			2021 LI ZIZHEN liruntu2333@gmail.com
// @lisense				GPL
//

#include "main.h"
#include "renderer.h"
#include "sprite.h"
#include "player.h"
#include "enemy.h"
#include "map.h"
#include "ui.h"

//
//	Macros
//
#define TEXTURE_MAX				(7)
#define HEART_MAX				PLAYER_HEALTH_MAX
#define COIN_MAX				1
#define STAMINA_BAR_MAX			2
#define MONEY_MAX				1

#define HEART_FULL_TEX_NO			0
#define HEART_EMPTY_TEX_NO			1
#define HEART_START					D3DXVECTOR3(50.0f, 30.0f, 0.0f)		// The point where No.1 heart present
#define HEART_LENGTH				50.0f
#define HEART_INTERVAL				10.0f

#define STA_FULL_TEX_NO				2
#define STA_EMPTY_TEX_NO			3
#define	STA_BAR_START				(HEART_START + D3DXVECTOR3(0.0f, HEART_LENGTH + 10.0f, 0.0f))		// The point where stamina bar present
#define	STA_BAR_W					400.0f
#define	STA_BAR_H					30.0f

#define COIN_START					(STA_BAR_START + D3DXVECTOR3(0.0f, STA_BAR_H + 10.0f, 0.0f))
#define COIN_LENGTH					50.0f
#define COIN_TEX_NO					4

#define MONEY_START					(COIN_START + D3DXVECTOR3(COIN_LENGTH + 10.0f + MONEY_DW * MONEY_DIGIT, 0.0f, 0.0f))
#define MONEY_TEX_NO				5
#define MONEY_DW					25.0f
#define	MONEY_DH					50.0f
#define MONEY_DIGIT					3

#define ENEMY_BAR_MAX				ENEMY_MAX
#define ENM_BAR_TEX_NO				6
#define	ENM_BAR_W					50.0f
#define	ENM_BAR_H					10.0f

//
//	Function Prototype Declarations
//

//
//	Global Variables
//
static ID3D11Buffer* g_VertexBuffer = nullptr;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
		"data/TEXTURE/heart.png",
		"data/TEXTURE/heart_empty.png",
		"data/TEXTURE/stamina_full.png",
		"data/TEXTURE/stamina_empty.png",
		"data/TEXTURE/coin.png",
		"data/TEXTURE/number.png",
		"data/TEXTURE/enemy_bar.png",
};

static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

static ICON	g_Heart[HEART_MAX];
static ICON	g_StaminaBar[STAMINA_BAR_MAX];
static ICON g_Coin[COIN_MAX];
static ICON g_Money[MONEY_MAX];
static ICON g_EnmBar[ENEMY_BAR_MAX];

//
// @brief	initiate ui object
// @param
// @return
//
HRESULT InitUI(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			nullptr,
			nullptr,
			&g_Texture[i],
		nullptr);
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	for (int i = 0; i < HEART_MAX; i++)
	{
		ICON* s_Heart = g_Heart + i;

		s_Heart->pos = HEART_START + D3DXVECTOR3((HEART_LENGTH + HEART_INTERVAL) * i, 0.0f, 0.0f);
		s_Heart->w = s_Heart->h = HEART_LENGTH;
		s_Heart->texNo = HEART_FULL_TEX_NO;
		s_Heart->use = TRUE;
	}

	for (int i = 0; i < STAMINA_BAR_MAX; i++)
	{
		ICON* s_Bar = g_StaminaBar + i;

		s_Bar->pos = STA_BAR_START;
		s_Bar->w = STA_BAR_W;
		s_Bar->h = STA_BAR_H;
		s_Bar->texNo = STA_FULL_TEX_NO + i;
		s_Bar->use = TRUE;
	}

	for (int i = 0; i < COIN_MAX; i++)
	{
		ICON* s_Coin = g_Coin + i;

		s_Coin->pos = COIN_START;
		s_Coin->w = s_Coin->h = COIN_LENGTH;
		s_Coin->texNo = COIN_TEX_NO;
		s_Coin->use = TRUE;
	}

	for (int i = 0; i < MONEY_MAX; i++)
	{
		ICON* s_Money = g_Money + i;

		s_Money->pos = MONEY_START;
		s_Money->w = MONEY_DW;
		s_Money->h = MONEY_DH;
		s_Money->texNo = MONEY_TEX_NO;
		s_Money->use = TRUE;
	}

	for (int i = 0; i < ENEMY_BAR_MAX; i++)
	{
		ICON* s_EnmBar = g_EnmBar + i;
		s_EnmBar->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		s_EnmBar->w = ENM_BAR_W;
		s_EnmBar->h = ENM_BAR_H;
		s_EnmBar->texNo = ENM_BAR_TEX_NO;
		s_EnmBar->use = FALSE;
	}

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//
// @brief	destruct elevator object
// @param
// @return
//
void UninitUI(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = nullptr;
		}
	}

	g_Load = FALSE;
}

//
// @brief	update ui object
// @param
// @return
//
void UpdateUI(void)
{
	int s_Health = GetPlayer()->health;
	int s_Stamina = GetPlayer()->stamina;
	for (int i = 0; i < HEART_MAX; i++)
	{
		ICON* s_Heart = g_Heart + i;
		s_Heart->texNo = (i < s_Health) ? HEART_FULL_TEX_NO : HEART_EMPTY_TEX_NO;

#ifdef _DEBUG
		// デバッグ表示
	/*	PrintDebugProc("Heart No. %d x:%f y: %f use: %d TexNo: %d\n", i, s_Heart->pos.x, s_Heart->pos.y, s_Heart->use, s_Heart->texNo);*/
#endif
	}

	for (int i = 0; i < STAMINA_BAR_MAX; i++)
	{
		ICON* s_Bar = g_StaminaBar + i;
		if (s_Bar->texNo == STA_FULL_TEX_NO)
		{
			s_Bar->w = STA_BAR_W * s_Stamina / PLAYER_STAMINA_MAX;
		}
	}

	for (int i = 0; i < ENEMY_BAR_MAX; i++)
	{
		ICON* s_EnmBar = g_EnmBar + i;
		ENEMY* s_Enemy = GetEnemy() + i;
		if (s_Enemy->use)
		{
			s_EnmBar->pos = s_Enemy->pos - D3DXVECTOR3(s_Enemy->w / 2 - 25.0f, s_Enemy->h / 2 + 5.0f, 0.0f);
			s_EnmBar->w = ENM_BAR_W * s_Enemy->health / ENEMY_HEALTH_MAX;
			s_EnmBar->use = TRUE;
		}
		else
		{
			s_EnmBar->use = FALSE;
		}
	}
}

//
// @brief	draw ui object
// @param
// @return
//
void DrawUI(void)
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

	// Draw Heart
	for (int i = 0; i < HEART_MAX; i++)
	{
		ICON* s_Heart = g_Heart + i;
		if (s_Heart->use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Heart->texNo]);

			SetSpriteLTColor(g_VertexBuffer,
				s_Heart->pos.x, s_Heart->pos.y, s_Heart->w, s_Heart->h,
				0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	// Draw Stamina Bar
	for (int i = 0; i < STAMINA_BAR_MAX; i++)
	{
		ICON* s_Bar = g_StaminaBar + i;
		if (s_Bar->texNo == STA_FULL_TEX_NO)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Bar->texNo]);

			SetSpriteLTColor(g_VertexBuffer,
				s_Bar->pos.x, s_Bar->pos.y, s_Bar->w, s_Bar->h,
				0.0f, 0.0f, s_Bar->w / STA_BAR_W, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
		else
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Bar->texNo]);

			SetSpriteLTColor(g_VertexBuffer,
				s_Bar->pos.x, s_Bar->pos.y, s_Bar->w, s_Bar->h,
				0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	// Draw Coin
	for (int i = 0; i < COIN_MAX; i++)
	{
		ICON* s_Coin = g_Coin + i;
		if (s_Coin->use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Coin->texNo]);

			SetSpriteLTColor(g_VertexBuffer,
				s_Coin->pos.x, s_Coin->pos.y, s_Coin->w, s_Coin->h,
				0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	// Draw Money
	int money = GetPlayer()->money;
	for (int i = 0; i < MONEY_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(money % 10);

		// 次の桁へ
		money /= 10;

		// スコアの位置やテクスチャー座標を反映
		float px = g_Money->pos.x - g_Money->w * i;	// スコアの表示位置X
		float py = g_Money->pos.y;				// スコアの表示位置Y
		float pw = g_Money->w;					// スコアの表示幅
		float ph = g_Money->h;					// スコアの表示高さ

		float tw = 1.0f / 10;					// テクスチャの幅
		float th = 1.0f / 1;					// テクスチャの高さ
		float tx = x * tw;						// テクスチャの左上X座標
		float ty = 0.0f;						// テクスチャの左上Y座標

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Money->texNo]);
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// Draw enemy health bar
	for (int i = 0; i < ENEMY_BAR_MAX; i++)
	{
		ICON* s_EnmBar = g_EnmBar + i;
		if (s_EnmBar->use)
		{
			BG* s_Map = GetMap();
			float ex = s_EnmBar->pos.x - s_Map->pos.x;	// relative position
			float ey = s_EnmBar->pos.y - s_Map->pos.y;	// relative position

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_EnmBar->texNo]);

			SetSpriteLTColor(g_VertexBuffer,
				ex, ey, s_EnmBar->w, s_EnmBar->h,
				0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}
}