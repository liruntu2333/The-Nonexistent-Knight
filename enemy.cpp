//=============================================================================
//
// プレイヤー処理 [ENEMY.cpp]
// Author : 
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include <math.h>
#include "player.h"
#include "collision.h"
#include "score.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(800/8)	// キャラサイズ
#define TEXTURE_HEIGHT				(800/8)	// 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(2)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値
#define PI							acos(-1)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DestructEnemy(ENEMY* ep);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static LINEAR_INTERPOLATION g_MoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ D3DXVECTOR3(50.0f,  50.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
	{ D3DXVECTOR3(250.0f,  50.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 3.14f),	D3DXVECTOR3(0.0f, 0.0f, 1.0f),	0.05f },
	{ D3DXVECTOR3(250.0f, 250.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(2.0f, 2.0f, 1.0f),	0.005f },
};


static LINEAR_INTERPOLATION g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ D3DXVECTOR3(1700.0f,   0.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
	{ D3DXVECTOR3(1700.0f,  SCREEN_HEIGHT, 0.0f),D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(2.0f, 2.0f, 1.0f),	0.01f },
};


static LINEAR_INTERPOLATION g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
	{ D3DXVECTOR3(3000.0f, 100.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 0.0f),		D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
	{ D3DXVECTOR3(3000 + SCREEN_WIDTH, 100.0f, 0.0f),	D3DXVECTOR3(0.0f, 0.0f, 6.28f),	D3DXVECTOR3(1.0f, 1.0f, 1.0f),	0.01f },
};

