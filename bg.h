//=============================================================================
//
// BG���� [bg.h]
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
#define TEXTURE_GROUND_MAX					(110)				// �e�N�X�`���̐�
#define STEP_MAX							(25)			// ���S�T�C�Y



struct BG
{
	XMFLOAT3	pos;		// �|���S���̍��W
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�

	float		scrl;		// �X�N���[��
	XMFLOAT3	old_pos;	// �P�t���O�̃|���S���̍��W
	float		scrl2;		// �X�N���[��
};

struct GROUND
{
	XMFLOAT3	pos;		// �|���S���̍��W
	BOOL		use;		// true:�g���Ă���  false:���g�p
	float		w, h;		// ���ƍ���
	int			texNo;		// �g�p���Ă���e�N�X�`���ԍ�

};



#define	SKY_H	(150.0f)	// ��̃|���S���̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBG(void);
void UninitBG(void);
void UpdateBG(void);
void DrawBG(void);

BG* GetBG(void);
GROUND* GetGround(void);
GROUND* GetStep(void);





