#include "score.h"
#include "sprite.h"

#define TEXTURE_WIDTH				(SCORE_DW)	 
#define TEXTURE_HEIGHT				(SCORE_DH)	
#define TEXTURE_MAX					(1)			 

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[] = {
	"data/TEXTURE/number16x32.png",
};

static BOOL		g_Load = FALSE;		 
static SCORE	g_Score;

HRESULT InitScore(void)
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

	g_Score.w = TEXTURE_WIDTH;
	g_Score.h = TEXTURE_HEIGHT;
	g_Score.pos = D3DXVECTOR3(SCORE_DX, SCORE_DY, 0.0f);
	g_Score.texNo = 0;
	g_Score.score = 0;	 

	g_Load = TRUE;		 
	return S_OK;
}

void UninitScore(void)
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

void UpdateScore(void)
{
}

void DrawScore(void)
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

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Score.texNo]);

	int number = g_Score.score;

	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		float x = (float)(number % 10);

		number /= 10;

		float px = g_Score.pos.x - g_Score.w * i;	 
		float py = g_Score.pos.y;				 
		float pw = g_Score.w;					 
		float ph = g_Score.h;					 

		float tw = 1.0f / 10;					 
		float th = 1.0f / 1;					 
		float tx = x * tw;						 
		float ty = 0.0f;						 

		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}

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