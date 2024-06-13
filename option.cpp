//=============================================================================
//
// オプション画面処理 [option.cpp]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#include "option.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

#include "file.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(18)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(236 * 2)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(66 * 2)			// 

#define MAGIC_CIRCLE_ROT			(0.01f)			//魔法陣の回転速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/options_bg_000.jpg",
	"data/TEXTURE/options_bg_001.png",
	"data/TEXTURE/title_option_cursor.png",
	"data/TEXTURE/title_SUMMONER.png",
	"data/TEXTURE/title_START.png",
	"data/TEXTURE/option_language.png",
	"data/TEXTURE/option_BGM.png",
	"data/TEXTURE/option_english.png",
	"data/TEXTURE/option_japanese.png",
	"data/TEXTURE/option_chinese.png",
	"data/TEXTURE/option_volume_000.png",
	"data/TEXTURE/option_volume_001.png",
	"data/TEXTURE/key_Q.png",
	"data/TEXTURE/option_summon.png",
	"data/TEXTURE/option_setting.png",
	"data/TEXTURE/key_E.png",
	"data/TEXTURE/option_000.png",
	"data/TEXTURE/option_001.png",


	





};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;

int								backTo;

static OPTION	g_Option[OPTION_MAX];

static LANGUAGE	g_Language[LANGUAGE_MAX];
BOOL		languageChoose = LANGUAGE_JAPANESE;

BOOL		optionSummon;
BOOL		optionSetting;

int			volumeMax;
int			volume;


XMFLOAT3	optionCursorPos;			// ポリゴンの座標
BOOL		optionCursorUse;			// true:使っている  false:未使用
float		optionCursorW, optionCursorH;			// 幅と高さ
int			optionCursorTexNo;			// テクスチャ番号
BOOL		optionCursorPosition;





//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitOption(void)
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
	g_TexNo = 0;

	optionSummon = FALSE;
	optionSetting = TRUE;


	//オプションの初期化
	for (int i = 0; i < OPTION_MAX; i++)
	{
		g_Option[i].use = TRUE;
		g_Option[i].pos = XMFLOAT3(SCREEN_WIDTH / 2.5, SCREEN_HEIGHT / 2.5 + i * 60.0f + 50.0f, 0.0f);	// 中心点から表示
		g_Option[i].w = 210.0f;
		g_Option[i].h = 70.0f;
		g_Option[i].texNo = 5 + i;

	}

	g_Option[OPTION_BGM].w = 120.0f;


	//言語の初期化
	for (int i = 0; i < LANGUAGE_MAX; i++)
	{
		g_Language[i].pos = XMFLOAT3(SCREEN_WIDTH / 1.5, SCREEN_HEIGHT / 2.5 + 50.0f, 0.0f);	// 中心点から表示
		g_Language[i].w = 150.0f;
		g_Language[i].h = 70.0f;
		g_Language[i].texNo = 7 + i;

	}
	g_Language[LANGUAGE_ENGLISH].use = FALSE;
	g_Language[LANGUAGE_JAPANESE].use = FALSE;
	g_Language[LANGUAGE_CHINESE].use = FALSE;

	languageChoose = GetLanguage();

	g_Language[languageChoose].use = TRUE;

	//音量の初期化
	volumeMax = 10;
	volume = 2;

	SetVolume(volume);

	//カーソルの初期化
	optionCursorPos = XMFLOAT3(g_Option[OPTION_LANGUAGE].pos.x - 120.0f, g_Option[OPTION_LANGUAGE].pos.y, 0.0f);
	optionCursorUse = TRUE;
	optionCursorW = 48.0f;
	optionCursorH = 48.0f;
	optionCursorTexNo = 2;
	optionCursorPosition = OPTION_LANGUAGE;


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitOption(void)
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
void UpdateOption(void)
{

	
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
		if (backTo == MODE_GAME)
		{
			SetLoadGame(TRUE);

			SetMode(MODE_GAME);

		}

		if (backTo == MODE_TITLE)
		{
			SetMode(MODE_TITLE);

		}
	}


	if ((GetKeyboardTrigger(DIK_E)) && (optionSummon == TRUE))
	{
		optionSummon = FALSE;

		optionSetting = TRUE;

	}
	else if ((GetKeyboardTrigger(DIK_E)) && (optionSetting == TRUE))
	{
		optionSetting = FALSE;

		optionSummon = TRUE;


	}




	// カーソルを移動する
	if (optionCursorUse == TRUE)
	{
		// カーソルを移動する
		if (GetKeyboardTrigger(DIK_W))
		{
			optionCursorPosition = (++optionCursorPosition + OPTION_MAX) % OPTION_MAX;

		}
		else if (GetKeyboardTrigger(DIK_S))
		{
			optionCursorPosition = (--optionCursorPosition + OPTION_MAX) % OPTION_MAX;

		}

		optionCursorPos.y = g_Option[optionCursorPosition].pos.y;			// ポリゴンの座標

		{//カーソルの位置によって調整する
			switch (optionCursorPosition)
			{
			case OPTION_LANGUAGE:

				if (GetKeyboardTrigger(DIK_D))
				{
					languageChoose = (++languageChoose + LANGUAGE_MAX) % LANGUAGE_MAX;
					for (int i = 0; i < LANGUAGE_MAX; i++)
					{
						g_Language[i].use = FALSE;
					}
					g_Language[languageChoose].use = TRUE;


				}
				else if (GetKeyboardTrigger(DIK_A))
				{
					languageChoose = (--languageChoose + LANGUAGE_MAX) % LANGUAGE_MAX;
					for (int i = 0; i < LANGUAGE_MAX; i++)
					{
						g_Language[i].use = FALSE;
					}

					g_Language[languageChoose].use = TRUE;



				}

				break;

			case OPTION_BGM:

				if (GetKeyboardTrigger(DIK_D))
				{
					volume++;
					if (volume > volumeMax)
					{
						volume = volumeMax;
					}
				}
				else if (GetKeyboardTrigger(DIK_A))
				{
					volume--;
					if (volume < 0)
					{
						volume = 0;
					}

				}

				SetVolume(volume);


				break;

			}
		}

	}









