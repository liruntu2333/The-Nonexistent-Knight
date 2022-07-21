#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "terrain.h"
#include <math.h>
#include "player.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "effect.h"

#define TEXTURE_WIDTH				(800/8)	 
#define TEXTURE_HEIGHT				(800/8)	
#define TEXTURE_MAX					(2)		 

#define TEXTURE_PATTERN_DIVIDE_X	(2)		 
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		 
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	 
#define ANIM_WAIT					(8)		 
#define PI							(acos(-1))

#define	RUN_SPEED					(4.0f)
#define BIG_STUN_FRAME				(30)
#define BIG_STUN_HSPD				(20)
#define PARRY_STUN_FRAME			(60)

#define PARRY_SM_FRAME				(60)	       
#define DEAD_FRAME					60

#define ENEMYRHT_TEX_NO				1
#define ENEMYLFT_TEX_NO				0

#define GET_HIT_PARTICLE			3

void DestructEnemy(ENEMY* ep);
void ChekHitPlayer(ENEMY* enemy);

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy02.png",
	"data/TEXTURE/enemy_reverse.png",
};

static BOOL		g_Load = FALSE;			 
static ENEMY	g_Enemy[ENEMY_MAX];	 

HRESULT InitEnemy(void)
{
	ID3D11Device* pDevice = GetDevice();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			nullptr,
			nullptr,
			&g_Texture[i],
		nullptr);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		ENEMY* s_Enemy = g_Enemy + i;

		s_Enemy->use = TRUE;
		s_Enemy->pos = D3DXVECTOR3(TEXTURE_WIDTH / 2 + 500.0f + 100.0f * i, SCREEN_HEIGHT / 2, 0.0f);	 
		s_Enemy->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		s_Enemy->w = TEXTURE_WIDTH;
		s_Enemy->h = TEXTURE_HEIGHT;
		s_Enemy->texNo = ENEMYRHT_TEX_NO;

		s_Enemy->countAnim = 0;
		s_Enemy->patternAnim = 0;

		s_Enemy->state = STAND;
		s_Enemy->orient = RIGHT;
		s_Enemy->atkOrient = RIGHT;
		s_Enemy->atkDetect = TRUE;
		s_Enemy->vertSpd = 0;
		s_Enemy->horzSpd = 0;
		s_Enemy->actCount = 0;
		s_Enemy->effect = nullptr;
		s_Enemy->rddot = nullptr;
		s_Enemy->slashed = FALSE;

		s_Enemy->stamina = 0;
		s_Enemy->health = ENEMY_HEALTH_MAX;

	}

	g_Load = TRUE;	 
	return S_OK;
}

void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = nullptr;
		}
	}

	g_Load = FALSE;
}

void UpdateEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		ENEMY* s_Enemy = g_Enemy + i;

		if (s_Enemy->use == TRUE)
		{
			if (s_Enemy->state < DASH || s_Enemy->state > ATTACK)
			{
				if (s_Enemy->state > ATTACK)
				{
					s_Enemy->pos.y += s_Enemy->vertSpd;
					s_Enemy->vertSpd += GRAVITATIONAL_CONST;
					if (s_Enemy->vertSpd > FALL_LIMIT)
					{
						s_Enemy->vertSpd = FALL_LIMIT;
					}
					if (s_Enemy->vertSpd >= 0)
					{
						s_Enemy->state = FALL;
					}
					if (GetTerrain(s_Enemy->pos.x, s_Enemy->pos.y - s_Enemy->h / 2))
					{
						s_Enemy->pos.y += 5.0f;
						s_Enemy->vertSpd = 1;
					}
				}

				if (s_Enemy->orient == RIGHT)
				{
					s_Enemy->pos.x += RUN_SPEED;
					if (GetTerrain(s_Enemy->pos.x + s_Enemy->w / 2, s_Enemy->pos.y))
					{
						s_Enemy->orient = LEFT;
						s_Enemy->texNo = ENEMYLFT_TEX_NO;
					}
				}
				else
				{
					s_Enemy->pos.x -= RUN_SPEED;
					if (GetTerrain(s_Enemy->pos.x - s_Enemy->w / 2, s_Enemy->pos.y))
					{
						s_Enemy->orient = RIGHT;
						s_Enemy->texNo = ENEMYRHT_TEX_NO;
					}
				}

				if (GetTerrain(s_Enemy->pos.x, s_Enemy->pos.y + s_Enemy->h / 2))
				{
					if (s_Enemy->state == FALL)
					{
						s_Enemy->vertSpd = 0;
						s_Enemy->state = STAND;
						s_Enemy->pos = ReloacteObj(s_Enemy->pos.x, s_Enemy->pos.y, s_Enemy->w, s_Enemy->h);
					}
				}
				else if (s_Enemy->state <= RUN)
				{
					s_Enemy->state = FALL;
				}
			}

			if (s_Enemy->state == STUN)
			{
				if (s_Enemy->actCount--)
				{
					if (s_Enemy->rddot)
					{
						s_Enemy->rddot->pos = s_Enemy->pos;
					}
					if (s_Enemy->horzSpd > 0)
					{
						if (!GetTerrain(s_Enemy->pos.x + s_Enemy->w / 2, s_Enemy->pos.y))
						{
							s_Enemy->pos.x += s_Enemy->horzSpd;
						}
					}
					else if (s_Enemy->horzSpd < 0)
					{
						if (!GetTerrain(s_Enemy->pos.x - s_Enemy->w / 2, s_Enemy->pos.y))
						{
							s_Enemy->pos.x += s_Enemy->horzSpd;
						}
					}

					s_Enemy->horzSpd = (int)(0.9f * s_Enemy->horzSpd);

					if (!GetTerrain(s_Enemy->pos.x, s_Enemy->pos.y + s_Enemy->h / 2))
					{
						s_Enemy->pos.y += s_Enemy->vertSpd;
						s_Enemy->vertSpd += GRAVITATIONAL_CONST;
						if (s_Enemy->vertSpd > FALL_LIMIT)
						{
							s_Enemy->vertSpd = FALL_LIMIT;
						}
					}
				}
				else
				{
					s_Enemy->state = FALL;
					s_Enemy->slashed = FALSE;
					if (s_Enemy->rddot)
					{
						s_Enemy->rddot->use = FALSE;
						s_Enemy->rddot = nullptr;
						GetPlayer()->pryDetect = FALSE;
					}
				}
			}
			if (s_Enemy->state == DEAD)
			{
				if (s_Enemy->actCount == DEAD_FRAME)
				{
					SetStunFrame();
				}
				if (!s_Enemy->actCount--)
				{
					s_Enemy->use = FALSE;
				}
				else
				{
					SetShake(10);
					SetEffect(s_Enemy->pos.x, s_Enemy->pos.y, MAGIC_CIRCLE, rand() % 4);
				}
			}

			if (s_Enemy->state != STUN && s_Enemy->state != DEAD)
			{
				ChekHitPlayer(s_Enemy);
			}

			g_Enemy[i].countAnim += 1.0f;
			if (g_Enemy[i].countAnim > ANIM_WAIT)
			{
				g_Enemy[i].countAnim = 0.0f;
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

#ifdef _DEBUG
#endif
		}
	}
}

void DrawEnemy(void)
{
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	SetWorldViewProjection2D();

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	Terrain* bg = GetTerrain();

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		 
		{								 
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			float px = g_Enemy[i].pos.x - bg->pos.x;	 
			float py = g_Enemy[i].pos.y - bg->pos.y;	 
			float pw = g_Enemy[i].w;		 
			float ph = g_Enemy[i].h;		 

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	 
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	 
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	 
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	 

			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			GetDeviceContext()->Draw(4, 0);
		}
	}
}

