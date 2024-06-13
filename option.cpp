//=============================================================================
//
// �I�v�V������ʏ��� [option.cpp]
// Author : �K�q���i�`�F���Y�[�V�F���j
//
//=============================================================================
#include "option.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

#include "file.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(18)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(236 * 2)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(66 * 2)			// 

#define MAGIC_CIRCLE_ROT			(0.01f)			//���@�w�̉�]���x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

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


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Load = FALSE;

int								backTo;

static OPTION	g_Option[OPTION_MAX];

static LANGUAGE	g_Language[LANGUAGE_MAX];
BOOL		languageChoose = LANGUAGE_JAPANESE;

BOOL		optionSummon;
BOOL		optionSetting;

int			volumeMax;
int			volume;


XMFLOAT3	optionCursorPos;			// �|���S���̍��W
BOOL		optionCursorUse;			// true:�g���Ă���  false:���g�p
float		optionCursorW, optionCursorH;			// ���ƍ���
int			optionCursorTexNo;			// �e�N�X�`���ԍ�
BOOL		optionCursorPosition;





//=============================================================================
// ����������
//=============================================================================
HRESULT InitOption(void)
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
	g_TexNo = 0;

	optionSummon = FALSE;
	optionSetting = TRUE;


	//�I�v�V�����̏�����
	for (int i = 0; i < OPTION_MAX; i++)
	{
		g_Option[i].use = TRUE;
		g_Option[i].pos = XMFLOAT3(SCREEN_WIDTH / 2.5, SCREEN_HEIGHT / 2.5 + i * 60.0f + 50.0f, 0.0f);	// ���S�_����\��
		g_Option[i].w = 210.0f;
		g_Option[i].h = 70.0f;
		g_Option[i].texNo = 5 + i;

	}

	g_Option[OPTION_BGM].w = 120.0f;


	//����̏�����
	for (int i = 0; i < LANGUAGE_MAX; i++)
	{
		g_Language[i].pos = XMFLOAT3(SCREEN_WIDTH / 1.5, SCREEN_HEIGHT / 2.5 + 50.0f, 0.0f);	// ���S�_����\��
		g_Language[i].w = 150.0f;
		g_Language[i].h = 70.0f;
		g_Language[i].texNo = 7 + i;

	}
	g_Language[LANGUAGE_ENGLISH].use = FALSE;
	g_Language[LANGUAGE_JAPANESE].use = FALSE;
	g_Language[LANGUAGE_CHINESE].use = FALSE;

	languageChoose = GetLanguage();

	g_Language[languageChoose].use = TRUE;

	//���ʂ̏�����
	volumeMax = 10;
	volume = 2;

	SetVolume(volume);

	//�J�[�\���̏�����
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateOption(void)
{

	
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




	// �J�[�\�����ړ�����
	if (optionCursorUse == TRUE)
	{
		// �J�[�\�����ړ�����
		if (GetKeyboardTrigger(DIK_W))
		{
			optionCursorPosition = (++optionCursorPosition + OPTION_MAX) % OPTION_MAX;

		}
		else if (GetKeyboardTrigger(DIK_S))
		{
			optionCursorPosition = (--optionCursorPosition + OPTION_MAX) % OPTION_MAX;

		}

		optionCursorPos.y = g_Option[optionCursorPosition].pos.y;			// �|���S���̍��W

		{//�J�[�\���̈ʒu�ɂ���Ē�������
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









#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawOption(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �Ŕ�`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, SCREEN_WIDTH / 5, SCREEN_HEIGHT / 8, 426.0f * 1.5, 340.0f * 1.5, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	if (optionSetting == TRUE)
	{
		// �ݒ�̃I�v�V������`��
		{
			for (int i = 0; i < OPTION_MAX; i++)
			{
				if (g_Option[i].use == TRUE)
				{
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Option[i].texNo]);

					//�X�^�[�g�̈ʒu��e�N�X�`���[���W�𔽉f
					float px = g_Option[i].pos.x;	// �I�v�V�����̕\���ʒuX
					float py = g_Option[i].pos.y;	// �I�v�V�����̕\���ʒuY
					float pw = g_Option[i].w;		// �I�v�V�����̕\����
					float ph = g_Option[i].h;		// �I�v�V�����̕\������

					float tw = 1.0f;	// �e�N�X�`���̕�
					float th = 1.0f;	// �e�N�X�`���̍���
					float tx = 0.0f;	// �e�N�X�`���̍���X���W
					float ty = 0.0f;	// �e�N�X�`���̍���Y���W

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
						0.0f);

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);

				}

			}


		}

		// ����̃I�v�V������`��
		{
			for (int i = 0; i < LANGUAGE_MAX; i++)
			{
				if (g_Language[i].use == TRUE)
				{
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Language[i].texNo]);

					//�X�^�[�g�̈ʒu��e�N�X�`���[���W�𔽉f
					float px = g_Language[i].pos.x;	// �I�v�V�����̕\���ʒuX
					float py = g_Language[i].pos.y;	// �I�v�V�����̕\���ʒuY
					float pw = g_Language[i].w;		// �I�v�V�����̕\����
					float ph = g_Language[i].h;		// �I�v�V�����̕\������

					float tw = 1.0f;	// �e�N�X�`���̕�
					float th = 1.0f;	// �e�N�X�`���̍���
					float tx = 0.0f;	// �e�N�X�`���̍���X���W
					float ty = 0.0f;	// �e�N�X�`���̍���Y���W

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
						0.0f);

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);

				}

			}


		}

		// BGM�Q�[�W
		{
			// ���~���̃Q�[�W�i�g�I�ȕ��j
			//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = SCREEN_WIDTH / 2;		// �Q�[�W�̕\���ʒuX
			float py = SCREEN_HEIGHT / 2.5 + 100.0f;		// �Q�[�W�̕\���ʒuY
			float pw = 300.0f;		// �Q�[�W�̕\����
			float ph = 30.0f;		// �Q�[�W�̕\������

			float tw = 1.0f;	// �e�N�X�`���̕�
			float th = 1.0f;	// �e�N�X�`���̍���
			float tx = 0.0f;	// �e�N�X�`���̍���X���W
			float ty = 0.0f;	// �e�N�X�`���̍���Y���W



			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);





			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

			//�Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
			px = SCREEN_WIDTH / 2;		// �Q�[�W�̕\���ʒuX
			py = SCREEN_HEIGHT / 2.5 + 100.0f;		// �Q�[�W�̕\���ʒuY
			pw = 300.0f;		// �Q�[�W�̕\����
			ph = 30.0f;		// �Q�[�W�̕\������

			tw = 1.0f;	// �e�N�X�`���̕�
			th = 1.0f;	// �e�N�X�`���̍���
			tx = 0.0f;	// �e�N�X�`���̍���X���W
			ty = 0.0f;	// �e�N�X�`���̍���Y���W

			pw = pw * volume / volumeMax;
			tw = tw * volume / volumeMax;



			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

		//�J�[�\����`��
		{
			if (optionCursorUse == TRUE)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[optionCursorTexNo]);

				//�X�^�[�g�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = optionCursorPos.x;	// �X�^�[�g�̕\���ʒuX
				float py = optionCursorPos.y;	// �X�^�[�g�̕\���ʒuY
				float pw = optionCursorW;		// �X�^�[�g�̕\����
				float ph = optionCursorH;		// �X�^�[�g�̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
				float ty = 0.0f;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
					0.0f);

				// �|���S���`��
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
