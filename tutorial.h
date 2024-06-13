//=============================================================================
//
// �`���[�g����ʏ��� [tutorial.h]
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

enum {
	KEY_D,
	TUTORIAL_MOVE_R,
	KEY_A,
	TUTORIAL_MOVE_L,
	KEY_J,
	TUTORIAL_JUMP,
	KEY_O,
	TUTORIAL_ATTACK,
	TUTORIAL_CATCHATTACK,
	KEY_P,
	TUTORIAL_SUMMON,
	KEY_DEL,

	OBJ_MAX
};

struct OBJ
{
	XMFLOAT3	pos;		// �|���S���̍��W
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	BOOL		use;
	BOOL		start;
	BOOL		on;

	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[

	int			patternX;
	int			patternY;
	int			patternNum;




};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);