#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawOption(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 看板を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, SCREEN_WIDTH / 5, SCREEN_HEIGHT / 8, 426.0f * 1.5, 340.0f * 1.5, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	if (optionSetting == TRUE)
	{
		// 設定のオプションを描画
		{
			for (int i = 0; i < OPTION_MAX; i++)
			{
				if (g_Option[i].use == TRUE)
				{
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Option[i].texNo]);

					//スタートの位置やテクスチャー座標を反映
					float px = g_Option[i].pos.x;	// オプションの表示位置X
					float py = g_Option[i].pos.y;	// オプションの表示位置Y
					float pw = g_Option[i].w;		// オプションの表示幅
					float ph = g_Option[i].h;		// オプションの表示高さ

					float tw = 1.0f;	// テクスチャの幅
					float th = 1.0f;	// テクスチャの高さ
					float tx = 0.0f;	// テクスチャの左上X座標
					float ty = 0.0f;	// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
						0.0f);

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);

				}

			}


		}

		// 言語のオプションを描画
		{
			for (int i = 0; i < LANGUAGE_MAX; i++)
			{
				if (g_Language[i].use == TRUE)
				{
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Language[i].texNo]);

					//スタートの位置やテクスチャー座標を反映
					float px = g_Language[i].pos.x;	// オプションの表示位置X
					float py = g_Language[i].pos.y;	// オプションの表示位置Y
					float pw = g_Language[i].w;		// オプションの表示幅
					float ph = g_Language[i].h;		// オプションの表示高さ

					float tw = 1.0f;	// テクスチャの幅
					float th = 1.0f;	// テクスチャの高さ
					float tx = 0.0f;	// テクスチャの左上X座標
					float ty = 0.0f;	// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
						0.0f);

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);

				}

			}


		}

		// BGMゲージ
		{
			// 下敷きのゲージ（枠的な物）
			//ゲージの位置やテクスチャー座標を反映
			float px = SCREEN_WIDTH / 2;		// ゲージの表示位置X
			float py = SCREEN_HEIGHT / 2.5 + 100.0f;		// ゲージの表示位置Y
			float pw = 300.0f;		// ゲージの表示幅
			float ph = 30.0f;		// ゲージの表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標



			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);





			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

			//ゲージの位置やテクスチャー座標を反映
			px = SCREEN_WIDTH / 2;		// ゲージの表示位置X
			py = SCREEN_HEIGHT / 2.5 + 100.0f;		// ゲージの表示位置Y
			pw = 300.0f;		// ゲージの表示幅
			ph = 30.0f;		// ゲージの表示高さ

			tw = 1.0f;	// テクスチャの幅
			th = 1.0f;	// テクスチャの高さ
			tx = 0.0f;	// テクスチャの左上X座標
			ty = 0.0f;	// テクスチャの左上Y座標

			pw = pw * volume / volumeMax;
			tw = tw * volume / volumeMax;



			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

		//カーソルを描画
		{
			if (optionCursorUse == TRUE)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[optionCursorTexNo]);

				//スタートの位置やテクスチャー座標を反映
				float px = optionCursorPos.x;	// スタートの表示位置X
				float py = optionCursorPos.y;	// スタートの表示位置Y
				float pw = optionCursorW;		// スタートの表示幅
				float ph = optionCursorH;		// スタートの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					0.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}

		}


	}







}




void SetBack(int back)
{
	backTo = back;
}

BOOL GetLanguage(void)
{
	return languageChoose;
}
