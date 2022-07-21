#pragma once

#include "main.h"
#include "player.h"
#include "enemy.h"
#include "score.h"

struct	SAVEDATA				 
{
	PLAYER	player[PLAYER_MAX];	 

	ENEMY	enemy[ENEMY_MAX];	 

	int		score;				 

	int sum;
};

void SaveData(void);
void LoadData(void);
