//=============================================================================
//
// �t�@�C������ [file.h]
// Author : 
//
//=============================================================================

/***************************************************************************
* �C���N���[�h�t�@�C��****
*******************************************************************************/
#include "file.h"
#include "score.h"


/*******************************************************************************
* �}�N����`
*******************************************************************************/



/*******************************************************************************
* �\���̒�`
*******************************************************************************/



/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/



/*******************************************************************************
�}�N����`
*******************************************************************************/



/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
SAVEDATA	save;		// �Z�[�u�f�[�^�쐬�ꏊ


/*******************************************************************************
�֐���:	void SaveData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���쐬���A�t�@�C���֏o�͂���
*******************************************************************************/
void SaveData(void)
{
	{	// �v���C���[�f�[�^���Z�[�u����
		PLAYER* player = GetPlayer();

		// �v���C���[�̐l�����Z�[�u����
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			save.player[i] = player[i];
		}
	}

	{	// �G�l�~�[�f�[�^���Z�[�u����
		ENEMY* enemy = GetEnemy();

		// �G�l�~�[�̐l�����Z�[�u����
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			save.enemy[i] = enemy[i];
		}
	}


	// �X�R�A�f�[�^���Z�[�u����
	save.score = GetScore();

	// add every byte of data as int
	char* adr = (char*)&save;
	int sum = 0;
	save.sum = 0;
	for (int i = 0; i < sizeof(SAVEDATA); i++)
	{
		sum += *(adr + i);
	}

	save.sum = sum;

	// SAVEDATA�\���̂��ƑS�����t�@�C���ɏo�͂���
	FILE* fp;

	printf("\n�Z�[�u�J�n�E�E�E");
	fp = fopen("savedata.bin", "wb");			// �t�@�C�����o�C�i���������݃��[�h��Open����

	if (fp != NULL)								// �t�@�C��������Ώ������݁A������Ζ���
	{
		fwrite(&save, sizeof BYTE, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}

}


/*******************************************************************************
�֐���:	void LoadData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���t�@�C������ǂݍ���
*******************************************************************************/
void LoadData(void)
{
	PLAYER* player = GetPlayer();	// �v���C���[�̃A�h���X���擾����
	ENEMY* enemy = GetEnemy();	// �G�l�~�[�̃A�h���X���擾����

	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE* fp;

	printf("\n���[�h�J�n�E�E�E");
	fp = fopen("savedata.bin", "rb");	// �t�@�C�����o�C�i���ǂݍ��݃��[�h��Open����

	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
	{
		fread(&save, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X�֎w�肵���o�C�g�����t�@�C������ǂݍ���
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
	}

	// add every byte of data as int
	char* adr = (char*)&save;
	int sum = 0;
	int org = save.sum;
	save.sum = 0;
	for (int i = 0; i < sizeof(SAVEDATA); i++)
	{
		sum += *(adr + i);
	}

	// calculated sum is differnt from saved data, implies been modified
	if (sum != org)
	{
		return;	// return without load
	}

	// �v���C���[�̐l�����Aplayer���[�N�֖߂�
	{	// �v���C���[�f�[�^�����[�h����
		PLAYER* player = GetPlayer();

		// �v���C���[�̐l�������[�h����
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			player[i] = save.player[i];
		}
	}

	{	// �G�l�~�[�f�[�^�����[�h����
		ENEMY* enemy = GetEnemy();

		// �G�l�~�[�̐l�������[�h����
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			enemy[i] = save.enemy[i];
		}
	}

	// �X�R�A�f�[�^�����[�h����
	SetScore(save.score);


}


