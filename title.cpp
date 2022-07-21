#include "title.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "file.h"
#include "main.h"

#include <math.h>

static const float TEXTURE_WIDTH = SCREEN_WIDTH; 
static const float MAP_HEIGHT = SCREEN_HEIGHT;
static const int TEXTURE_MAX = 4; 

static const int LOGO_MAX = 2;
static const float LOGO_WIDTH = TEXTURE_WIDTH / 2;
static const float LOGO_HEIGHT = MAP_HEIGHT / 2;
static const int LOGO_DIVIDE_X = 100;
static const int LOGO_DIVIDE_Y = 50;

static const float MOSAIC_WIDTH = 100.0f;
static const float MOSAIC_HEIGHT = 100.0f;
static const int MOSAIC_DIVIDE_X = 50;
static const int MOSAIC_DIVIDE_Y = 50;
static const int MOSAIC_SAMPLING_INTVL = 3;

#define M_PI 3.14159265358979323846
static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/background.jpg",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/title_logo.png",
};

static BOOL		g_Load = FALSE;		 
static TITLE	g_Title, g_Logo[LOGO_MAX], g_Mosaic;

static double effect_dx, effect_dy, effect_dt;      
static const double effect_ddt = 2 * M_PI / 100;

HRESULT InitTitle(void)
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

	g_Title.w = TEXTURE_WIDTH;
	g_Title.h = MAP_HEIGHT;
	g_Title.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Title.texNo = 1;
	g_Title.offset = 0.0f;

	for (size_t i = 0; i < LOGO_MAX; i++)
	{
		g_Logo[i].w = LOGO_WIDTH;
		g_Logo[i].h = LOGO_HEIGHT;
		g_Logo[i].pos = D3DXVECTOR3(TEXTURE_WIDTH / 4, MAP_HEIGHT / LOGO_MAX * i, 0.0f);
		g_Logo[i].texNo = 3;
	}

	g_Mosaic.w = MOSAIC_WIDTH;
	g_Mosaic.h = MOSAIC_HEIGHT;
	g_Mosaic.pos = g_Logo[1].pos;
	g_Mosaic.texNo = 3;

	effect_dx = 100.0f;
	effect_dy = 100.0f;
	effect_dt = 0.0f;

	g_Load = TRUE;	 
	return S_OK;
}

void UninitTitle(void)
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

void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}

	g_Title.offset += 0.001f;

	float speed = 4.0f;

	if (GetKeyboardPress(DIK_S))
	{
		effect_dy += speed;
		g_Mosaic.pos.y += speed;
	}
	if (GetKeyboardPress(DIK_W))
	{
		effect_dy -= speed;
		g_Mosaic.pos.y -= speed;
	}
	if (GetKeyboardPress(DIK_D))
	{
		effect_dx += speed;
		g_Mosaic.pos.x += speed;
	}
	if (GetKeyboardPress(DIK_A))
	{
		effect_dx -= speed;
		g_Mosaic.pos.x -= speed;
	}

	effect_dt += effect_ddt;

	if (GetKeyboardTrigger(DIK_L))
	{
		SetLoadGame(TRUE);
		SetFade(FADE_OUT, MODE_GAME);
	}

}

void DrawTitle(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		SetSpriteLTColor(g_VertexBuffer,								
			g_Title.pos.x, g_Title.pos.y, g_Title.w, g_Title.h,			
			g_Title.offset, 0.0f, 1.0f, 1.0f,										
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));							

		GetDeviceContext()->Draw(4, 0);
	}

	static const float Logo_div_xw = g_Logo[0].w / LOGO_DIVIDE_X;
	static const float Logo_div_yh = g_Logo[0].h / LOGO_DIVIDE_Y;

	static const float Logo_div_uw = 1.0f / LOGO_DIVIDE_X;
	static const float Logo_div_vh = 1.0f / LOGO_DIVIDE_Y;

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Logo[0].texNo]);
	for (int i = 0; i < LOGO_DIVIDE_Y; i++)
	{
		double phase_x = 2 * M_PI / LOGO_DIVIDE_Y * i + effect_dt;

		for (int j = 0; j < LOGO_DIVIDE_X; j++)
		{
			double phase_y = 2 * M_PI / LOGO_DIVIDE_X * j + effect_dt;

			SetSpriteLTColor(g_VertexBuffer,							 
				(float)(g_Logo[0].pos.x + sin(phase_x) * effect_dx / 10 + Logo_div_xw * j),			 
				(float)(g_Logo[0].pos.y + sin(phase_y) * effect_dx / 10 + Logo_div_yh * i),			 
				Logo_div_xw,										 
				Logo_div_yh,										 
				Logo_div_uw * j,								 
				Logo_div_vh * i,								 
				Logo_div_uw,									 
				Logo_div_vh,									 
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.2f - (FLOAT)effect_dy / 500.0f));	
			GetDeviceContext()->Draw(4, 0);
		}

	}

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Logo[1].texNo]);

	SetSpriteLTColor(g_VertexBuffer,										
		g_Logo[1].pos.x, g_Logo[1].pos.y, g_Logo[1].w, g_Logo[1].h,			
		g_Logo[1].offset, 0.0f, 1.0f, 1.0f,									
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));									

	GetDeviceContext()->Draw(4, 0);

	static const float Mosaic_div_xw = g_Mosaic.w / MOSAIC_DIVIDE_X;
	static const float Mosaic_div_yh = g_Mosaic.h / MOSAIC_DIVIDE_Y;

	static const float Mosaic_div_uw = 1.0f * MOSAIC_WIDTH / LOGO_WIDTH / MOSAIC_DIVIDE_X;
	static const float Mosaic_div_vh = 1.0f * MOSAIC_HEIGHT / LOGO_HEIGHT / MOSAIC_DIVIDE_Y;

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Mosaic.texNo]);
	for (int i = 0; i < MOSAIC_DIVIDE_Y; i++)
	{
		float Mosaic_y = g_Mosaic.pos.y + i * Mosaic_div_yh;
		int v_range = i / MOSAIC_SAMPLING_INTVL;

		for (int j = 0; j < MOSAIC_DIVIDE_X; j++)
		{
			float Mosaic_x = g_Mosaic.pos.x + j * Mosaic_div_xw;

			if (Mosaic_x >= g_Logo[1].pos.x && Mosaic_x <= g_Logo[1].pos.x + g_Logo[1].w &&
				Mosaic_y >= g_Logo[1].pos.y && Mosaic_y <= g_Logo[1].pos.y + g_Logo[1].h)
			{
				int u_range = j / MOSAIC_SAMPLING_INTVL;

				float Mosaic_u = (g_Mosaic.pos.x - g_Logo[1].pos.x) / LOGO_WIDTH +		        
					u_range * MOSAIC_SAMPLING_INTVL * Mosaic_div_uw +					     
					rand() % MOSAIC_SAMPLING_INTVL * Mosaic_div_uw;						    
				float Mosaic_v = (g_Mosaic.pos.y - g_Logo[1].pos.y) / LOGO_HEIGHT +		        
					v_range * MOSAIC_SAMPLING_INTVL * Mosaic_div_vh +					     
					rand() % MOSAIC_SAMPLING_INTVL * Mosaic_div_vh;						    

				SetSpriteLTColor(g_VertexBuffer,						 
					Mosaic_x,											 
					Mosaic_y,											 
					Mosaic_div_xw,										 
					Mosaic_div_yh,										 
					Mosaic_u,											 
					Mosaic_v,											 
					Mosaic_div_uw,										 
					Mosaic_div_vh,										 
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));					
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
}