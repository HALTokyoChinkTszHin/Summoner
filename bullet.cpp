//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "score.h"
#include "bg.h"
#include "effect.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(200)	// キャラサイズ
#define TEXTURE_HEIGHT				(200)	// 
#define TEXTURE_MAX					(12)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// アニメパターンのテクスチャ内分割数（X)
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

static char *g_TexturName[] = {
	"data/TEXTURE/catch_attack_000.png",
	"data/TEXTURE/catch_attack_001.png",
	"data/TEXTURE/Summon_000_Toad_R.png",
	"data/TEXTURE/Summon_001_Toad_L.png",
	"data/TEXTURE/Summon_002_Goblin_R.png",
	"data/TEXTURE/Summon_003_Goblin_L.png",
	"data/TEXTURE/Summon_004_Mushroom_R.png",
	"data/TEXTURE/Summon_005_Mushroom_L.png",
	"data/TEXTURE/Summon_006_Bleach_R.png",
	"data/TEXTURE/Summon_007_Bleach_L.png",


};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static BULLET	g_Bullet[BULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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


	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use   = FALSE;			// 未使用（発射されていない弾）
		g_Bullet[i].w     = TEXTURE_WIDTH;
		g_Bullet[i].h     = TEXTURE_HEIGHT;
		g_Bullet[i].pos   = XMFLOAT3(300, 300.0f, 0.0f);
		g_Bullet[i].rot   = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;

		g_Bullet[i].collision = TRUE;

		g_Bullet[i].catchAttack = TRUE;
		g_Bullet[i].summonMonster = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, 0.0f, 0.0f);	// 移動量を初期化
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{
	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			if (g_Bullet[i].patternAnim == ANIM_PATTERN_NUM - 1)
			{
				g_Bullet[i].use = false;


				g_Bullet[i].patternAnim = 0;

			}

			//捕獲攻撃の処理
			{

				if (g_Bullet[i].catchAttack == TRUE)
				{

					// バレットの移動処理
					XMVECTOR pos = XMLoadFloat3(&g_Bullet[i].pos);
					XMVECTOR move = XMLoadFloat3(&g_Bullet[i].move);

					if (g_Bullet[i].texNo == 0)
					{

						pos += move;
					}
					else if (g_Bullet[i].texNo == 1)
					{

						pos -= move;

					}


					XMStoreFloat3(&g_Bullet[i].pos, pos);

					// 画面外まで進んだ？
					BG* bg = GetBG();
					if (g_Bullet[i].pos.y < (-g_Bullet[i].h / 2))		// 自分の大きさを考慮して画面外か判定している
					{
						g_Bullet[i].use = false;
					}
					if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h / 2))	// 自分の大きさを考慮して画面外か判定している
					{
						g_Bullet[i].use = false;
					}

				}




			}

			// 当たり判定処理
			{
				ENEMY* enemy = GetEnemy();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if ((enemy[j].use == TRUE) && (g_Bullet[i].collision == TRUE))
					{
						//XMFLOAT3 enemy_pos = XMFLOAT3(enemy[j].pos.x - bg->pos.x, enemy[j].pos.y - bg->pos.y, enemy[j].pos.z);
						
						XMFLOAT3 Ppos = XMFLOAT3(g_Bullet[i].pos.x, g_Bullet[i].pos.y , 0.0f);
						XMFLOAT3 Epos = XMFLOAT3(enemy[j].pos.x - enemy[j].boxPosX, enemy[j].pos.y - enemy[j].boxPosY, 0.0f);
						BOOL ans = CollisionBB(Ppos, g_Bullet[i].w - 15.0f, g_Bullet[i].h,
							Epos, enemy[j].w - enemy[j].boxX, enemy[j].h - enemy[j].boxY);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							//捕獲攻撃
							if (g_Bullet[i].catchAttack == TRUE)
							{
								SetDamageEnemy(10 ,j);

								g_Bullet[i].collision = FALSE;


								if (enemy[j].hp <= 0)
								{
									// エフェクト発生
									SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);

									enemy[j].use = FALSE;
									g_Bullet[i].summonMonster = enemy[j].monster;

									AddScore(15);

								}
							}

							if (g_Bullet[i].catchAttack == FALSE)
							//召喚攻撃
							{
								switch (g_Bullet[i].summonMonster)
								{
								case 2:
									SetDamageEnemy(80, j);

									break;

								case 4:
									SetDamageEnemy(50, j);

									break;

								case 6:
									SetDamageEnemy(25, j);

									break;

								case 8:
									SetDamageEnemy(150, j);

									break;

								}

								g_Bullet[i].collision = FALSE;

								if (enemy[j].hp < 0)
								{
									enemy[j].use = FALSE;

									AddScore(10);


								}

							}


						}


					}
				}
			}


			bulletCount++;
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - bg->pos.x;	// バレットの表示位置X
			float py = g_Bullet[i].pos.y - bg->pos.y;	// バレットの表示位置Y
			float pw = g_Bullet[i].w;		// バレットの表示幅
			float ph = g_Bullet[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// 契約中の召喚獣を描画
		{
			if (g_Bullet[i].summonMonster != 0)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].summonMonster]);

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer,
					850.0f, 45.0f, 58.0f * 2, 58.0f * 2,
					0.0f, 0.0f, 1.0f / 8, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

			}
		}

	}




}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet(XMFLOAT3 pos ,int num)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			PLAYER* player = GetPlayer();

			g_Bullet[i].texNo = g_Bullet[i].summonMonster;
			g_Bullet[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet[i].pos = pos;			// 座標をセット
			g_Bullet[i].collision = TRUE;


			if ((num == 0) || (num == 1))
			{
				g_Bullet[i].w = 75.0f;
				g_Bullet[i].h = 75.0f;

				g_Bullet[i].catchAttack = TRUE;
			
				if (player[0].dir == CHAR_DIR_RIGHT)
				{
					g_Bullet[i].texNo = 0;


				}
				else if (player[0].dir == CHAR_DIR_LEFT)
				{
					g_Bullet[i].texNo = 1;


				}


			}
			else
			{
				g_Bullet[i].w = TEXTURE_WIDTH;
				g_Bullet[i].h = TEXTURE_HEIGHT;

				g_Bullet[i].catchAttack = FALSE;
				if (player[0].dir == CHAR_DIR_RIGHT)
				{
					g_Bullet[i].texNo = g_Bullet[i].summonMonster;

				}
				else if (player[0].dir == CHAR_DIR_LEFT)
				{
					g_Bullet[i].texNo = g_Bullet[i].summonMonster + 1;


				}


			}




			return;							// 1発セットしたので終了する
		}
	}
}