static LINEAR_INTERPOLATION* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,

};

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy01.png",
	"data/TEXTURE/enemy02.png",
	"data/TEXTURE/enemy03.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];	// プレイヤー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
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


	// エネミー構造体の初期化
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = TRUE;
		g_Enemy[i].pos = D3DXVECTOR3(50.0f + i * 150, 100.0f, 0.0f);	// 中心点から表示
		g_Enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w = TEXTURE_WIDTH;
		g_Enemy[i].h = TEXTURE_HEIGHT;
		g_Enemy[i].texNo = i;

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_Enemy[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);

		// 行動パターンを初期化
		g_Enemy[i].time = 0.0f;			// 線形補間用
		g_Enemy[i].moveTblNo = 0;			// データテーブル
		g_Enemy[i].tblMax = sizeof(g_MoveTbl0) / sizeof(LINEAR_INTERPOLATION);// 線形補間用

	}

	// ２番目の奴の行動パターンをセット
	g_Enemy[1].moveTblNo = 1;				// データテーブル
	g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(LINEAR_INTERPOLATION);	// 線形補間用

	// ３番目の奴の行動パターンをセット
	g_Enemy[2].moveTblNo = 2;				// データテーブル
	g_Enemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(LINEAR_INTERPOLATION);	// 線形補間用

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Enemy[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = g_Enemy[i].pos;

			// アニメーション  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			//static float s_enemy_move_x = 4.0f;
			//static float s_enemy_move_y = 4.0f;
			//g_Enemy[i].pos.x += s_enemy_move_x;
			//g_Enemy[i].pos.y += s_enemy_move_y;
			//if (g_Enemy[i].pos.x < 0 || g_Enemy[i].pos.x >(SCREEN_WIDTH - TEXTURE_WIDTH))
			//	s_enemy_move_x = -s_enemy_move_x;
			//if (g_Enemy[i].pos.y < 0 || g_Enemy[i].pos.y >(SCREEN_HEIGHT - TEXTURE_HEIGHT))
			//	s_enemy_move_y = -s_enemy_move_y;

			//static float s_enemy_move_x_1 = 2.0f;
			//static float s_enemy_move_y_2 = 4.0f;
			//static double s_enemy_move_t_3 = 0.0;
			//D3DXVECTOR3 pos = GetPlayer()->pos - g_Enemy[0].pos;
			////	float angle = atan2(GetPlayer()->pos.y - g_Enemy[0].pos.y, GetPlayer()->pos.x - g_Enemy[0].pos.x) + D3DX_PI;

			//switch (i)
			//{
			//case 0:
			//	//g_Enemy[i].pos.x += s_enemy_move_x_1;
			//	//if (g_Enemy[i].pos.x < 0 || g_Enemy[i].pos.x >(SCREEN_WIDTH - TEXTURE_WIDTH / 2))
			//	//	s_enemy_move_x_1 = -s_enemy_move_x_1;
			//	g_Enemy[i].pos += pos * 0.01f;
			//	//	g_Enemy[i].pos.x += static_cast<float> (cos(angle) * s_enemy_move_x_1);
			//	//	g_Enemy[i].pos.y += static_cast<float> (sin(angle) * s_enemy_move_x_1);

			//	break;
			//case 1:
			//	g_Enemy[i].pos.y += s_enemy_move_y_2;
			//	if (g_Enemy[i].pos.y - TEXTURE_HEIGHT / 2 < 0 || g_Enemy[i].pos.y > (SCREEN_HEIGHT - TEXTURE_HEIGHT / 2))
			//		s_enemy_move_y_2 = -s_enemy_move_y_2;
			//	break;
			//case 2:
			//	g_Enemy[i].pos.x = 500.0f + 100.0f * cos(s_enemy_move_t_3);
			//	g_Enemy[i].pos.y = 300.0f + 100.0f * sin(s_enemy_move_t_3);
			//	s_enemy_move_t_3 += PI / 30;
			//	break;
			//default:
			//	break;
			//}

			// 移動処理 
			{
				// 行動テーブルに従って座標移動（線形補間）
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				LINEAR_INTERPOLATION* tbl = g_MoveTblAdr[g_Enemy[i].moveTblNo];	// 行動テーブルのアドレスを取得
				D3DXVECTOR3	pos = tbl[nextNo].pos - tbl[nowNo].pos;	// XYZ移動量を計算している
				D3DXVECTOR3	rot = tbl[nextNo].rot - tbl[nowNo].rot;	// XYZ回転量を計算している
				D3DXVECTOR3	scl = tbl[nextNo].scl - tbl[nowNo].scl;	// XYZ拡大率を計算している
				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している
				pos *= nowTime;								// 現在の移動量を計算している
				rot *= nowTime;								// 現在の回転量を計算している
				scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				g_Enemy[i].pos = tbl[nowNo].pos + pos;

				// 計算して求めた回転量を現在の移動テーブルに足している
				g_Enemy[i].rot = tbl[nowNo].rot + rot;

				// 計算して求めた拡大率を現在の移動テーブルに足している
				g_Enemy[i].scl = tbl[nowNo].scl + scl;
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frameを使て時間経過処理をする
				g_Enemy[i].time += tbl[nowNo].time;			// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}
			}

			BG* bg = GetBG();

			if (g_Enemy[i].pos.x < 0.0f)
			{
				g_Enemy[i].pos.x = 0.0f;
			}

			if (g_Enemy[i].pos.x > bg->w)
			{
				g_Enemy[i].pos.x = bg->w;
			}

			if (g_Enemy[i].pos.y < 0.0f)
			{
				g_Enemy[i].pos.y = 0.0f;
			}

			if (g_Enemy[i].pos.y > bg->h)
			{
				g_Enemy[i].pos.y = bg->h;
			}

			for (int j = 0; j < PLAYER_MAX; j++)
			{
				if ((GetPlayer() + j)->use)
				{
					if (BBCollision(&(g_Enemy + i)->pos, &(GetPlayer() + j)->pos,
						(g_Enemy + i)->w, (GetPlayer() + j)->w,
						(g_Enemy + i)->h, (GetPlayer() + j)->h))
					{
						DestructEnemy(g_Enemy + i);
					}
				}
			}
		}

#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Enemy No%d  X:%f Y:%f\n", i, g_Enemy[i].pos.x, g_Enemy[i].pos.y);
#endif

	}

}


//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	BG* bg = GetBG();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Enemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Enemy[i].w;		// エネミーの表示幅
			float ph = g_Enemy[i].h;		// エネミーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f;	// テクスチャの幅
			//float th = 1.0f;	// テクスチャの高さ
			//float tx = 0.0f;	// テクスチャの左上X座標
			//float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// プレイヤー構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}

void DestructEnemy(ENEMY* ep)
{
	AddScore(100);
	PlaySound(SOUND_LABEL_SE_hit000);
	if (ep->use) ep->use = FALSE;
	return;
}