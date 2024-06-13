//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 銭子軒（チェンズーシェン）
//
//=============================================================================
#include "player.h"
#include "input.h"
#include "bg.h"
#include "bullet.h"

#include "enemy.h"
#include "collision.h"
#include "option.h"
#include "score.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// キャラサイズ
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(7)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(3)		// アニメーションの切り替わるWait値

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// ジャンプ処理
#define	PLAYER_JUMP_CNT_MAX			(60)		// 30フレームで着地する
#define	PLAYER_JUMP_Y_MAX			(180.0f)	// ジャンプの高さ

// 無敵処理
#define	PLAYER_COLLISION_CNT_MAX			(30)		// 30フレームの間無敵



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Character_000.png",
	"data/TEXTURE/Character_001.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/heart_base.png",
	"data/TEXTURE/heart_main.png",
	"data/TEXTURE/heart_base10.png",
	"data/TEXTURE/heart_main10.png",



};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体


static int      g_jumpCnt = 0;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(35.0f, SCREEN_HEIGHT - 90.0f, 0.0f);	// 中心点から表示
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// 移動量

		g_Player[i].dir = CHAR_DIR_RIGHT;					// 下向きにしとくか
		g_Player[i].action = CHAR_ANIM_MOVE;				//アニメの動作の初期化
		g_Player[i].moving = FALSE;							// 移動中フラグ

		// ジャンプの初期化
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		// 攻撃の初期化
		g_Player[i].attack = FALSE;


		//当たり判定の初期化
		g_Player[i].collision = TRUE;
		g_Player[i].collisionCnt = 0;
		g_Player[i].collisionEffect = 1.0f;


		//パラメータの初期化
		g_Player[i].hpMax = 10;
		g_Player[i].hp = g_Player[i].hpMax;

		// 分身用
		g_Player[i].dash = FALSE;
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}




	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{

			//死亡時の更新
			if (g_Player[i].hp == 0)
			{

				SetMode(MODE_RESULT);

			}

			// 地形との当たり判定用に座標のバックアップを取っておく
			XMFLOAT3 pos_old = g_Player[i].pos;

			// 分身用
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			// アニメーション  
			if (g_Player[i].moving == TRUE)
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}


			if (g_Player[i].jump == TRUE)
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// パターンの切り替え
					g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}


			// キー入力で移動 
			{
				float speed = g_Player[i].move.x;

				g_Player[i].moving = FALSE;
				g_Player[i].dash = FALSE;

				if (GetKeyboardPress(DIK_C) || IsButtonPressed(0, BUTTON_A))
				{
					speed *= 4;
					g_Player[i].dash = TRUE;
				}


				if (GetKeyboardPress(DIK_DOWN))
				{
					g_Player[i].pos.y += speed;
					g_Player[i].dir = CHAR_DIR_DOWN;
					g_Player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_UP))
				{
					g_Player[i].pos.y -= speed;
					g_Player[i].dir = CHAR_DIR_UP;
					g_Player[i].moving = TRUE;
				}

				if (GetKeyboardPress(DIK_D))
				{
					g_Player[i].pos.x += speed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].texNo = g_Player[i].dir;
					g_Player[i].moving = TRUE;
				}
				else if (GetKeyboardPress(DIK_A))
				{
					g_Player[i].pos.x -= speed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].texNo = g_Player[i].dir;
					g_Player[i].moving = TRUE;
				}
				 

				// ゲームパッドでで移動処理
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					g_Player[i].pos.y += speed;
					g_Player[i].dir = CHAR_DIR_DOWN;
					g_Player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_UP))
				{
					g_Player[i].pos.y -= speed;
					g_Player[i].dir = CHAR_DIR_UP;
					g_Player[i].moving = TRUE;
				}

				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].pos.x += speed;
					g_Player[i].dir = CHAR_DIR_RIGHT;
					g_Player[i].moving = TRUE;
				}
				else if (IsButtonPressed(0, BUTTON_LEFT))
				{
					g_Player[i].pos.x -= speed;
					g_Player[i].dir = CHAR_DIR_LEFT;
					g_Player[i].moving = TRUE;
				}

				// ジャンプ処理中？
				if (g_Player[i].jump == TRUE)
				{
					float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[i].jumpCnt;
					float y = g_Player[i].jumpYMax * cosf(XM_PI / 2 + angle);
					g_Player[i].jumpY = y;


					g_Player[i].jumpCnt++;
					if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
					{
						g_Player[i].jump = FALSE;
						g_Player[i].action = CHAR_ANIM_MOVE;
						g_Player[i].jumpCnt = 0;
						g_Player[i].jumpY = 0.0f;
					}

				}
				// ジャンプボタン押した？
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_J)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].action = CHAR_ANIM_JUMP;
					g_Player[i].jumpCnt = 0;
					g_Player[i].jumpY = 0.0f;

				}



				// MAP外チェック
				BG* bg = GetBG();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > bg->w)
				{
					g_Player[i].pos.x = bg->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > bg->h)
				{
					g_Player[i].pos.y = bg->h;
				}

				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;

				// 移動が終わったら地面との当たり判定
				{
					GROUND* ground = GetGround();


					// 地面の数分当たり判定を行う
					for (int j = 0; j < TEXTURE_GROUND_MAX; j++)
					{
						// 地面と当たり判定をする
						if (ground[j].use == TRUE)
						{
							XMFLOAT3 Ppos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].jumpY, 0.0f);
							BOOL ans = CollisionBB(Ppos, g_Player[i].w, g_Player[i].h - 30.0f,
								ground[j].pos, ground[j].w, ground[j].h);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
								g_Player[i].pos = pos_old;
							}
						}
					}
				}

				// 移動が終わったら踏み場との当たり判定
				{
					GROUND* step = GetStep();


					// 地面の数分当たり判定を行う
					for (int j = 0; j < STEP_MAX; j++)
					{
						// 地面と当たり判定をする
						if (step[j].use == TRUE)
						{
							XMFLOAT3 Ppos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].jumpY, 0.0f);
							BOOL ans = CollisionBB(Ppos, g_Player[i].w, g_Player[i].h - 30.0f,
								step[j].pos, step[j].w, step[j].h);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
								g_Player[i].pos = pos_old;
							}
						}
					}
				}



				// 移動が終わったらエネミーとの当たり判定
				{
					ENEMY* enemy = GetEnemy();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if ((enemy[j].use == TRUE) && (g_Player[i].collision == TRUE))
						{

							XMFLOAT3 Ppos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].jumpY, 0.0f);
							XMFLOAT3 Epos = XMFLOAT3(enemy[j].pos.x - enemy[j].boxPosX, enemy[j].pos.y - enemy[j].boxPosY, 0.0f);
							BOOL ans = CollisionBB(Ppos, g_Player[i].w - 20.0f, g_Player[i].h,
								Epos, enemy[j].w - enemy[j].boxX, enemy[j].h - enemy[j].boxY);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
								//enemy[j].use = FALSE;
								//AddScore(10);
								SetDamagePlayer(1);
								//SetDamageEnemy(5 ,j);


								g_Player[i].collision = FALSE;


								//g_Player[i].pos = pos_old;

							}
						}
					}

					//無敵状態の持続
					if (g_Player[i].collision == FALSE)
					{
						g_Player[i].collisionCnt++;

						if (g_Player[i].dir == CHAR_DIR_RIGHT)
						{
							g_Player[i].pos.x -= 3.0f;

						}
						else if (g_Player[i].dir == CHAR_DIR_LEFT)
						{
							g_Player[i].pos.x += 3.0f;

						}

						if ((g_Player[i].collisionCnt % 5 == 0) && (g_Player[i].collisionCnt % 10 != 0))
						{
							g_Player[i].collisionEffect -= 0.5f;

						}

						if (g_Player[i].collisionCnt % 10 == 0)
						{
							g_Player[i].collisionEffect += 0.5f;

						}


						if (g_Player[i].collisionCnt > PLAYER_COLLISION_CNT_MAX)
						{
							g_Player[i].collisionCnt = 0;
							g_Player[i].collision = TRUE;

						}

					}

				}


				// 捕獲攻撃処理中？
				if (g_Player[i].attack == TRUE)
				{
					// アニメーション  
					g_Player[i].attackCnt++;
					if ((g_Player[i].attackCnt % ANIM_WAIT) == 0)
					{
						// パターンの切り替え
						g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X;
					}

					if (g_Player[i].patternAnim == TEXTURE_PATTERN_DIVIDE_X - 1)
					{
						g_Player[i].attack = FALSE;
						g_Player[i].action = CHAR_ANIM_MOVE;

						g_Player[i].patternAnim = 0;

					}


				}
				// 捕獲攻撃ボタン押した？
				else if ((g_Player[i].attack == FALSE) && (GetKeyboardTrigger(DIK_O)))
				{
					g_Player[i].attack = TRUE;
					g_Player[i].action = CHAR_ANIM_ATTACK;

					XMFLOAT3 pos = g_Player[i].pos;
					int num = 0;
					pos.y += g_Player[i].jumpY;


					if (g_Player[i].dir == CHAR_DIR_LEFT)
					{
						num = 1;
					}
					else if (g_Player[i].dir == CHAR_DIR_RIGHT)
					{
						num = 0;
					}


					SetBullet(pos, num);


				}

				// 召喚攻撃処理
				if (GetKeyboardTrigger(DIK_P))
				{
					BULLET* bullet = GetBullet();

					XMFLOAT3 pos = g_Player[i].pos;
					int num = 0;

					pos.y += g_Player[i].jumpY;

					if (g_Player[i].dir == CHAR_DIR_LEFT)
					{
						pos.x -= 75.0f;

						if (bullet[i].summonMonster == 8)
						{
							pos.x -= 60.0f;
							pos.y -= 50.0f;

						}

						num = 3;
					}
					else if (g_Player[i].dir == CHAR_DIR_RIGHT)
					{
						pos.x += 75.0f;

						if (bullet[i].summonMonster == 8)
						{
							pos.x -= -60.0f;
							pos.y -= 50.0f;

						}


						num = 2;
					}

					if (bullet[i].summonMonster != 0)
					{
						SetBullet(pos, num);

					}
				}
				if (IsButtonTriggered(0, BUTTON_B))
				{
					XMFLOAT3 pos = g_Player[i].pos;

					pos.y += g_Player[i].jumpY;

					if (g_Player[i].dir == CHAR_DIR_LEFT)
					{
						pos.x -= 75.0f;
					}
					else if (g_Player[i].dir == CHAR_DIR_RIGHT)
					{
						pos.x += 75.0f;
					}


					//SetBullet(pos);
				}

			}
		}
	}




	// 現状をセーブする
	if (GetKeyboardTrigger(DIK_BACK))
	{
		SaveData();
		SetBack(MODE_GAME);
		SetMode(MODE_OPTION);
	}


