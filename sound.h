//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{

	SOUND_LABEL_BGM_DrunkStar,	// BGM1
	SOUND_LABEL_BGM_EoS,	// BGM3

	SOUND_LABEL_SE_attck,		// 
	SOUND_LABEL_SE_bounce,		// 
	SOUND_LABEL_SE_dash,		// 
	SOUND_LABEL_SE_hit,		// 
	SOUND_LABEL_SE_reflect,		// 
	SOUND_LABEL_SE_coin,		// 
	SOUND_LABEL_SE_timePause,		// 
	SOUND_LABEL_SE_pasueReturn,		// 
	SOUND_LABEL_SE_walk,		// 
	SOUND_LABEL_SE_heal,		// 

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

