//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : �K�q���i�`�F���Y�[�V�F���j
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100)	// 
#define TEXTURE_MAX					(7)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(3)		// �A�j���[�V�����̐؂�ւ��Wait�l

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X				(SCREEN_WIDTH/2)
#define PLAYER_DISP_Y				(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT)

// �W�����v����
#define	PLAYER_JUMP_CNT_MAX			(60)		// 30�t���[���Œ��n����
#define	PLAYER_JUMP_Y_MAX			(180.0f)	// �W�����v�̍���

// ���G����
#define	PLAYER_COLLISION_CNT_MAX			(30)		// 30�t���[���̊Ԗ��G



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Character_000.png",
	"data/TEXTURE/Character_001.png",
	"data/TEXTURE/shadow000.jpg",
	"data/TEXTURE/heart_base.png",
	"data/TEXTURE/heart_main.png",
	"data/TEXTURE/heart_base10.png",
	"data/TEXTURE/heart_main10.png",



};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];	// �v���C���[�\����


static int      g_jumpCnt = 0;
static int		g_jump[PLAYER_JUMP_CNT_MAX] =
{
	-15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5,-4,-3,-2,-1,
	  1,   2,   3,   4,   5,   6,  7,  8,  9, 10, 11,12,13,14,15
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].use = TRUE;
		g_Player[i].pos = XMFLOAT3(35.0f, SCREEN_HEIGHT - 90.0f, 0.0f);	// ���S�_����\��
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].w = TEXTURE_WIDTH;
		g_Player[i].h = TEXTURE_HEIGHT;
		g_Player[i].texNo = 0;

		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X;

		g_Player[i].move = XMFLOAT3(4.0f, 0.0f, 0.0f);		// �ړ���

		g_Player[i].dir = CHAR_DIR_RIGHT;					// �������ɂ��Ƃ���
		g_Player[i].action = CHAR_ANIM_MOVE;				//�A�j���̓���̏�����
		g_Player[i].moving = FALSE;							// �ړ����t���O

		// �W�����v�̏�����
		g_Player[i].jump = FALSE;
		g_Player[i].jumpCnt = 0;
		g_Player[i].jumpY = 0.0f;
		g_Player[i].jumpYMax = PLAYER_JUMP_Y_MAX;

		// �U���̏�����
		g_Player[i].attack = FALSE;


		//�����蔻��̏�����
		g_Player[i].collision = TRUE;
		g_Player[i].collisionCnt = 0;
		g_Player[i].collisionEffect = 1.0f;


		//�p�����[�^�̏�����
		g_Player[i].hpMax = 10;
		g_Player[i].hp = g_Player[i].hpMax;

		// ���g�p
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
// �I������
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
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{

			//���S���̍X�V
			if (g_Player[i].hp == 0)
			{

				SetMode(MODE_RESULT);

			}

			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			XMFLOAT3 pos_old = g_Player[i].pos;

			// ���g�p
			for (int j = PLAYER_OFFSET_CNT - 1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j - 1];
			}
			g_Player[i].offset[0] = pos_old;

			// �A�j���[�V����  
			if (g_Player[i].moving == TRUE)
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}


			if (g_Player[i].jump == TRUE)
			{
				g_Player[i].countAnim += 1.0f;
				if (g_Player[i].countAnim > ANIM_WAIT)
				{
					g_Player[i].countAnim = 0.0f;
					// �p�^�[���̐؂�ւ�
					g_Player[i].patternAnim = (g_Player[i].dir * TEXTURE_PATTERN_DIVIDE_X) + ((g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X);
				}
			}


			// �L�[���͂ňړ� 
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
				 

				// �Q�[���p�b�h�łňړ�����
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

				// �W�����v�������H
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
				// �W�����v�{�^���������H
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardTrigger(DIK_J)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].action = CHAR_ANIM_JUMP;
					g_Player[i].jumpCnt = 0;
					g_Player[i].jumpY = 0.0f;

				}



				// MAP�O�`�F�b�N
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

				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				bg->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;
				if (bg->pos.x < 0) bg->pos.x = 0;
				if (bg->pos.x > bg->w - SCREEN_WIDTH) bg->pos.x = bg->w - SCREEN_WIDTH;

				bg->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;
				if (bg->pos.y < 0) bg->pos.y = 0;
				if (bg->pos.y > bg->h - SCREEN_HEIGHT) bg->pos.y = bg->h - SCREEN_HEIGHT;

				// �ړ����I�������n�ʂƂ̓����蔻��
				{
					GROUND* ground = GetGround();


					// �n�ʂ̐��������蔻����s��
					for (int j = 0; j < TEXTURE_GROUND_MAX; j++)
					{
						// �n�ʂƓ����蔻�������
						if (ground[j].use == TRUE)
						{
							XMFLOAT3 Ppos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].jumpY, 0.0f);
							BOOL ans = CollisionBB(Ppos, g_Player[i].w, g_Player[i].h - 30.0f,
								ground[j].pos, ground[j].w, ground[j].h);
							// �������Ă���H
							if (ans == TRUE)
							{
								// �����������̏���
								g_Player[i].pos = pos_old;
							}
						}
					}
				}

				// �ړ����I������瓥�ݏ�Ƃ̓����蔻��
				{
					GROUND* step = GetStep();


					// �n�ʂ̐��������蔻����s��
					for (int j = 0; j < STEP_MAX; j++)
					{
						// �n�ʂƓ����蔻�������
						if (step[j].use == TRUE)
						{
							XMFLOAT3 Ppos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].jumpY, 0.0f);
							BOOL ans = CollisionBB(Ppos, g_Player[i].w, g_Player[i].h - 30.0f,
								step[j].pos, step[j].w, step[j].h);
							// �������Ă���H
							if (ans == TRUE)
							{
								// �����������̏���
								g_Player[i].pos = pos_old;
							}
						}
					}
				}



				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				{
					ENEMY* enemy = GetEnemy();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if ((enemy[j].use == TRUE) && (g_Player[i].collision == TRUE))
						{

							XMFLOAT3 Ppos = XMFLOAT3(g_Player[i].pos.x, g_Player[i].pos.y + g_Player[i].jumpY, 0.0f);
							XMFLOAT3 Epos = XMFLOAT3(enemy[j].pos.x - enemy[j].boxPosX, enemy[j].pos.y - enemy[j].boxPosY, 0.0f);
							BOOL ans = CollisionBB(Ppos, g_Player[i].w - 20.0f, g_Player[i].h,
								Epos, enemy[j].w - enemy[j].boxX, enemy[j].h - enemy[j].boxY);
							// �������Ă���H
							if (ans == TRUE)
							{
								// �����������̏���
								//enemy[j].use = FALSE;
								//AddScore(10);
								SetDamagePlayer(1);
								//SetDamageEnemy(5 ,j);


								g_Player[i].collision = FALSE;


								//g_Player[i].pos = pos_old;

							}
						}
					}

					//���G��Ԃ̎���
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


				// �ߊl�U���������H
				if (g_Player[i].attack == TRUE)
				{
					// �A�j���[�V����  
					g_Player[i].attackCnt++;
					if ((g_Player[i].attackCnt % ANIM_WAIT) == 0)
					{
						// �p�^�[���̐؂�ւ�
						g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % TEXTURE_PATTERN_DIVIDE_X;
					}

					if (g_Player[i].patternAnim == TEXTURE_PATTERN_DIVIDE_X - 1)
					{
						g_Player[i].attack = FALSE;
						g_Player[i].action = CHAR_ANIM_MOVE;

						g_Player[i].patternAnim = 0;

					}


				}
				// �ߊl�U���{�^���������H
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

				// �����U������
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




	// ������Z�[�u����
	if (GetKeyboardTrigger(DIK_BACK))
	{
		SaveData();
		SetBack(MODE_GAME);
		SetMode(MODE_OPTION);
	}


