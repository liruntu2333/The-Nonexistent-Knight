//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "tutorial.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define MAP_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// テクスチャのファイル名
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/bg000.jpg",
	"data/TEXTURE/tex.png",
};


static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static TUTORIAL	g_Tutorial;



// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
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


	// 変数の初期化
	g_Tutorial.w = TEXTURE_WIDTH;
	g_Tutorial.h = MAP_HEIGHT;
	g_Tutorial.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Tutorial.texNo = 0;
	//g_Tutorial.move = 3.7f;
	g_Tutorial.offset = 0.0f;

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
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
// 描画処理
//=============================================================================
void DrawTutorial(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Tutorial.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,								//描画buffer
			g_Tutorial.pos.x, g_Tutorial.pos.y, g_Tutorial.w, g_Tutorial.h,			//表示座標
			g_Tutorial.offset, 0.0f, 1.0f, 1.0f,										//画像座標
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));							//色の各要素

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}





