//=============================================================================
//
// チュートリ画面処理 [tutorial.cpp]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "fade.h"
#include "option.h"
#include "sound.h"

#include "file.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(37)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(100)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(100)			// 

#define ANIM_WAIT					(5)		// アニメーションの切り替わるWait値



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/tutorial_000_JP1.png",
	"data/TEXTURE/tutorial_001_JP2.png",
	"data/TEXTURE/tutorial_002_JP3.png",
	"data/TEXTURE/tutorial_003_JP4.png",
	"data/TEXTURE/tutorial_004_JP5.png",
	"data/TEXTURE/key_A.png",
	"data/TEXTURE/tutorial_char_move_r.png",
	"data/TEXTURE/key_D.png",
	"data/TEXTURE/tutorial_char_move_l.png",
	"data/TEXTURE/key_J.png",
	"data/TEXTURE/tutorial_char_jump.png",
	"data/TEXTURE/key_O.png",
	"data/TEXTURE/tutorial_char_attack.png",
	"data/TEXTURE/catch_attack_000.png",
	"data/TEXTURE/key_P.png",
	"data/TEXTURE/Summon_006_Bleach_R.png",
	"data/TEXTURE/key_DEL.png",
	"data/TEXTURE/tutorialMon_000.png",
	"data/TEXTURE/tutorialMon_001.png",
	"data/TEXTURE/tutorialMon_002.png",
	"data/TEXTURE/tutorialMon_003.png",
	"data/TEXTURE/tutorialMon_004.png",
	"data/TEXTURE/tutorialMon_005.png",
	"data/TEXTURE/tutorialMon_006.png",
	"data/TEXTURE/tutorialMon_007.png",
	"data/TEXTURE/tutorial_EN1.png",
	"data/TEXTURE/tutorial_EN2.png",
	"data/TEXTURE/tutorial_EN3.png",
	"data/TEXTURE/tutorial_EN4.png",
	"data/TEXTURE/tutorial_EN5.png",
	"data/TEXTURE/tutorial_CN1.png",
	"data/TEXTURE/tutorial_CN2.png",
	"data/TEXTURE/tutorial_CN3.png",
	"data/TEXTURE/tutorial_CN4.png",
	"data/TEXTURE/tutorial_CN5.png",
	"data/TEXTURE/tutorial_NEXT.png",
	"data/TEXTURE/tutorial_BACK.png",


	



};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static OBJ	g_Object[OBJ_MAX];	// チュートリアル構造体
static OBJ	g_Monster[8];	// チュートリアル構造体


static BOOL						g_Load = FALSE;

int								page;

