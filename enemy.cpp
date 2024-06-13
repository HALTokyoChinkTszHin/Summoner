//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#include "enemy.h"
#include "bg.h"
#include "player.h"
#include "bullet.h"
#include "fade.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200)	// キャラサイズ
#define TEXTURE_HEIGHT				(200)	// 
#define TEXTURE_MAX					(11)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/HPGauge_000.png",
	"data/TEXTURE/HPGauge_001.png",
	"data/TEXTURE/Monster_000_Toad_R.png",
	"data/TEXTURE/Monster_001_Toad_L.png",
	"data/TEXTURE/Monster_002_Goblin_R.png",
	"data/TEXTURE/Monster_003_Goblin_L.png",
	"data/TEXTURE/Monster_004_Mushroom_R.png",
	"data/TEXTURE/Monster_005_Mushroom_L.png",
	"data/TEXTURE/Monster_006_Bleach_R.png",
	"data/TEXTURE/Monster_007_Bleach_L.png",
	"data/TEXTURE/shadow000.jpg",




};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];		// エネミー構造体

static int		g_EnemyCnt = ENEMY_MAX;

static INTERPOLATION_DATA g_MoveTbl0[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(3800.0f,  SCREEN_HEIGHT - 75.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	1200 , 3},
	{ XMFLOAT3(40.0f,  SCREEN_HEIGHT - 75.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	1200 , 2},
};


static INTERPOLATION_DATA g_MoveTbl1[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(3800.0f,   SCREEN_HEIGHT - 70.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	900 , 5},
	{ XMFLOAT3(40.0f,  SCREEN_HEIGHT - 70.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	900 , 4},
};


static INTERPOLATION_DATA g_MoveTbl2[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(3800.0f,   SCREEN_HEIGHT - 85.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.5f, 1.5f, 1.0f),	1500 , 7},
	{ XMFLOAT3(40.0f,  SCREEN_HEIGHT - 85.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.5f, 1.5f, 1.0f),	1500 , 6},
};

static INTERPOLATION_DATA g_MoveTbl3[] = {
	//座標									回転率							拡大率							時間
	{ XMFLOAT3(3820.0f,   SCREEN_HEIGHT - 130.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	750 , 9},
	{ XMFLOAT3(20.0f,  SCREEN_HEIGHT - 130.0f, 0.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f),	750 , 8},
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	g_MoveTbl0,
	g_MoveTbl1,
	g_MoveTbl2,
	g_MoveTbl3,


};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
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


	// エネミー構造体の初期化
	g_EnemyCnt = 0;
	g_Enemy[TYPE_TOAD].hpMax = 500;
	g_Enemy[TYPE_GOBLIN].hpMax = 200;
	g_Enemy[TYPE_MUSHROOM].hpMax = 50;
	g_Enemy[TYPE_BLEACH].hpMax = 1000;



	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = TRUE;
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Enemy[i].w = TEXTURE_WIDTH;
		g_Enemy[i].h = TEXTURE_HEIGHT;
		g_Enemy[i].texNo = 2 + 2 * i;


		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;
		g_Enemy[i].patternX = 8;
		g_Enemy[i].patternY = 2;
		g_Enemy[i].patternAnimNum = g_Enemy[i].patternX * g_Enemy[i].patternY;



		g_Enemy[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Enemy[i].attack = FALSE;

		g_Enemy[i].hp = g_Enemy[i].hpMax;


		g_Enemy[i].collision = TRUE;

		g_Enemy[i].hpGauge = TRUE;

		g_EnemyCnt++;

		g_Enemy[i].rebornCnt = 0;
	}

	//個別修正　
	g_Enemy[TYPE_TOAD].pos = XMFLOAT3(500.0f, SCREEN_HEIGHT - 70.0f, 0.0f);	// 中心点から表示
	g_Enemy[TYPE_TOAD].hpPos = XMFLOAT3(30.0f, 70.0f, 0.0f);
	g_Enemy[TYPE_TOAD].w = 175.0f;
	g_Enemy[TYPE_TOAD].h = 175.0f;
	g_Enemy[TYPE_TOAD].boxX = 120.0f ;
	g_Enemy[TYPE_TOAD].boxY = 80.0f;
	g_Enemy[TYPE_TOAD].boxPosX = 0.0f;
	g_Enemy[TYPE_TOAD].boxPosY = 0.0f;
	g_Enemy[TYPE_TOAD].monster = 2;

	g_Enemy[TYPE_GOBLIN].pos = XMFLOAT3(1000.0f, SCREEN_HEIGHT - 70.0f, 0.0f);	// 中心点から表示
	g_Enemy[TYPE_GOBLIN].hpPos = XMFLOAT3(30.0f, 50.0f, 0.0f);
	g_Enemy[TYPE_GOBLIN].boxX = 140.0f;
	g_Enemy[TYPE_GOBLIN].boxY = 110.0f;
	g_Enemy[TYPE_GOBLIN].boxPosX = 0.0f;
	g_Enemy[TYPE_GOBLIN].boxPosY = 0.0f;
	g_Enemy[TYPE_GOBLIN].monster = 4;


	g_Enemy[TYPE_MUSHROOM].w = TEXTURE_WIDTH * 1.5;
	g_Enemy[TYPE_MUSHROOM].h = TEXTURE_HEIGHT * 1.5;
	g_Enemy[TYPE_MUSHROOM].pos = XMFLOAT3(1500.0f, SCREEN_HEIGHT - 70.0f, 0.0f);	// 中心点から表示
	g_Enemy[TYPE_MUSHROOM].hpPos = XMFLOAT3(45.0f, 50.0f, 0.0f);
	g_Enemy[TYPE_MUSHROOM].boxX = 175.0f * 1.5;
	g_Enemy[TYPE_MUSHROOM].boxY = 135.0f * 1.5;
	g_Enemy[TYPE_MUSHROOM].boxPosX = 0.0f;
	g_Enemy[TYPE_MUSHROOM].boxPosY = 0.0f;
	g_Enemy[TYPE_MUSHROOM].monster = 6;


	g_Enemy[TYPE_BLEACH].pos = XMFLOAT3(2000.0f, SCREEN_HEIGHT - 155.0f, 0.0f);	// 中心点から表示
	g_Enemy[TYPE_BLEACH].hpPos = XMFLOAT3(-20.0f, 50.0f, 0.0f);
	g_Enemy[TYPE_BLEACH].w = 250.0f;
	g_Enemy[TYPE_BLEACH].h = 250.0f;
	g_Enemy[TYPE_BLEACH].boxX = 120.0f;
	g_Enemy[TYPE_BLEACH].boxY = 60.0f;
	g_Enemy[TYPE_BLEACH].boxPosX = -60.0f;
	g_Enemy[TYPE_BLEACH].boxPosY = -40.0f;
	g_Enemy[TYPE_BLEACH].monster = 8;


	//// 0番だけ線形補間で動かしてみる
	g_Enemy[0].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[0].tblNo = 0;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tblMax = sizeof(g_MoveTbl0) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 1番だけ線形補間で動かしてみる
	g_Enemy[1].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[1].tblNo = 1;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[1].tblMax = sizeof(g_MoveTbl1) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 2番だけ線形補間で動かしてみる
	g_Enemy[2].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[2].tblNo = 2;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[2].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	// 3番だけ線形補間で動かしてみる
	g_Enemy[3].time = 0.0f;		// 線形補間用のタイマーをクリア
	g_Enemy[3].tblNo = 3;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[3].tblMax = sizeof(g_MoveTbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	g_EnemyCnt = 0;	// 生きてるエネミーの数


	for (int i = 0; i < ENEMY_MAX; i++)
	{

		// 生きてるエネミーだけ処理をする
		if (g_Enemy[i].use == TRUE)
		{
			BULLET* bullet = GetBullet();

			if (g_Enemy[i].hp == 0)
			{
				g_Enemy[i].hpGauge = FALSE;

				g_Enemy[i].use = FALSE;


			}


			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Enemy[i].pos;

			// アニメーション  
			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				// パターンの切り替え
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % g_Enemy[i].patternAnimNum;
			}

			// 移動処理
			if (g_Enemy[i].tblMax > 0)	// 線形補間を実行する？
			{	// 線形補間の処理
				int nowNo = (int)g_Enemy[i].time;			// 整数分であるテーブル番号を取り出している
				int maxNo = g_Enemy[i].tblMax;				// 登録テーブル数を数えている
				int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
				INTERPOLATION_DATA* tbl = g_MoveTblAdr[g_Enemy[i].tblNo];	// 行動テーブルのアドレスを取得
				
				XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
				XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
				XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換
				
				XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
				XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
				XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している
				
				float nowTime = g_Enemy[i].time - nowNo;	// 時間部分である少数を取り出している
				//Pos = g_Enemy[i].pos;
				
				Pos *= nowTime;								// 現在の移動量を計算している
				Rot *= nowTime;								// 現在の回転量を計算している
				Scl *= nowTime;								// 現在の拡大率を計算している

				// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
				XMStoreFloat3(&g_Enemy[i].pos, nowPos + Pos);

				// 計算して求めた回転量を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].rot, nowRot + Rot);

				// 計算して求めた拡大率を現在の移動テーブルに足している
				XMStoreFloat3(&g_Enemy[i].scl, nowScl + Scl);
				g_Enemy[i].w = TEXTURE_WIDTH * g_Enemy[i].scl.x;
				g_Enemy[i].h = TEXTURE_HEIGHT * g_Enemy[i].scl.y;

				// frameを使て時間経過処理をする
				g_Enemy[i].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
				if ((int)g_Enemy[i].time >= maxNo)			// 登録テーブル最後まで移動したか？
				{
					g_Enemy[i].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
				}

				g_Enemy[i].texNo = tbl[nowNo].texNo;

			}

			if (g_Enemy[TYPE_BLEACH].texNo == 8)
			{
				g_Enemy[TYPE_BLEACH].hpPos = XMFLOAT3(80.0f, 50.0f, 0.0f);
				g_Enemy[TYPE_BLEACH].boxX = 120.0f;
				g_Enemy[TYPE_BLEACH].boxY = 60.0f;
				g_Enemy[TYPE_BLEACH].boxPosX = 60.0f;
				g_Enemy[TYPE_BLEACH].boxPosY = -40.0f;

			}
			else if (g_Enemy[TYPE_BLEACH].texNo == 9)
			{
				g_Enemy[TYPE_BLEACH].hpPos = XMFLOAT3(-20.0f, 50.0f, 0.0f);
				g_Enemy[TYPE_BLEACH].boxX = 120.0f;
				g_Enemy[TYPE_BLEACH].boxY = 60.0f;
				g_Enemy[TYPE_BLEACH].boxPosX = -60.0f;
				g_Enemy[TYPE_BLEACH].boxPosY = -40.0f;

			}


			// 移動が終わったらエネミーとの当たり判定
			{
				PLAYER* player = GetPlayer();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (player[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Enemy[i].pos, g_Enemy[i].w, g_Enemy[i].h,
							player[j].pos, player[j].w, player[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							//player[j].use = FALSE;	// デバッグで一時的に無敵にしておくか
						}
					}
				}
			}


			g_EnemyCnt++;		// 生きてた敵の数
		}
	}


	// エネミー全滅チェック
	if (g_EnemyCnt <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	BG* bg = GetBG();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].pos.x - bg->pos.x;	// エネミーの表示位置X
			float py = g_Enemy[i].pos.y - bg->pos.y;	// エネミーの表示位置Y
			float pw = g_Enemy[i].w;		// エネミーの表示幅
			float ph = g_Enemy[i].h;		// エネミーの表示高さ

			 //アニメーション用
			float tw = 1.0f / g_Enemy[i].patternX;	// テクスチャの幅
			float th = 1.0f / g_Enemy[i].patternY;	// テクスチャの高さ
			float tx = (float)(g_Enemy[i].patternAnim % g_Enemy[i].patternX) * tw;	// テクスチャの左上X座標
			float ty = (float)g_Enemy[i].attack * th;	// テクスチャの左上Y座標

			//float tw = 1.0f;	// テクスチャの幅
			//float th = 1.0f;	// テクスチャの高さ
			//float tx = 0.0f;	// テクスチャの左上X座標
			//float ty = 0.0f;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}


		// ゲージのテスト
		{
			// HPゲージ
			if ((g_Enemy[i].use == TRUE) && (g_Enemy[i].hpGauge == TRUE))			// このエネミーが使われている？
			{

				// 下敷きのゲージ（枠的な物）
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

				//ゲージの位置やテクスチャー座標を反映
				float px = g_Enemy[i].pos.x - g_Enemy[i].hpPos.x - bg->pos.x;		// ゲージの表示位置X
				float py = g_Enemy[i].pos.y - g_Enemy[i].hpPos.y - bg->pos.y;		// ゲージの表示位置Y
				float pw = g_Enemy[i].w / 3;		// ゲージの表示幅
				float ph = 12.0f;		// ゲージの表示高さ

				float tw = 1.0f;	// テクスチャの幅
				float th = 1.0f;	// テクスチャの高さ
				float tx = 0.0f;	// テクスチャの左上X座標
				float ty = 0.0f;	// テクスチャの左上Y座標
				SetSpriteLTColor(g_VertexBuffer,
					px, py, pw, ph,
					tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				//ゲージの位置やテクスチャー座標を反映
				px = g_Enemy[i].pos.x - g_Enemy[i].hpPos.x - bg->pos.x;		// ゲージの表示位置X
				py = g_Enemy[i].pos.y - g_Enemy[i].hpPos.y - bg->pos.y;		// ゲージの表示位置Y
				pw = g_Enemy[i].w / 3;		// ゲージの表示幅
				ph = 12.0f;		// ゲージの表示高さ

				tw = 1.0f;	// テクスチャの幅
				th = 1.0f;	// テクスチャの高さ
				tx = 0.0f;	// テクスチャの左上X座標
				ty = 0.0f;	// テクスチャの左上Y座標


				pw = pw * (float)(g_Enemy[i].hp / g_Enemy[i].hpMax);
				tw = tw * (float)(g_Enemy[i].hp / g_Enemy[i].hpMax);

				//pw_log = (float)(g_Player[i].hp / g_Player[i].hpMax);
				//tw_log = tw;

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);


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
// Enemy構造体の先頭アドレスを取得
//=============================================================================
ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}



//=============================================================================
// EnemyのHP減少
//=============================================================================
void SetDamageEnemy(int damage, int num)
{
		g_Enemy[num].hp -= damage;
	
}