#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// このプレイヤーが使われている？
		{									// Yes

			// プレイヤーの分身を描画
			if (g_Player[i].dash)
			{	// ダッシュ中だけ分身処理
				DrawPlayerOffset(i);
			}

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//プレイヤーの位置やテクスチャー座標を反映
			float px = g_Player[i].pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = g_Player[i].pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = g_Player[i].w;		// プレイヤーの表示幅
			float ph = g_Player[i].h;		// プレイヤーの表示高さ

			py += g_Player[i].jumpY;		// ジャンプ中の高さを足す

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)g_Player[i].action * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_Player[i].collisionEffect),
				g_Player[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

		// HPゲージ
		{

			//ゲージの位置やテクスチャー座標を反映
			float px = 10.0f;		// ゲージの表示位置X
			float py = 10.0f;		// ゲージの表示位置Y
			float pw = 300.0f;		// ゲージの表示幅
			float ph = 30.0f;		// ゲージの表示高さ

			float tw = 1.0f;	// テクスチャの幅
			float th = 1.0f;	// テクスチャの高さ
			float tx = 0.0f;	// テクスチャの左上X座標
			float ty = 0.0f;	// テクスチャの左上Y座標



			pw = pw * (float)(g_Player[i].hp / g_Player[i].hpMax);
			tw = tw * (float)(g_Player[i].hp / g_Player[i].hpMax);

			//pw_log = (float)(g_Player[i].hp / g_Player[i].hpMax);
			//tw_log = tw;

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);





			// 下敷きのゲージ（枠的な物）
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			//ゲージの位置やテクスチャー座標を反映
			px = 10.0f;		// ゲージの表示位置X
			py = 10.0f;		// ゲージの表示位置Y
			pw = 300.0f;		// ゲージの表示幅
			ph = 30.0f;		// ゲージの表示高さ

			tw = 1.0f;	// テクスチャの幅
			th = 1.0f;	// テクスチャの高さ
			tx = 0.0f;	// テクスチャの左上X座標
			ty = 0.0f;	// テクスチャの左上Y座標

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


//=============================================================================
// Player構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - bg->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - bg->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;							// プレイヤーの表示幅
		float ph = g_Player[no].h;							// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// PlayerのHP減少
//=============================================================================
void SetDamagePlayer(int damage) 
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].hp -= damage;

	}
}