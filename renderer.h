#pragma once

#include "main.h"

#define LIGHT_MAX		(10)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		
	LIGHT_TYPE_DIRECTIONAL,	
	LIGHT_TYPE_POINT,		

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		
	BLEND_MODE_ALPHABLEND,	
	BLEND_MODE_ADD,			
	BLEND_MODE_SUBTRACT,	

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			
	CULL_MODE_FRONT,		
	CULL_MODE_BACK,			

	CULL_MODE_NUM
};

struct VERTEX_3D
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXCOLOR  Diffuse;
	D3DXVECTOR2 TexCoord;
};

struct MATERIAL
{
	D3DXCOLOR	Ambient;
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Specular;
	D3DXCOLOR	Emission;
	float		Shininess;
	int			noTexSampling;
};

struct LIGHT {
	D3DXVECTOR3 Direction;	 
	D3DXVECTOR3 Position;	 
	D3DXCOLOR	Diffuse;	 
	D3DXCOLOR   Ambient;	 
	float		Attenuation; 
	int			Type;		 
	int			Enable;		 
};

struct FOG {
	float		FogStart;	 
	float		FogEnd;		 
	D3DXCOLOR	FogColor;	 
};

HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device* GetDevice(void);
ID3D11DeviceContext* GetDeviceContext(void);

void SetDepthEnable(bool Enable);
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);
void SetAlphaTestEnable(BOOL flag);

void SetWorldViewProjection2D(void);
void SetWorldMatrix(D3DXMATRIX* WorldMatrix);
void SetViewMatrix(D3DXMATRIX* ViewMatrix);
void SetProjectionMatrix(D3DXMATRIX* ProjectionMatrix);

void SetMaterial(MATERIAL material);

void SetLightEnable(BOOL flag);
void SetLight(int index, LIGHT* light);

void SetFogEnable(BOOL flag);
void SetFog(FOG* fog);

void DebugTextOut(char* text, int x, int y);

void SetFuchi(int flag);
void SetShaderCamera(D3DXVECTOR3 pos);
