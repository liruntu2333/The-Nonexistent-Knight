#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "terrain.h"
#include <math.h>
#include "player.h"
#include "bullet.h"
#include "collision.h"
#include "score.h"
#include "sound.h"

#define MAP_WIDTH				(800/16)	 
#define MAP_HEIGHT				(800/16)	
#define TEXTURE_MAX					(1)		 

#define TEXTURE_PATTERN_DIVIDE_X	(1)		 
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		 
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	 
#define ANIM_WAIT					(8)		 
#define PI							acos(-1)

void SetBullet(D3DXVECTOR3 pos);

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/bullet00.png"
};

static BOOL		g_Load = FALSE;			 
static BULLET	g_Bullet[BULLET_MAX];	 

HRESULT InitBullet(void)
{
	ID3D11Device* pDevice = GetDevice();

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

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use = FALSE;
		g_Bullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	 
		g_Bullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].w = MAP_WIDTH;
		g_Bullet[i].h = MAP_HEIGHT;
		g_Bullet[i].texNo = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = D3DXVECTOR3(4.0f, 0.0f, 0.0f);		 
	}

	g_Load = TRUE;	 

	return S_OK;
}

void UninitBullet()
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

void UpdateBullet(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)
		{
			g_Bullet[i].countAnim += 1.0f;
			if (g_Bullet[i].countAnim > ANIM_WAIT)
			{
				g_Bullet[i].countAnim = 0.0f;
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

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
						break;
					}
				}
			}
		}

#ifdef _DEBUG
#endif
	}
}

void DrawBullet(void)
{
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	SetWorldViewProjection2D();

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	Terrain* bg = GetTerrain();
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		 
		{									 
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			float px = g_Bullet[i].pos.x - bg->pos.x;	 
			float py = g_Bullet[i].pos.y - bg->pos.y;	 
			float pw = g_Bullet[i].w;		 
			float ph = g_Bullet[i].h;		 

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	 
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	 
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	 
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	 

			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			GetDeviceContext()->Draw(4, 0);
		}
	}
}

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
			return;
		}
	}
}

void DestructBullet(BULLET* bp)
{
	if (bp->use) bp->use = FALSE;
	return;
}