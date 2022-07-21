#pragma once

#include "main.h"
#include "renderer.h"

void SetSpriteLTColor(ID3D11Buffer* buf,
	float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,
	D3DXCOLOR color);

void SetSprite(ID3D11Buffer* buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH);

void SetSpriteColor(ID3D11Buffer* buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH, D3DXCOLOR color);

void SetSpriteColorRotation(ID3D11Buffer* buf, float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,
	D3DXCOLOR Color, float Rot);
