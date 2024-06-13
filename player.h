//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define	PLAYER_OFFSET_CNT	(16)	// 16分身

enum
{
	//方向
	CHAR_DIR_RIGHT,
	CHAR_DIR_LEFT,
	CHAR_DIR_UP,
	CHAR_DIR_DOWN,

	CHAR_DIR_MAX
};

enum
{
	//動作
	CHAR_ANIM_MOVE,
	CHAR_ANIM_JUMP,
	CHAR_ANIM_ATTACK,
	CHAR_ANIM_DIE,


	CHAR_ANIM_MAX

};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	
	int			dir;			// 向き（0:右 1:左 2:上 3:下）
	int			action;			//アニメの動作
	BOOL		moving;			// 移動中フラグ

	BOOL		dash;			// ダッシュ中フラグ
	XMFLOAT3	move;			// 移動速度
	XMFLOAT3	offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

	BOOL		jump;			// ジャンプフラグ
	float		jumpY;			// ジャンプの高さ
	int			jumpCnt;		// ジャンプ中のカウント
	float		jumpYMax;		// 

	BOOL		attack;			// 攻撃フラグ

	int			attackCnt;		//  攻撃中のカウント

	BOOL		collision;		//当たり判定をする？（無敵）
	int			collisionCnt;		// 無敵中のカウント
	float		collisionEffect;	//無敵状態のα値


	float		hpMax;			//HPの最大値
	float		hp;			//HP
};




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER* GetPlayer(void);

void SetDamagePlayer(int damage);




