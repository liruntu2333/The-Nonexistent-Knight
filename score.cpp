//=============================================================================
//
// スコア処理 [score.cpp]
// Author : 
//
//=============================================================================
#include "score.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCORE_DW)	// 数字のサイズ
#define MAP_HEIGHT				(SCORE_DH)	// 
#define TEXTURE_MAX					(1)			// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/number16x32.png",
};

static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static SCORE	g_Score;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
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
	g_Score.w = TEXTURE_WIDTH;
	g_Score.h = MAP_HEIGHT;
	g_Score.pos = D3DXVECTOR3(SCORE_DX, SCORE_DY, 0.0f);
	g_Score.texNo = 0;
	g_Score.score = 0;	// 得点を初期化

	g_Load = TRUE;		// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateScore(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	//// 頂点バッファ設定
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//// マトリクス設定
	//SetWorldViewProjection2D();

	//// プリミティブトポロジ設定
	//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//// マテリアル設定
	//MATERIAL material;
	//ZeroMemory(&material, sizeof(material));
	//material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//SetMaterial(material);

	//// テクスチャ設定
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Score.texNo]);

	//// 桁数分処理する
	//int number = g_Score.score;

	//for (int i = 0; i < SCORE_DIGIT; i++)
	//{
	//	// 今回表示する桁の数字
	//	float x = (float)(number % 10);

	//	// 次の桁へ
	//	number /= 10;
	//	
	//	// スコアの位置やテクスチャー座標を反映
	//	float px = g_Score.pos.x - g_Score.w*i;	// スコアの表示位置X
	//	float py = g_Score.pos.y;				// スコアの表示位置Y
	//	float pw = g_Score.w;					// スコアの表示幅
	//	float ph = g_Score.h;					// スコアの表示高さ

	//	float tw = 1.0f / 10;					// テクスチャの幅
	//	float th = 1.0f / 1;					// テクスチャの高さ
	//	float tx = x * tw;						// テクスチャの左上X座標
	//	float ty = 0.0f;						// テクスチャの左上Y座標

	//	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//	SetSpriteLTColor(g_VertexBuffer,
	//		px, py, pw, ph,
	//		tx, ty, tw, th,
	//		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//	// ポリゴン描画
	//	GetDeviceContext()->Draw(4, 0);

	//}

}


//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
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