BOOL							first;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 25;
	page = 0;

	first = TRUE;

	// チュートリアル構造体の初期化
	g_Object[KEY_D].pos = XMFLOAT3(50.0f, 400.0f, 0.0f);	// 中心点から表示
	g_Object[KEY_D].patternX = 1;

	g_Object[TUTORIAL_MOVE_R].pos = XMFLOAT3(150.0f, 400.0f, 0.0f);	// 中心点から表示
	g_Object[TUTORIAL_MOVE_R].patternX = 8;

	g_Object[KEY_A].pos = XMFLOAT3(50.0f, 450.0f, 0.0f);	// 中心点から表示
	g_Object[KEY_A].patternX = 1;

	g_Object[TUTORIAL_MOVE_L].pos = XMFLOAT3(150.0f, 450.0f, 0.0f);	// 中心点から表示
	g_Object[TUTORIAL_MOVE_L].patternX = 8;

	g_Object[KEY_J].pos = XMFLOAT3(300.0f, 425.0f, 0.0f);	// 中心点から表示
	g_Object[KEY_J].patternX = 1;

	g_Object[TUTORIAL_JUMP].pos = XMFLOAT3(400.0f, 425.0f, 0.0f);	// 中心点から表示
	g_Object[TUTORIAL_JUMP].patternX = 8;

	g_Object[KEY_O].pos = XMFLOAT3(600.0f, 125.0f, 0.0f);	// 中心点から表示
	g_Object[KEY_O].patternX = 1;

	g_Object[TUTORIAL_ATTACK].pos = XMFLOAT3(650.0f, 200.0f, 0.0f);	// 中心点から表示
	g_Object[TUTORIAL_ATTACK].patternX = 8;

	g_Object[TUTORIAL_CATCHATTACK].pos = XMFLOAT3(750.0f, 200.0f, 0.0f);	// 中心点から表示
	g_Object[TUTORIAL_CATCHATTACK].patternX = 8;

	g_Object[KEY_P].pos = XMFLOAT3(625.0f, 350.0f, 0.0f);	// 中心点から表示
	g_Object[KEY_P].patternX = 1;

	g_Object[TUTORIAL_SUMMON].pos = XMFLOAT3(750.0f, 325.0f, 0.0f);	// 中心点から表示
	g_Object[TUTORIAL_SUMMON].patternX = 8;

	g_Object[KEY_DEL].pos = XMFLOAT3(750.0f, 450.0f, 0.0f);	// 中心点から表示
	g_Object[KEY_DEL].patternX = 1;

	for (int i = 0; i < OBJ_MAX; i++)

	{
		g_Object[i].use = TRUE;
		g_Object[i].start = FALSE;
		g_Object[i].w = 50.0f;
		g_Object[i].h = 50.0f;
		g_Object[i].texNo = i + 5;

		g_Object[i].patternY = 1;
		g_Object[i].patternNum = g_Object[i].patternX * g_Object[i].patternY;

		g_Object[i].countAnim = 0;
		g_Object[i].patternAnim = 0;

	}

	g_Object[TUTORIAL_SUMMON].w = 100.0f;
	g_Object[TUTORIAL_SUMMON].h = 100.0f;

	for (int i = 0; i < 8; i++)
	{
		g_Monster[i].use = FALSE;
		g_Monster[i].start = FALSE;
		g_Monster[i].w = 300.0f;
		g_Monster[i].h = 300.0f;
		g_Monster[i].texNo = i + 17;


		g_Monster[i].patternX = 8;
		g_Monster[i].patternY = 1;
		g_Monster[i].patternNum = g_Monster[i].patternX * g_Monster[i].patternY;

		g_Monster[i].countAnim = 0;
		g_Monster[i].patternAnim = 0;

	}

	g_Monster[0].pos = XMFLOAT3(200.0f, 80.0f, 0.0f);	// 中心点から表示
	g_Monster[1].pos = XMFLOAT3(500.0f, 80.0f, 0.0f);	// 中心点から表示
	g_Monster[2].pos = XMFLOAT3(200.0f, 80.0f, 0.0f);	// 中心点から表示
	g_Monster[3].pos = XMFLOAT3(500.0f, 80.0f, 0.0f);	// 中心点から表示
	g_Monster[4].pos = XMFLOAT3(200.0f, 80.0f, 0.0f);	// 中心点から表示
	g_Monster[5].pos = XMFLOAT3(500.0f, 80.0f, 0.0f);	// 中心点から表示
	g_Monster[6].pos = XMFLOAT3(250.0f, 20.0f, 0.0f);	// 中心点から表示
	g_Monster[7].pos = XMFLOAT3(550.0f, 20.0f, 0.0f);	// 中心点から表示

	//言語確認
	switch (GetLanguage())
	{
	case LANGUAGE_ENGLISH:
		g_TexNo = 25;

		break;
	case LANGUAGE_JAPANESE:
		g_TexNo = 0;

		break;
	case LANGUAGE_CHINESE:
		g_TexNo = 30;

		break;

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
	for (int i = 0; i < OBJ_MAX; i++)
	{
		// アニメーション  
		if (g_Object[i].use == TRUE)
		{
			g_Object[i].countAnim += 1.0f;
			if (g_Object[i].countAnim > ANIM_WAIT)
			{
				g_Object[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Object[i].patternAnim = g_Object[i].patternX + ((g_Object[i].patternAnim + 1) % g_Object[i].patternX);
			}
		}


	}

	for (int i = 0; i < 8; i++)
	{
		// アニメーション  
		if (g_Monster[i].use == TRUE)
		{
			g_Monster[i].countAnim += 1.0f;
			if (g_Monster[i].countAnim > ANIM_WAIT)
			{
				g_Monster[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Monster[i].patternAnim = g_Monster[i].patternX + ((g_Monster[i].patternAnim + 1) % g_Monster[i].patternX);
			}
		}


	}



	
	// ゲームパッドで入力処理
	if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_GAME);

	}

	if (GetKeyboardTrigger(DIK_BACK))
	{
		SetMode(MODE_TITLE);
	}

	int num;
	switch (GetLanguage())
	{
	case LANGUAGE_ENGLISH:
		num = 25;

		break;
	case LANGUAGE_JAPANESE:
		num = 0;

		break;
	case LANGUAGE_CHINESE:
		num = 30;

		break;

	}

	if (GetKeyboardTrigger(DIK_A))
	{
		page = (--page + 5) % 5;

		g_TexNo = (--g_TexNo + 5) % 5 + num;


	}
	else if (GetKeyboardTrigger(DIK_D))
	{
		page = (++page + 5) % 5;

		g_TexNo = (++g_TexNo + 5) % 5 + num;

	}



	switch (page)
	{
	case 0:

		first = TRUE;

		for (int i = 0; i < 8; i++)
		{
			g_Monster[i].use = FALSE;


		}

		for (int i = 0; i < OBJ_MAX; i++)

		{
			g_Object[i].use = TRUE;

		}

		break;

	case 1:

		first = FALSE;

		for (int i = 0; i < 8; i++)
		{
			g_Monster[i].use = FALSE;

		}

		for (int i = 0; i < OBJ_MAX; i++)

		{
			g_Object[i].use = FALSE;
		}

		g_Monster[0].use = TRUE;
		g_Monster[1].use = TRUE;

		break;
	case 2:
		for (int i = 0; i < 8; i++)
		{
			g_Monster[i].use = FALSE;

		}
		g_Monster[2].use = TRUE;
		g_Monster[3].use = TRUE;

		break;

	case 3:
		for (int i = 0; i < 8; i++)
		{
			g_Monster[i].use = FALSE;

		}
		g_Monster[4].use = TRUE;
		g_Monster[5].use = TRUE;

		break;
	case 4:

		first = FALSE;

		for (int i = 0; i < 8; i++)
		{
			g_Monster[i].use = FALSE;

		}
		for (int i = 0; i < OBJ_MAX; i++)

		{
			g_Object[i].use = FALSE;
		}

		g_Monster[6].use = TRUE;
		g_Monster[7].use = TRUE;

		break;


	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// オプションの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 800.0f, 25.0f, 25.0f, 25.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 900.0f, 25.0f, 25.0f, 25.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	if (first == TRUE)
	{
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[35]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLeftTop(g_VertexBuffer, 825.0f, 20.0f, 60.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}

	//BACKの描画
	{	
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 800.0f, 55.0f, 40.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
		
	
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[36]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 850.0f, 50.0f, 70.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}



	// チュートリアルアニメを描画
	{

		for (int i = 0; i < OBJ_MAX; i++)
		{
			if (g_Object[i].use == TRUE)		// このOBJが使われている？
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Object[i].texNo]);

				float px = g_Object[i].pos.x;	// OBJの表示位置X
				float py = g_Object[i].pos.y;	// OBJの表示位置Y
				float pw = g_Object[i].w;		// OBJの表示幅
				float ph = g_Object[i].h;		// OBJの表示高さ


				//float tw = 1.0f;	// テクスチャの幅
				//float th = 1.0f;	// テクスチャの高さ
				//float tx = 0.0f;	// テクスチャの左上X座標
				//float ty = 0.0f;	// テクスチャの左上Y座標

			// アニメーション用
				float tw = 1.0f / g_Object[i].patternX;	// テクスチャの幅
				float th = 1.0f / g_Object[i].patternY;	// テクスチャの高さ
				float tx = (float)(g_Object[i].patternAnim % g_Object[i].patternX) * tw;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

		   // １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}


		}

	}


	// チュートリアルモンスターを描画
	{

		for (int i = 0; i < OBJ_MAX; i++)
		{
			if (g_Monster[i].use == TRUE)		// このOBJが使われている？
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Monster[i].texNo]);

				float px = g_Monster[i].pos.x;	// OBJの表示位置X
				float py = g_Monster[i].pos.y;	// OBJの表示位置Y
				float pw = g_Monster[i].w;		// OBJの表示幅
				float ph = g_Monster[i].h;		// OBJの表示高さ


			// アニメーション用
				float tw = 1.0f / g_Monster[i].patternX;	// テクスチャの幅
				float th = 1.0f / g_Monster[i].patternY;	// テクスチャの高さ
				float tx = (float)(g_Monster[i].patternAnim % g_Monster[i].patternX) * tw;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

		   // １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}


		}

	}



}