ENEMY* GetEnemy(void)
{
	return &g_Enemy[0];
}

void DestructEnemy(ENEMY* ep)
{
	AddScore(100);
	if (ep->use) ep->use = FALSE;
	return;
}

void ChekHitPlayer(ENEMY* enemy)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = GetPlayer() + i;
		if (s_Player->use)
		{
			if (enemy->effect)
			{
				if (BBCollision(&enemy->effect->pos, &s_Player->pos,
					enemy->effect->w, s_Player->w,
					enemy->effect->h, s_Player->h))
				{
					if (s_Player->pos.y + s_Player->h / 2 < enemy->effect->pos.y)
					{
						enemy->atkOrient = UP;
					}
					else if (s_Player->pos.y - s_Player->h / 2 > enemy->effect->pos.y)
					{
						enemy->atkOrient = DOWN;
					}
					if (s_Player->pos.x + s_Player->w / 2 < enemy->effect->pos.x)
					{
						enemy->atkOrient = LEFT;
					}
					else if (s_Player->pos.x - s_Player->w / 2 > enemy->effect->pos.x)
					{
						enemy->atkOrient = RIGHT;
					}
					HitPlayer(enemy, s_Player, 1, enemy->atkOrient);
				}
			}
			else
			{
				if (BBCollision(&enemy->pos, &s_Player->pos,
					enemy->w, s_Player->w,
					enemy->h, s_Player->h))
				{
					if (s_Player->pos.y + s_Player->h / 2 < enemy->pos.y)
					{
						enemy->atkOrient = UP;
					}
					else if (s_Player->pos.y - s_Player->h / 2 > enemy->pos.y)
					{
						enemy->atkOrient = DOWN;
					}
					else if (s_Player->pos.x + s_Player->w / 2 < enemy->pos.x)
					{
						enemy->atkOrient = LEFT;
					}
					else if (s_Player->pos.x - s_Player->w / 2 > enemy->pos.x)
					{
						enemy->atkOrient = RIGHT;
					}
					HitPlayer(enemy, s_Player, 1, enemy->atkOrient);
				}
			}
		}
	}
}

void HitEnemy(ENEMY* enemy, int damage, int orient)
{
	enemy->state = STUN;

	if (!damage)
	{
		enemy->actCount = PARRY_STUN_FRAME;
		SetSlowMotion(PARRY_SM_FRAME);
		enemy->rddot = SetEffect(enemy->pos.x, enemy->pos.y, RED_DOT, RIGHT);
		return;
	}
	else
	{
		if (enemy->rddot)
		{
			enemy->rddot->use = FALSE;
			enemy->rddot = nullptr;
		}
	}
	enemy->actCount = BIG_STUN_FRAME;
	SetEffect(enemy->pos.x, enemy->pos.y, BLOOD_SPLASH, orient);
	switch (orient)
	{
	case RIGHT:
	case LEFT:

		enemy->state = STUN;
		if (orient % 4)
			enemy->horzSpd = -BIG_STUN_HSPD * damage;
		else
			enemy->horzSpd = BIG_STUN_HSPD * damage;
		break;

	case DOWN:
	case UP:
		enemy->horzSpd = 0;
		break;

	default:
		break;
	}
	if (damage == 2)
	{
		enemy->slashed = TRUE;
	}
	enemy->health -= damage;
	if (enemy->health <= 0)
	{
		enemy->health = 0;
		enemy->state = DEAD;
		enemy->actCount = DEAD_FRAME;
	}

	for (int i = 0; i < damage; i++)
	{
		SetEffect(enemy->pos.x, enemy->pos.y, COIN, RIGHT);
	}
	for (int i = 0; i < GET_HIT_PARTICLE; i++)
	{
		SetEffect(enemy->pos.x, enemy->pos.y, MAGIC_CIRCLE, orient);
	}
	PlaySound(SOUND_LABEL_SE_hit);
}