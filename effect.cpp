//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "effect.h"
#include "sprite.h"
#include "main.h"
#include "bg.h"
#include "player.h"

#include <math.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define M_PI				3.14159265358979323846
#define ANIM_WAIT			2
#define EFFECT_PNG_W		2400
#define EFFECT_PNG_H		500

enum TEXTURE_INFO
{
	WIDTH,
	HEIGHT,
	DISTANCE,
	FRAME,
	INFO_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

static const int TEXTURE_MAX = 1;// テクスチャの数
static const float ACCELERATION = 0.05f;
static const int EFFECT_MAX = 100;
static const int PARTICLE_GEN = 1;

// WIDTH		HEIGHT		DISTANCE	FRAME
static const float c_TextureInfo[EFFECT_TYPE_MAX][INFO_MAX] =
{
	{200.0f,	100.0f,		100.0f,		4.0f},
	{100.0f,	100.0f,		50.0f,		5.0f},
	{600.0f,	300.0f,		50.0f,		4.0f},
};

static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

// テクスチャのファイル名
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/effect.png",
};


static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static EFFECT	g_Effect[EFFECT_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
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
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		(g_Effect + i)->use = FALSE;
	}

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
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
void UpdateEffect(void)
{
	// Particle rain which currently been deprecated.
	/*for (int i = 0; i < EFFECT_MAX; i++)
	{
		int s_count = 0;
		if (!g_Effect[i].use)
		{
			g_Effect[i].pos = D3DXVECTOR3(GetPlayer()->pos.x, 0.0f, 0.0f);
			g_Effect[i].velocity_x = ((float)(rand() % 100) / 100);
			if (rand() % 2) g_Effect[i].velocity_x = -g_Effect[i].velocity_x;
			g_Effect[i].velocity_y = 0.0f;
			g_Effect[i].use = FALSE;
			s_count++;
		}
		if (s_count == PARTICLE_GEN) break;
	}
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (g_Effect[i].use)
		{
			g_Effect[i].pos += D3DXVECTOR3(g_Effect[i].velocity_x, g_Effect[i].velocity_y, 0);
			g_Effect[i].velocity_y += ACCELERATION;
			if (g_Effect[i].pos.y >= GetBG()->h)
			{
				g_Effect[i].use = FALSE;
			}
		}
	}*/

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		EFFECT* s_Effect = g_Effect + i;
		//	Only proceed when effect been using
		if (s_Effect->use)
		{
			// Animation
			if (++s_Effect->countAnim > ANIM_WAIT)
			{
				s_Effect->countAnim = 0.0f;
				// Change pattern and check if it's the end of animation.
				if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
				{
					s_Effect->use = FALSE;
				};
			}

#ifdef _DEBUG
			// デバッグ表示
			PrintDebugProc("X:%f Y:%f texNo: %d patternAnim: %d \n",
				s_Effect->pos.x,
				s_Effect->pos.y,
				s_Effect->texNo,
				s_Effect->patternAnim);
#endif
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(void)
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

	//
	// draw additive color mixing effect
	//
	//SetBlendState(BLEND_MODE_ADD);
	//SetBlendState(BLEND_MODE_SUBTRACT);
	BG* s_BG = GetBG();

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		EFFECT* s_Effect = g_Effect + i;
		if (s_Effect->use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			// Calculate the relative loacation of effect
			float ex = s_Effect->pos.x - s_BG->pos.x;
			float ey = s_Effect->pos.y - s_BG->pos.y;

			// Calculate the parameter for animation
			float tw = 1.0f / EFFECT_PNG_W * s_Effect->w;	
			float th = 1.0f / EFFECT_PNG_H * s_Effect->h;	
			float tx = (float)s_Effect->patternAnim * tw;
			float sumH = 0.0f;
			for (int i = 0; i < s_Effect->type; i++)
			{
				sumH += c_TextureInfo[i][HEIGHT];
			}
			float ty = 1.0f / EFFECT_PNG_H * sumH;
			SetSpriteColorRotation(g_VertexBuffer, ex, ey, s_Effect->w, s_Effect->h,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 
				s_Effect->rot.z);

			GetDeviceContext()->Draw(4, 0);
		}
	}
	//SetBlendState(BLEND_MODE_ALPHABLEND);

}

//
// @brief	Initiate a effect if avaliable.
// @param	Location XY, Type, orient(0123).
// @return	Successfully initiated EFFECT or NULL.
//
EFFECT* SetEffect(float X, float Y, int Type, int orient)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		EFFECT* s_Effect = g_Effect + i;
		if (!s_Effect->use)
		{
			s_Effect->type = Type;
			s_Effect->rot = D3DXVECTOR3(0.0f, 0.0f, (float)M_PI / 2 * orient);
			float eX = X + c_TextureInfo[Type][DISTANCE] * (float)cos(s_Effect->rot.z);
			float eY = Y + c_TextureInfo[Type][DISTANCE] * (float)sin(s_Effect->rot.z);
			s_Effect->pos = D3DXVECTOR3(eX, eY, 0.0f);
			s_Effect->w = c_TextureInfo[Type][WIDTH];
			s_Effect->h = c_TextureInfo[Type][HEIGHT];
			s_Effect->countAnim = 0.0f;
			s_Effect->patternAnim = 0;
			s_Effect->texNo = 0;
			s_Effect->use = TRUE;
			return s_Effect;
		}
	}
	return FALSE;
}



