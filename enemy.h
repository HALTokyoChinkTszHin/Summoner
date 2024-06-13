//=============================================================================
//
// �G�l�~�[���� [enemy.h]
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
#define ENEMY_MAX		(4)		// �G�l�~�[��Max�l��


enum {
	TYPE_TOAD,
	TYPE_GOBLIN,
	TYPE_MUSHROOM,
	TYPE_BLEACH,

	TYPE_MAX
};



//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct ENEMY
{
	XMFLOAT3	pos;			// �|���S���̍��W
	XMFLOAT3	rot;			// �|���S���̉�]��
	XMFLOAT3	scl;			// �|���S���̊g��k��
	BOOL		use;			// true:�g���Ă���  false:���g�p
	float		w, h;			// ���ƍ���
	float		countAnim;		// �A�j���[�V�����J�E���g
	int			patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int			texNo;			// �e�N�X�`���ԍ�
	XMFLOAT3	move;			// �ړ����x

	int			dir;			// �����i0:�E 1:�� �j
	int			monster;


	int			patternX;		// �A�j���p�^�[���̃e�N�X�`�����������iX)
	int			patternY;		// �A�j���p�^�[���̃e�N�X�`�����������iY)
	int			patternAnimNum;	// �A�j���[�V�����p�^�[����



	float		time;			// ���`��ԗp
	int			tblNo;			// �s���f�[�^�̃e�[�u���ԍ�
	int			tblMax;			// ���̃e�[�u���̃f�[�^��

	BOOL		attack;			// �U���t���O

	BOOL		collision;		//�����蔻�������H
	float		boxX, boxY, boxPosX, boxPosY;			//�����蔻������R�ɂ��邽�߂̌��l

	XMFLOAT3	hpPos;			// �|���S���̍��W
	BOOL		hpGauge;		//HP�Q�[�W��\������H
	float		hpMax;			//HP�̍ő�l
	float		hp;			//HP

	int			rebornCnt;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);
void SetDamageEnemy(int damage, int num);



