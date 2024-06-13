//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : �K�q���i�`�F���Y�[�V�F���j
// 
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_bgm_title,	// BGM0
	SOUND_LABEL_BGM_bgm_battle,	// BGM1
	SOUND_LABEL_BGM_bgm_result,	// BGM2

	SOUND_LABEL_BGM_maou,		// BGM Maou

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

void SetVolume(int num);