#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetBG();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// ���̃v���C���[���g���Ă���H
		{									// Yes

			// �v���C���[�̕��g��`��
			if (g_Player[i].dash)
			{	// �_�b�V�����������g����
				DrawPlayerOffset(i);
			}

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

			//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Player[i].pos.x - bg->pos.x;	// �v���C���[�̕\���ʒuX
			float py = g_Player[i].pos.y - bg->pos.y;	// �v���C���[�̕\���ʒuY
			float pw = g_Player[i].w;		// �v���C���[�̕\����
			float ph = g_Player[i].h;		// �v���C���[�̕\������

			py += g_Player[i].jumpY;		// �W�����v���̍����𑫂�

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)g_Player[i].action * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, g_Player[i].collisionEffect),
				g_Player[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

		// HP�Q�[�W
		{

			//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = 10.0f;		// �Q�[�W�̕\���ʒuX
			float py = 10.0f;		// �Q�[�W�̕\���ʒuY
			float pw = 300.0f;		// �Q�[�W�̕\����
			float ph = 30.0f;		// �Q�[�W�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W



			pw = pw * (float)(g_Player[i].hp / g_Player[i].hpMax);
			tw = tw * (float)(g_Player[i].hp / g_Player[i].hpMax);

			//pw_log = (float)(g_Player[i].hp / g_Player[i].hpMax);
			//tw_log = tw;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);





			// ���~���̃Q�[�W�i�g�I�ȕ��j
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

			//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
			px = 10.0f;		// �Q�[�W�̕\���ʒuX
			py = 10.0f;		// �Q�[�W�̕\���ʒuY
			pw = 300.0f;		// �Q�[�W�̕\����
			ph = 30.0f;		// �Q�[�W�̕\������

			tw = 1.0f;	// �e�N�X�`���̕�
			th = 1.0f;	// �e�N�X�`���̍���
			tx = 0.0f;	// �e�N�X�`���̍���X���W
			ty = 0.0f;	// �e�N�X�`���̍���Y���W

		   // �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);





		}

	}


}


//=============================================================================
// Player�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}


//=============================================================================
// �v���C���[�̕��g��`��
//=============================================================================
void DrawPlayerOffset(int no)
{
	BG* bg = GetBG();
	float alpha = 0.0f;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);

	for (int j = PLAYER_OFFSET_CNT - 1; j >= 0; j--)
	{
		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[no].offset[j].x - bg->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Player[no].offset[j].y - bg->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Player[no].w;							// �v���C���[�̕\����
		float ph = g_Player[no].h;							// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// Player��HP����
//=============================================================================
void SetDamagePlayer(int damage) 
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].hp -= damage;

	}
}