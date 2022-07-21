#include "result.h"
#include "sprite.h"
#include "input.h"
#include "score.h"
#include "fade.h"

#define TEXTURE_WIDTH				(SCREEN_WIDTH)	 
#define MAP_HEIGHT				(SCREEN_HEIGHT)	
#define TEXTURE_MAX					(2)				 

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result.png",
	"data/TEXTURE/number16x32.png",
};

static BOOL		g_Load = FALSE;		 
static RESULT	g_Result;

static int		g_ResultScore = 0;

HRESULT InitResult(void)
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

	g_Result.w = TEXTURE_WIDTH;
	g_Result.h = MAP_HEIGHT;
	g_Result.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Result.texNo = 0;

	g_Load = TRUE;	 
	return S_OK;
}

void UninitResult(void)
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

void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{ 
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
}

void DrawResult(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Result.texNo]);

		SetSpriteLTColor(g_VertexBuffer,
			g_Result.pos.x, g_Result.pos.y, g_Result.w, g_Result.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}

void SetResult(int score)
{
	g_ResultScore = score;
}