//=============================================================================
//
// プレイヤー処理 [ENEMY.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "map.h"
#include <math.h>
#include "player.h"
#include "bullet.h"
#include "collision.h"
#include "score.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAP_WIDTH				(800/16)	// キャラサイズ
#define MAP_HEIGHT				(800/16)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値
#define PI							acos(-1)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetBullet(D3DXVECTOR3 pos);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bullet00.png"
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// プレイヤー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;
		g_Bullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 中心点から表示
		g_Bullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].w = MAP_WIDTH;
		g_Bullet[i].h = MAP_HEIGHT;
		g_Bullet[i].texNo = 0;
	

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// 移動量

	}

	g_Load = TRUE;	// データの初期化を行った

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet()
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
void UpdateBullet(void)
{
	//if (GetKeyboardTrigger(DIK_K))
	//{
	//	SetBullet(GetPlayer()->pos);
	//}


	for (int i = 0; i < BULLET_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Bullet[i].use == TRUE)
		{
			// アニメーション  
			g_Bullet[i].countAnim += 1.0f;
			if (g_Bullet[i].countAnim > ANIM_WAIT)
			{
				g_Bullet[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//static float s_enemy_move_x = 4.0f;
			//static float s_enemy_move_y = 4.0f;
			//g_Bullet[i].pos.x += s_enemy_move_x;
			//g_Bullet[i].pos.y += s_enemy_move_y;
			//if (g_Bullet[i].pos.x < 0 || g_Bullet[i].pos.x >(SCREEN_WIDTH - TEXTURE_WIDTH))
			//	s_enemy_move_x = -s_enemy_move_x;
			//if (g_Bullet[i].pos.y < 0 || g_Bullet[i].pos.y >(SCREEN_HEIGHT - TEXTURE_HEIGHT))
			//	s_enemy_move_y = -s_enemy_move_y;


			//	float angle = atan2(GetPlayer()->pos.y - g_Bullet[0].pos.y, GetPlayer()->pos.x - g_Bullet[0].pos.x) + D3DX_PI;


			g_Bullet[i].pos += g_Bullet[i].move;

			PLAYER* player = GetPlayer();
			if (g_Bullet[i].pos.x > player->pos.x + SCREEN_WIDTH)
			{
				DestructBullet(g_Bullet + i);
			}


			for (int j = 0; j < ENEMY_MAX; j++)
			{
				ENEMY* ep = GetEnemy() + j;
				if (ep->use)
				{
					if (BBCollision(&(g_Bullet + i)->pos, &ep->pos, g_Bullet[i].w, ep->w, g_Bullet[i].h, ep->h))
					{
						//DestructEnemy(ep);
						break;
					}
				}
			}

		}

#ifdef _DEBUG
		// デバッグ表示
		//PrintDebugProc("Bullet No%d  X:%f Y:%f\n", i, g_Bullet[i].pos.x, g_Bullet[i].pos.y);
#endif

	}

}


//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	BG *bg = GetMap();
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = g_Bullet[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = g_Bullet[i].w;		// プレイヤーの表示幅
			float ph = g_Bullet[i].h;		// プレイヤーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			//float tx = g_Bullet[i].patternAnim * tw;	// テクスチャの左上X座標
			//float ty = g_Bullet[i].patternAnim * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// プレイヤー構造体の先頭アドレスを取得
//=============================================================================
BULLET* GetBullet(void)
{

	return &g_Bullet[0];
}


void SetBullet(D3DXVECTOR3 pos)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)
		{
			g_Bullet[i].use = TRUE;
			g_Bullet[i].pos = pos + D3DXVECTOR3(MAP_WIDTH, 0.0F, 0.0F);
			PlaySound(SOUND_LABEL_SE_laser000);
			return;
		}
	}


}

void DestructBullet(BULLET* bp)
{
	if (bp->use) bp->use = FALSE;
	return;
}

