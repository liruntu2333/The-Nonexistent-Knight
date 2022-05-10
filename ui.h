//
// @file				ui.h
// @brief				Set & present UI at game mode.
// @copyright			2021 LI ZIZHEN liruntu2333@gmail.com
// @lisense				GPL
//
#pragma once

//
// MACROS
//

//
// Structrue Declarations
//
struct ICON
{
	D3DXVECTOR3		pos;		// coordinate of icon
	float			w, h;		// width & hight
	int				texNo;		// texture number

	BOOL			use;
};

//
// Function Prototype Declarations
//
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);