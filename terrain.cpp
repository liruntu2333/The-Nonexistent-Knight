#include "terrain.h"
#include "sprite.h"
#include "player.h"

#define MAP_WIDTH					(1600)	 
#define MAP_HEIGHT					(900)	

#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT*1/4)

#define TEXTURE_MAX					(2)				 
#define	SHAKE_FRAME					(60)			   
#define SHAKE_PD					(10)			   
#define SHAKE_AMP					(30.0f)			   

#define MAP_TEX_PATH				"data/TEXTURE/map.png"
#define BG_TEX_PATH					"data/TEXTURE/background.jpg"

static ID3D11Buffer* g_VertexBuffer = nullptr;
static ID3D11ShaderResourceView* gMapTexture = nullptr;	
static ID3D11ShaderResourceView* gBGTexture = nullptr;	

static BOOL	g_Load = FALSE;
static Terrain	g_Map;
static Terrain	g_BG;

static unsigned short int g_TerrainDT[MAP_HEIGHT / BLK_LGTH][MAP_WIDTH / BLK_LGTH] = { {0} };

HRESULT InitTerrain(void)
{
	gMapTexture = nullptr;
	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
		MAP_TEX_PATH,
		nullptr,
		nullptr,
		&gMapTexture,
		nullptr);

	gBGTexture = nullptr;
	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
		BG_TEX_PATH,
		nullptr,
		nullptr,
		&gBGTexture,
		nullptr);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	g_Map.w = MAP_WIDTH;
	g_Map.h = MAP_HEIGHT;
	g_Map.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Map.texNo = 0;
	g_Map.shake = FALSE;

	g_BG.w = SCREEN_WIDTH;
	g_BG.h = SCREEN_HEIGHT;
	g_BG.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 1;
	g_BG.shake = FALSE;

	for (int i = 0; i < MAP_WIDTH / BLK_LGTH; i++)
	{
		g_TerrainDT[MAP_HEIGHT / BLK_LGTH - 1][i] = 1;
		g_TerrainDT[MAP_HEIGHT / BLK_LGTH - 2][i] = 1;
		g_TerrainDT[MAP_HEIGHT / BLK_LGTH - 3][i] = 2;
		g_TerrainDT[0][i] = 2;
	}

	for (int i = 0; i < 26; i++)
	{
		g_TerrainDT[70][i] = 2;
		for (int j = 71; j < MAP_HEIGHT / BLK_LGTH; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}

	for (int i = 80; i < 80 + 26; i++)
	{
		g_TerrainDT[50][i] = 2;
		for (int j = 51; j < 51 + 5; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}
	for (int i = 115; i < 115 + 26; i++)
	{
		g_TerrainDT[40][i] = 2;
		for (int j = 41; j < 41 + 5; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < MAP_HEIGHT / BLK_LGTH; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}
	for (int i = 157; i < 159; i++)
	{
		for (int j = 0; j < MAP_HEIGHT / BLK_LGTH; j++)
		{
			g_TerrainDT[j][i] = 1;
		}
	}

	for (int i = 0; i < MAP_HEIGHT / BLK_LGTH; i++)
	{
		g_TerrainDT[i][MAP_WIDTH / BLK_LGTH] = 1;
	}

	g_Load = TRUE;	 
	return S_OK;
}

void UninitTerrain(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}

	if (gMapTexture)
	{
		gMapTexture->Release();
		gMapTexture = nullptr;
	}

	g_Load = FALSE;
}

void UpdateTerrain(void)
{
	PLAYER* s_Player = GetPlayer();
	g_Map.pos.x = s_Player->pos.x - PLAYER_DISP_X;
	if (g_Map.pos.x < 0) g_Map.pos.x = 0;
	if (g_Map.pos.x > g_Map.w - SCREEN_WIDTH) g_Map.pos.x = g_Map.w - SCREEN_WIDTH;

	g_Map.pos.y = s_Player->pos.y - PLAYER_DISP_Y;
	if (g_Map.pos.y < 0) g_Map.pos.y = 0;
	if (g_Map.pos.y > g_Map.h - SCREEN_HEIGHT) g_Map.pos.y = g_Map.h - SCREEN_HEIGHT;

	if (g_Map.shake)
	{
		if (g_Map.shake % SHAKE_PD < SHAKE_PD / 2)
		{
			g_Map.pos.x += SHAKE_AMP / SHAKE_FRAME * g_Map.shake;
			g_Map.pos.y += SHAKE_AMP / SHAKE_FRAME * g_Map.shake;
		}
		else
		{
			g_Map.pos.x -= SHAKE_AMP / SHAKE_FRAME * g_Map.shake;
			g_Map.pos.y -= SHAKE_AMP / SHAKE_FRAME * g_Map.shake;
		}

		g_Map.shake--;
	}

#ifdef _DEBUG

#endif
}

void DrawTerrain(void)
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

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &gMapTexture);

		SetSpriteLTColor(g_VertexBuffer,
			-g_Map.pos.x, -g_Map.pos.y, g_Map.w, g_Map.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}

Terrain* GetTerrain(void)
{
	return &g_Map;
}

int GetTerrain(float x, float y)
{
	return g_TerrainDT[(int)(y / BLK_LGTH)][(int)(x / BLK_LGTH)];
}

D3DXVECTOR3 ReloacteObj(float x, float y, float w, float h)
{
	int feetY = (int)((y + h / 2) / BLK_LGTH);
	int feetX = (int)(x / BLK_LGTH);
	while (true)
	{
		if (g_TerrainDT[feetY][feetX] == 2)
			break;
		feetY--;
	}
	return D3DXVECTOR3(x, (float)(feetY)*BLK_LGTH - h / 2, 0.0f);
}

void SetTerrain(int X, int Y, int terrain)
{
	g_TerrainDT[Y][X] = terrain;
}

void SetShake(int frame)
{
	g_Map.shake = frame;
}