//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : �K�q���i�`�F���Y�[�V�F���j
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_MAX		(1)		// �o���b�g��Max��
#define BULLET_SPEED	(8.0f)		// �o���b�g�̈ړ��X�s�[�h


// �o���b�g�\����
struct BULLET
{
	BOOL				use;				// true:�g���Ă���  false:���g�p
	float				w, h;				// ���ƍ���
	XMFLOAT3			pos;				// �o���b�g�̍��W
	XMFLOAT3			rot;				// �o���b�g�̉�]��
	XMFLOAT3			move;				// �o���b�g�̈ړ���
	int					countAnim;			// �A�j���[�V�����J�E���g
	int					patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int					texNo;				// ���Ԗڂ̃e�N�X�`���[���g�p����̂�

	BOOL				catchAttack;		//�ߊl�U���̃t���O
	int					summonMonster;		//�_�񒆂̏����b

	BOOL				collision;			//

};

enum {
	TEXT_TOAD_R,
	TEXT_TOAD_L,

	

	TEXT_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);
void SetBullet(XMFLOAT3 pos, int num);


