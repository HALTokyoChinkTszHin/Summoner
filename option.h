//=============================================================================
//
// オプション画面処理 [option.h]
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

struct OPTION
{
	BOOL		use;
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

};

struct LANGUAGE
{
	BOOL		use;
	XMFLOAT3	pos;			// ポリゴンの座標
	float		w, h;			// 幅と高さ
	int			texNo;			// テクスチャ番号

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
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitOption(void);
void UninitOption(void);
void UpdateOption(void);
void DrawOption(void);

void SetBack(int back);
BOOL GetLanguage(void);


