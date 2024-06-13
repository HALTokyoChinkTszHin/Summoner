//=============================================================================
//
// チュートリ画面処理 [tutorial.h]
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
	XMFLOAT3	pos;		// ポリゴンの座標
	float		w, h;		// 幅と高さ
	int			texNo;		// 使用しているテクスチャ番号

	BOOL		use;
	BOOL		start;
	BOOL		on;

	float		countAnim;		// アニメーションカウント
	int			patternAnim;	// アニメーションパターンナンバー

	int			patternX;
	int			patternY;
	int			patternNum;




};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);


