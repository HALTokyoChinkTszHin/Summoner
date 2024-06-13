//=============================================================================
//
// BG���� [bg.cpp]
// Author : �K�q���i�`�F���Y�[�V�F���j
//
//=============================================================================
#include "bg.h"




//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH * 4)			// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)				// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)							// �e�N�X�`���̐�

#define ANIM_WAIT					(5)							// �A�j���[�V�����̐؂�ւ��Wait�l

#define TEXTURE_GROUND_WIDTH		(35)						// (SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_GROUND_HEIGHT		(35)						// (SCREEN_HEIGHT)	// 

#define TEXTURE_WIDTH_LOGO			(480)						// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)						// 



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/sky000.png",
	"data/TEXTURE/sky001.png",
	"data/TEXTURE/fargrounds.png",
	"data/TEXTURE/turfground.png",
	"data/TEXTURE/rockground.png",



};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_BG;

static GROUND	g_Ground[TEXTURE_GROUND_MAX];	// �n�ʍ\����


static GROUND	g_Step[STEP_MAX];	// ���ݏ�\����




//=============================================================================
// ����������
//=============================================================================
HRESULT InitBG(void)
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
	g_BG.w     = TEXTURE_WIDTH;
	g_BG.h     = TEXTURE_HEIGHT;
	g_BG.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.texNo = 0;

	g_BG.scrl  = 0.0f;		// TEX�X�N���[��
	g_BG.scrl2 = 0.0f;		// TEX�X�N���[��

		// �n�ʍ\���̂̏�����
	for (int i = 0; i < TEXTURE_GROUND_MAX; i++)
	{
		g_Ground[i].use = TRUE;
		g_Ground[i].pos = XMFLOAT3(35.0f * i , SCREEN_HEIGHT - TEXTURE_GROUND_HEIGHT, 0.0f);	// ���S�_����\��
		g_Ground[i].w = TEXTURE_GROUND_WIDTH;
		g_Ground[i].h = TEXTURE_GROUND_HEIGHT;
		g_Ground[i].texNo = 3;



	}


	// ���ݏ�\���̂̏�����
	for (int i = 0; i < STEP_MAX; i++)
	{
		g_Step[i].use = FALSE;
		g_Step[i].pos = XMFLOAT3(35.0f * i, 480.0f, 0.0f);	// ���S�_����\��
		g_Step[i].w = TEXTURE_GROUND_WIDTH;
		g_Step[i].h = TEXTURE_GROUND_HEIGHT;
		g_Step[i].texNo = 4;



	}








	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateBG(void)
{
	g_BG.old_pos = g_BG.pos;	// �P�t���O�̏���ۑ�





#ifdef _DEBUG	// �f�o�b�O����\������


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

	// �w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_BG.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		g_BG.scrl += 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, SKY_H,
			g_BG.scrl, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		g_BG.scrl2 -= 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, SKY_H, SCREEN_WIDTH, SKY_H,
			g_BG.scrl2, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �n�ʂ�`��
	{

		for (int i = 0; i < TEXTURE_GROUND_MAX; i++)
		{
			if (g_Ground[i].use == TRUE)		// ���̒n�ʂ��g���Ă���H
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Ground[i].texNo]);

				float px = g_Ground[i].pos.x;	// �n�ʂ̕\���ʒuX
				float py = g_Ground[i].pos.y;	// �n�ʂ̕\���ʒuY
				float pw = g_Ground[i].w;		// �n�ʂ̕\����
				float ph = g_Ground[i].h;		// �n�ʂ̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
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


	// ���ݏ��`��
	{

		for (int i = 0; i < STEP_MAX; i++)
		{
			if (g_Step[i].use == TRUE)		// ���̒n�ʂ��g���Ă���H
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Step[i].texNo]);

				float px = g_Step[i].pos.x;	// �n�ʂ̕\���ʒuX
				float py = g_Step[i].pos.y;	// �n�ʂ̕\���ʒuY
				float pw = g_Step[i].w;		// �n�ʂ̕\����
				float ph = g_Step[i].h;		// �n�ʂ̕\������

				float tw = 1.0f;	// �e�N�X�`���̕�
				float th = 1.0f;	// �e�N�X�`���̍���
				float tx = 0.0f;	// �e�N�X�`���̍���X���W
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


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG* GetBG(void)
{
	return &g_BG;
}

//=============================================================================
// �n�ʍ\���̂̐擪�A�h���X���擾
//=============================================================================
GROUND* GetGround(void)
{
	return &g_Ground[0];
}

//=============================================================================
// �n�ʍ\���̂̐擪�A�h���X���擾
//=============================================================================
GROUND* GetStep(void)
{
	return &g_Step[0];
}






