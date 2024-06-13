//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : �K�q���i�`�F���Y�[�V�F���j
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(200)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(200)	// 
#define TEXTURE_MAX					(12)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(8)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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

static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static BULLET	g_Bullet[BULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
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


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use   = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
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

		g_Bullet[i].move = XMFLOAT3(BULLET_SPEED, 0.0f, 0.0f);	// �ړ��ʂ�������
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			if (g_Bullet[i].patternAnim == ANIM_PATTERN_NUM - 1)
			{
				g_Bullet[i].use = false;


				g_Bullet[i].patternAnim = 0;

			}

			//�ߊl�U���̏���
			{

				if (g_Bullet[i].catchAttack == TRUE)
				{

					// �o���b�g�̈ړ�����
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

					// ��ʊO�܂Ői�񂾁H
					BG* bg = GetBG();
					if (g_Bullet[i].pos.y < (-g_Bullet[i].h / 2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
					{
						g_Bullet[i].use = false;
					}
					if (g_Bullet[i].pos.y > (bg->h + g_Bullet[i].h / 2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
					{
						g_Bullet[i].use = false;
					}

				}




			}

			// �����蔻�菈��
			{
				ENEMY* enemy = GetEnemy();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if ((enemy[j].use == TRUE) && (g_Bullet[i].collision == TRUE))
					{
						//XMFLOAT3 enemy_pos = XMFLOAT3(enemy[j].pos.x - bg->pos.x, enemy[j].pos.y - bg->pos.y, enemy[j].pos.z);
						
						XMFLOAT3 Ppos = XMFLOAT3(g_Bullet[i].pos.x, g_Bullet[i].pos.y , 0.0f);
						XMFLOAT3 Epos = XMFLOAT3(enemy[j].pos.x - enemy[j].boxPosX, enemy[j].pos.y - enemy[j].boxPosY, 0.0f);
						BOOL ans = CollisionBB(Ppos, g_Bullet[i].w - 15.0f, g_Bullet[i].h,
							Epos, enemy[j].w - enemy[j].boxX, enemy[j].h - enemy[j].boxY);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							//�ߊl�U��
							if (g_Bullet[i].catchAttack == TRUE)
							{
								SetDamageEnemy(10 ,j);

								g_Bullet[i].collision = FALSE;


								if (enemy[j].hp <= 0)
								{
									// �G�t�F�N�g����
									SetEffect(enemy[j].pos.x, enemy[j].pos.y, 30);

									enemy[j].use = FALSE;
									g_Bullet[i].summonMonster = enemy[j].monster;

									AddScore(15);

								}
							}

							if (g_Bullet[i].catchAttack == FALSE)
							//�����U��
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
// �`�揈��
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �o���b�g�̕\����
			float ph = g_Bullet[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

		// �_�񒆂̏����b��`��
		{
			if (g_Bullet[i].summonMonster != 0)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].summonMonster]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLTColor(g_VertexBuffer,
					850.0f, 45.0f, 58.0f * 2, 58.0f * 2,
					0.0f, 0.0f, 1.0f / 8, 1.0f,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}
		}

	}




}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBullet(XMFLOAT3 pos ,int num)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			PLAYER* player = GetPlayer();

			g_Bullet[i].texNo = g_Bullet[i].summonMonster;
			g_Bullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos = pos;			// ���W���Z�b�g
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




			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

