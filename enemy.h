//=============================================================================
//
// エネミー処理 [enemy.h]
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
#define ENEMY_MAX		(4)		// エネミーのMax人数


enum {
	TYPE_TOAD,
	TYPE_GOBLIN,
	TYPE_MUSHROOM,
	TYPE_BLEACH,

	TYPE_MAX
};



//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct ENEMY
{
	XMFLOAT3	pos;			// ポリゴンの座標
	XMFLOAT3	rot;			// ポリゴンの回転量
	XMFLOAT3	scl;			// ポリゴンの拡大縮小
	BOOL		use;			// true:使っている  false:未使用
	float		w, h;			// 幅と高さ
	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー
	int			texNo;			// テクスチャ番号
	XMFLOAT3	move;			// 移動速度

	int			dir;			// 向き（0:右 1:左 ）
	int			monster;


	int			patternX;		// アニメパターンのテクスチャ内分割数（X)
	int			patternY;		// アニメパターンのテクスチャ内分割数（Y)
	int			patternAnimNum;	// アニメーションパターン数



	float		time;			// 線形補間用
	int			tblNo;			// 行動データのテーブル番号
	int			tblMax;			// そのテーブルのデータ数

	BOOL		attack;			// 攻撃フラグ

	BOOL		collision;		//当たり判定をする？
	float		boxX, boxY, boxPosX, boxPosY;			//当たり判定を自然にするための減値

	XMFLOAT3	hpPos;			// ポリゴンの座標
	BOOL		hpGauge;		//HPゲージを表示する？
	float		hpMax;			//HPの最大値
	float		hp;			//HP

	int			rebornCnt;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY* GetEnemy(void);
void SetDamageEnemy(int damage, int num);



