//=============================================================================
//
// �`���[�g����ʏ��� [tutorial.cpp]
// Author : �K�q���i�`�F���Y�[�V�F���j
//
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "fade.h"
#include "option.h"
#include "sound.h"

#include "file.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(37)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(100)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(100)			// 

#define ANIM_WAIT					(5)		// �A�j���[�V�����̐؂�ւ��Wait�l



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static OBJ	g_Object[OBJ_MAX];	// �`���[�g���A���\����
static OBJ	g_Monster[8];	// �`���[�g���A���\����


static BOOL						g_Load = FALSE;

int								page;

BOOL							first;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
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


	// �ϐ��̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 25;
	page = 0;

	first = TRUE;

	// �`���[�g���A���\���̂̏�����
	g_Object[KEY_D].pos = XMFLOAT3(50.0f, 400.0f, 0.0f);	// ���S�_����\��
	g_Object[KEY_D].patternX = 1;

	g_Object[TUTORIAL_MOVE_R].pos = XMFLOAT3(150.0f, 400.0f, 0.0f);	// ���S�_����\��
	g_Object[TUTORIAL_MOVE_R].patternX = 8;

	g_Object[KEY_A].pos = XMFLOAT3(50.0f, 450.0f, 0.0f);	// ���S�_����\��
	g_Object[KEY_A].patternX = 1;

	g_Object[TUTORIAL_MOVE_L].pos = XMFLOAT3(150.0f, 450.0f, 0.0f);	// ���S�_����\��
	g_Object[TUTORIAL_MOVE_L].patternX = 8;

	g_Object[KEY_J].pos = XMFLOAT3(300.0f, 425.0f, 0.0f);	// ���S�_����\��
	g_Object[KEY_J].patternX = 1;

	g_Object[TUTORIAL_JUMP].pos = XMFLOAT3(400.0f, 425.0f, 0.0f);	// ���S�_����\��
	g_Object[TUTORIAL_JUMP].patternX = 8;

	g_Object[KEY_O].pos = XMFLOAT3(600.0f, 125.0f, 0.0f);	// ���S�_����\��
	g_Object[KEY_O].patternX = 1;

	g_Object[TUTORIAL_ATTACK].pos = XMFLOAT3(650.0f, 200.0f, 0.0f);	// ���S�_����\��
	g_Object[TUTORIAL_ATTACK].patternX = 8;

	g_Object[TUTORIAL_CATCHATTACK].pos = XMFLOAT3(750.0f, 200.0f, 0.0f);	// ���S�_����\��
	g_Object[TUTORIAL_CATCHATTACK].patternX = 8;

	g_Object[KEY_P].pos = XMFLOAT3(625.0f, 350.0f, 0.0f);	// ���S�_����\��
	g_Object[KEY_P].patternX = 1;

	g_Object[TUTORIAL_SUMMON].pos = XMFLOAT3(750.0f, 325.0f, 0.0f);	// ���S�_����\��
	g_Object[TUTORIAL_SUMMON].patternX = 8;

	g_Object[KEY_DEL].pos = XMFLOAT3(750.0f, 450.0f, 0.0f);	// ���S�_����\��
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

	g_Monster[0].pos = XMFLOAT3(200.0f, 80.0f, 0.0f);	// ���S�_����\��
	g_Monster[1].pos = XMFLOAT3(500.0f, 80.0f, 0.0f);	// ���S�_����\��
	g_Monster[2].pos = XMFLOAT3(200.0f, 80.0f, 0.0f);	// ���S�_����\��
	g_Monster[3].pos = XMFLOAT3(500.0f, 80.0f, 0.0f);	// ���S�_����\��
	g_Monster[4].pos = XMFLOAT3(200.0f, 80.0f, 0.0f);	// ���S�_����\��
	g_Monster[5].pos = XMFLOAT3(500.0f, 80.0f, 0.0f);	// ���S�_����\��
	g_Monster[6].pos = XMFLOAT3(250.0f, 20.0f, 0.0f);	// ���S�_����\��
	g_Monster[7].pos = XMFLOAT3(550.0f, 20.0f, 0.0f);	// ���S�_����\��

	//����m�F
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateTutorial(void)
{
	for (int i = 0; i < OBJ_MAX; i++)
	{
		// �A�j���[�V����  
		if (g_Object[i].use == TRUE)
		{
			g_Object[i].countAnim += 1.0f;
			if (g_Object[i].countAnim > ANIM_WAIT)
			{
				g_Object[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Object[i].patternAnim = g_Object[i].patternX + ((g_Object[i].patternAnim + 1) % g_Object[i].patternX);
			}
		}


	}

	for (int i = 0; i < 8; i++)
	{
		// �A�j���[�V����  
		if (g_Monster[i].use == TRUE)
		{
			g_Monster[i].countAnim += 1.0f;
			if (g_Monster[i].countAnim > ANIM_WAIT)
			{
				g_Monster[i].countAnim = 0.0f;
				// �p�^�[���̐؂�ւ�
				g_Monster[i].patternAnim = g_Monster[i].patternX + ((g_Monster[i].patternAnim + 1) % g_Monster[i].patternX);
			}
		}


	}



	
	// �Q�[���p�b�h�œ��͏���
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


#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
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


	// �I�v�V�����̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 800.0f, 25.0f, 25.0f, 25.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 900.0f, 25.0f, 25.0f, 25.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	if (first == TRUE)
	{
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[35]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 825.0f, 20.0f, 60.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}

	//BACK�̕`��
	{	
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 800.0f, 55.0f, 40.0f, 40.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
		
	
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[36]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 850.0f, 50.0f, 70.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}



	// �`���[�g���A���A�j����`��
	{

		for (int i = 0; i < OBJ_MAX; i++)
		{
			if (g_Object[i].use == TRUE)		// ����OBJ���g���Ă���H
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Object[i].texNo]);

				float px = g_Object[i].pos.x;	// OBJ�̕\���ʒuX
				float py = g_Object[i].pos.y;	// OBJ�̕\���ʒuY
				float pw = g_Object[i].w;		// OBJ�̕\����
				float ph = g_Object[i].h;		// OBJ�̕\������


				//float tw = 1.0f;	// �e�N�X�`���̕�
				//float th = 1.0f;	// �e�N�X�`���̍���
				//float tx = 0.0f;	// �e�N�X�`���̍���X���W
				//float ty = 0.0f;	// �e�N�X�`���̍���Y���W

			// �A�j���[�V�����p
				float tw = 1.0f / g_Object[i].patternX;	// �e�N�X�`���̕�
				float th = 1.0f / g_Object[i].patternY;	// �e�N�X�`���̍���
				float tx = (float)(g_Object[i].patternAnim % g_Object[i].patternX) * tw;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

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


	// �`���[�g���A�������X�^�[��`��
	{

		for (int i = 0; i < OBJ_MAX; i++)
		{
			if (g_Monster[i].use == TRUE)		// ����OBJ���g���Ă���H
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Monster[i].texNo]);

				float px = g_Monster[i].pos.x;	// OBJ�̕\���ʒuX
				float py = g_Monster[i].pos.y;	// OBJ�̕\���ʒuY
				float pw = g_Monster[i].w;		// OBJ�̕\����
				float ph = g_Monster[i].h;		// OBJ�̕\������


			// �A�j���[�V�����p
				float tw = 1.0f / g_Monster[i].patternX;	// �e�N�X�`���̕�
				float th = 1.0f / g_Monster[i].patternY;	// �e�N�X�`���̍���
				float tx = (float)(g_Monster[i].patternAnim % g_Monster[i].patternX) * tw;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

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



}



