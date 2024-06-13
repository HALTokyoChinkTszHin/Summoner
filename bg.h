//=============================================================================
//
// BG処理 [bg.h]
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
#define TEXTURE_GROUND_MAX					(110)				// テクスチャの数
#define STEP_MAX							(25)			// ロゴサイズ



struct BG
{
	XMFLOAT3	pos;		// ポリゴンの座標
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号

	float		scrl;		// スクロール
	XMFLOAT3	old_pos;	// １フレ前のポリゴンの座標
	float		scrl2;		// スクロール
};

struct GROUND
{
	XMFLOAT3	pos;		// ポリゴンの座標
	BOOL		use;		// true:使っている  false:未使用
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号

};



#define	SKY_H	(150.0f)	// 空のポリゴンの高さ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBG(void);
void UninitBG(void);
void UpdateBG(void);
void DrawBG(void);

BG* GetBG(void);
GROUND* GetGround(void);
GROUND* GetStep(void);





