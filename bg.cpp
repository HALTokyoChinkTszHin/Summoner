//=============================================================================
//
// BG処理 [bg.cpp]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#include "bg.h"




//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH * 4)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)				// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)							// テクスチャの数

#define ANIM_WAIT					(5)							// アニメーションの切り替わるWait値

#define TEXTURE_GROUND_WIDTH		(35)						// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_GROUND_HEIGHT		(35)						// (SCREEN_HEIGHT)	// 

#define TEXTURE_WIDTH_LOGO			(480)						// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)						// 



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/sky000.png",
	"data/TEXTURE/sky001.png",
	"data/TEXTURE/fargrounds.png",
	"data/TEXTURE/turfground.png",
	"data/TEXTURE/rockground.png",



};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BG	g_BG;

static GROUND	g_Ground[TEXTURE_GROUND_MAX];	// 地面構造体


static GROUND	g_Step[STEP_MAX];	// 踏み場構造体




//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
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
	g_BG.w     = TEXTURE_WIDTH;
	g_BG.h     = TEXTURE_HEIGHT;
	g_BG.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;

	g_BG.scrl  = 0.0f;		// TEXスクロール
	g_BG.scrl2 = 0.0f;		// TEXスクロール

		// 地面構造体の初期化
	for (int i = 0; i < TEXTURE_GROUND_MAX; i++)
	{
		g_Ground[i].use = TRUE;
		g_Ground[i].pos = XMFLOAT3(35.0f * i , SCREEN_HEIGHT - TEXTURE_GROUND_HEIGHT, 0.0f);	// 中心点から表示
		g_Ground[i].w = TEXTURE_GROUND_WIDTH;
		g_Ground[i].h = TEXTURE_GROUND_HEIGHT;
		g_Ground[i].texNo = 3;



	}


	// 踏み場構造体の初期化
	for (int i = 0; i < STEP_MAX; i++)
	{
		g_Step[i].use = FALSE;
		g_Step[i].pos = XMFLOAT3(35.0f * i, 480.0f, 0.0f);	// 中心点から表示
		g_Step[i].w = TEXTURE_GROUND_WIDTH;
		g_Step[i].h = TEXTURE_GROUND_HEIGHT;
		g_Step[i].texNo = 4;



	}








	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
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
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// １フレ前の情報を保存





#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
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

	// 背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 空を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		g_BG.scrl += 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, SKY_H,
			g_BG.scrl, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		g_BG.scrl2 -= 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, SKY_H, SCREEN_WIDTH, SKY_H,
			g_BG.scrl2, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 地面を描画
	{

		for (int i = 0; i < TEXTURE_GROUND_MAX; i++)
		{
			if (g_Ground[i].use == TRUE)		// この地面が使われている？
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Ground[i].texNo]);

				float px = g_Ground[i].pos.x;	// 地面の表示位置X
				float py = g_Ground[i].pos.y;	// 地面の表示位置Y
				float pw = g_Ground[i].w;		// 地面の表示幅
				float ph = g_Ground[i].h;		// 地面の表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
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


	// 踏み場を描画
	{

		for (int i = 0; i < STEP_MAX; i++)
		{
			if (g_Step[i].use == TRUE)		// この地面が使われている？
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Step[i].texNo]);

				float px = g_Step[i].pos.x;	// 地面の表示位置X
				float py = g_Step[i].pos.y;	// 地面の表示位置Y
				float pw = g_Step[i].w;		// 地面の表示幅
				float ph = g_Step[i].h;		// 地面の表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
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


//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}

//=============================================================================
// 地面構造体の先頭アドレスを取得
//=============================================================================
GROUND* GetGround(void)
{
	return &g_Ground[0];
}

//=============================================================================
// 地面構造体の先頭アドレスを取得
//=============================================================================
GROUND* GetStep(void)
{
	return &g_Step[0];
}






