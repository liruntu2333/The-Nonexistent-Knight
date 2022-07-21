#include "file.h"
#include "score.h"

SAVEDATA	save;		 

void SaveData(void)
{
	{	 
		PLAYER* player = GetPlayer();

		for (int i = 0; i < PLAYER_MAX; i++)
		{
			save.player[i] = player[i];
		}
	}

	{	 
		ENEMY* enemy = GetEnemy();

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			save.enemy[i] = enemy[i];
		}
	}

	save.score = GetScore();

	char* adr = (char*)&save;
	int sum = 0;
	save.sum = 0;
	for (int i = 0; i < sizeof(SAVEDATA); i++)
	{
		sum += *(adr + i);
	}

	save.sum = sum;

	FILE* fp;

	printf("\nセーブ開始・・・");
	fp = fopen("savedata.bin", "wb");			 

	if (fp != nullptr)								 
	{
		fwrite(&save, sizeof BYTE, sizeof(SAVEDATA), fp);	 
		fclose(fp);								 
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}
}

void LoadData(void)
{
	PLAYER* player = GetPlayer();	 
	ENEMY* enemy = GetEnemy();	 

	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen("savedata.bin", "rb");	 

	if (fp != nullptr)						 
	{
		fread(&save, 1, sizeof(SAVEDATA), fp);	 
		fclose(fp);								 
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
	}

	char* adr = (char*)&save;
	int sum = 0;
	int org = save.sum;
	save.sum = 0;
	for (int i = 0; i < sizeof(SAVEDATA); i++)
	{
		sum += *(adr + i);
	}

	if (sum != org)
	{
		return;	   
	}

	{	 
		PLAYER* player = GetPlayer();

		for (int i = 0; i < PLAYER_MAX; i++)
		{
			player[i] = save.player[i];
		}
	}

	{	 
		ENEMY* enemy = GetEnemy();

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			enemy[i] = save.enemy[i];
		}
	}

	SetScore(save.score);
}