//
//	background.cpp
//	author : LI ZIZHEN liruntu2333@gmail.com
//
#include "background.h"
#include "sprite.h"
#include "input.h"
#include "fade.h"
#include "main.h"

#include <math.h>
//*****************************************************************************
// MACROS
//*****************************************************************************
#define TEXTURE_MAX					1

//*****************************************************************************
// GLOBALS
//*****************************************************************************
static ID3D11Buffer* gVertexBuffer = nullptr;		// 頂点情報
static ID3D11ShaderResourceView* gTexture[TEXTURE_MAX] = { nullptr };	// テクスチャ情報

static Background gBackground;
static BOOL		gLoad = FALSE;

// texture path
static char* gTextureName[TEXTURE_MAX] = {
	"data/TEXTURE/background.jpg",
};

/**
 * \brief Initialize texture resources, buffers and Singleton.
 * \return HRESULT S_OK if initialized properly.
 */
HRESULT InitBackground()
{
	ID3D11Device* pDevice = GetDevice();

	// Initialize texture.
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		gTexture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			gTextureName[i],
			nullptr,
			nullptr,
			&gTexture[i],
			nullptr);
	}

	// Initialize Vertex Buffer.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &gVertexBuffer);

	gBackground.w = SCREEN_WIDTH;
	gBackground.h = SCREEN_HEIGHT;
	gBackground.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	gBackground.texNo = 0;
	gBackground.shake = FALSE;

	gLoad = TRUE;
	return S_OK;
}

void UninitBackground()
{
	if (!gLoad) return;

	if (gVertexBuffer)
	{
		gVertexBuffer->Release();
		gVertexBuffer = nullptr;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (gTexture[i])
		{
			gTexture[i]->Release();
			gTexture[i] = nullptr;
		}
	}

	gLoad = FALSE;
}

void UpdateBackground()
{

}

/**
 * \brief Draw background in game play.
 */
void DrawBackground()
{
	const UINT stride = sizeof(VERTEX_3D);
	const UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &gVertexBuffer, &stride, &offset);

	SetWorldViewProjection2D();

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// Draw Background
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &gTexture[gBackground.texNo]);

		SetSpriteLTColor(gVertexBuffer,
			0.0f, 0.0f, gBackground.w, gBackground.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}