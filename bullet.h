//=============================================================================
//
// バレット処理 [bullet.h]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_MAX		(1)		// バレットのMax数
#define BULLET_SPEED	(8.0f)		// バレットの移動スピード


// バレット構造体
struct BULLET
{
	BOOL				use;				// true:使っている  false:未使用
	float				w, h;				// 幅と高さ
	XMFLOAT3			pos;				// バレットの座標
	XMFLOAT3			rot;				// バレットの回転量
	XMFLOAT3			move;				// バレットの移動量
	int					countAnim;			// アニメーションカウント
	int					patternAnim;		// アニメーションパターンナンバー
	int					texNo;				// 何番目のテクスチャーを使用するのか

	BOOL				catchAttack;		//捕獲攻撃のフラグ
	int					summonMonster;		//契約中の召喚獣

	BOOL				collision;			//

};

enum {
	TEXT_TOAD_R,
	TEXT_TOAD_L,

	

	TEXT_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);
void SetBullet(XMFLOAT3 pos, int num);


