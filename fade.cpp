//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : 
//
//=============================================================================
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(1)				// テクスチャの数

#define	FADE_RATE					(0.02f)			// フェード係数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void SetVertex(float X, float Y, float Width, float Height, float U, float V, float UW, float VH);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/fade_black.png",
};

static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static FADE		g_Fade;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
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
	g_Fade.w = TEXTURE_WIDTH;
	g_Fade.h = TEXTURE_HEIGHT;
	g_Fade.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Fade.texNo = 0;


	g_Fade.flag  = FADE_IN;
	g_Fade.nextMode = MODE_END;
	g_Fade.color = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateFade(void)
{

	if (g_Fade.flag != FADE_NONE)
	{// フェード処理中
		if (g_Fade.flag == FADE_OUT)
		{// フェードアウト処理
			g_Fade.color.a += FADE_RATE;		// α値を加算して画面を消していく
			if (g_Fade.color.a >= 1.0f)
			{
				// フェードイン処理に切り替え
				g_Fade.color.a = 1.0f;
				SetFade(FADE_IN, g_Fade.nextMode);

				// モードを設定
				SetMode(g_Fade.nextMode);
			}

		}
		else if (g_Fade.flag == FADE_IN)
		{// フェードイン処理
			g_Fade.color.a -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
			if (g_Fade.color.a <= 0.0f)
			{
				// フェード処理終了
				g_Fade.color.a = 0.0f;
				SetFade(FADE_NONE, g_Fade.nextMode);
			}

		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (g_Fade.flag == FADE_NONE) return;	// フェードしないのなら描画しない

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

	// フェード用ポリゴンを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Fade.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetVertex(g_Fade.pos.x, g_Fade.pos.y, g_Fade.w, g_Fade.h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


}


//=============================================================================
// 頂点データ設定
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
// フェードの状態設定
//=============================================================================
void SetFade(int fade, int nextMode)
{
	g_Fade.flag = fade;
	g_Fade.nextMode = nextMode;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
int GetFade(void)
{
	return g_Fade.flag;
}



