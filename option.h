//=============================================================================
//
// �I�v�V������ʏ��� [option.h]
// Author : �K�q���i�`�F���Y�[�V�F���j
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

struct OPTION
{
	BOOL		use;
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

};

struct LANGUAGE
{
	BOOL		use;
	XMFLOAT3	pos;			// �|���S���̍��W
	float		w, h;			// ���ƍ���
	int			texNo;			// �e�N�X�`���ԍ�

};


enum {
	OPTION_LANGUAGE,
	OPTION_BGM,

	OPTION_MAX
};

enum {
	LANGUAGE_ENGLISH,
	LANGUAGE_JAPANESE,
	LANGUAGE_CHINESE,

	LANGUAGE_MAX
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitOption(void);
void UninitOption(void);
void UpdateOption(void);
void DrawOption(void);

void SetBack(int back);
BOOL GetLanguage(void);


