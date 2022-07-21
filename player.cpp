#include "player.h"
#include "sprite.h"
#include "input.h"
#include "terrain.h"
#include "bullet.h"
#include "file.h"
#include "effect.h"
#include "collision.h"
#include "elevator.h"
#include "fade.h"
#include "sound.h"

#define TEXTURE_WIDTH				(75.0f)	 
#define TEXTURE_HEIGHT				(75.0f)	
#define TEXTURE_MAX					(1)		 
#define	PLAYER_TEX_NO				(0)

#define PLAYER_PNG_W				900
#define PLAYER_PNG_H				1050

#define UP_ATTACK_Y					13
#define DOWN_ATTACK_Y				12

#define ANIM_WAIT					(3)		 

#define ILLUSION_MAX				(20)

#define RUN_SPEED					(8.0f)

#define JUMP_SPEED					(-20)
#define HITJUMP_SPD					(-15)

#define DASH_SPEED					(15)
#define DASH_FRAME					(20)
#define DASH_GOD_ST					(0)
#define DASH_GOD_END				(15)

#define ATK_FRAME					(12)
#define ATK_DETECTION				(2)
#define ATK_END						(8)

#define SLASH_FRAME					(8)
#define SLASH_DETECTION				(0)
#define SLASH_END					(8)
#define SLASH_SPD					(15)

#define PARRY_FRAME					(15)
#define	PARRY_DETECTION				(2)
#define PARRY_END					(7)

#define HEAL_FRAME					(120)
#define HEAL_START					(60)
#define HEAL_TAKE_EFFECT			(80)
#define HEAL_COST					(10)

#define ATK_COST					(30)
#define DASH_COST					(20)
#define JUMP_COST					(10)
#define PARRY_COST					(15)

#define MINI_STUN_FRAME				(3)
#define MINI_STUN_HSPD				(10)
#define BIG_STUN_FRAME				(20)
#define BIG_STUN_HSPD				(20)
#define BIG_STUN_VSPD				(10)

#define DEAD_FRAME					(60)

#define GOD_FRAME					(120)
#define GOD_FLASH					(5)

#define	ILLUSION_LIFE_SPAN			(30)
#define ILLUSION_GEN_LAG			(4)

#define HIT_SHAKE					(10)
#define HITBYENEMY_SHAKE			(20)
#define SLASH_SHAKE					(15)

#define DUST_GEN_LAG				(30)

#define GET_HIT_PARTICLE				(10)

struct TEXTURE_INFO
{
	float startY;
	int frame;
};

struct ILLUSION
{
	PLAYER illusion;
	int life;
};

void CheckHitTerra(PLAYER* player, EFFECT* effect);
void CheckHitEnemy(PLAYER* player, EFFECT* effect);
void GetTrigger(PLAYER* player);

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/player.png",
};

static BOOL		g_Load = FALSE;			 
static PLAYER	g_Player[PLAYER_MAX];	 

static ILLUSION	g_Illusion[ILLUSION_MAX];

static const TEXTURE_INFO c_TexInfo[STATE_MAX] =
{
	{0.0f,					12},
	{TEXTURE_HEIGHT * 2,	8},
	{TEXTURE_HEIGHT * 8,	4},
	{TEXTURE_HEIGHT * 6,	2},
	{TEXTURE_HEIGHT * 2,	8},
	{TEXTURE_HEIGHT * 2,	8},
	{TEXTURE_HEIGHT * 4,	4},
	{TEXTURE_HEIGHT * 8,	4},
	{TEXTURE_HEIGHT * 4,	4},
	{TEXTURE_HEIGHT * 6,	2},
	{TEXTURE_HEIGHT * 6,	2},
	{TEXTURE_HEIGHT * 6,	2},
};

HRESULT InitPlayer(void)
{
	ID3D11Device* pDevice = GetDevice();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;

		s_Player->use = TRUE;
		s_Player->pos = D3DXVECTOR3(TEXTURE_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 0.0f);	 
		s_Player->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		s_Player->w = TEXTURE_WIDTH;
		s_Player->h = TEXTURE_HEIGHT;
		s_Player->texNo = PLAYER_TEX_NO;

		s_Player->countAnim = 0;
		s_Player->patternAnim = 0;

		s_Player->state = FALL;
		s_Player->orient = RIGHT;
		s_Player->atkOrient = RIGHT;
		s_Player->atkDetect = TRUE;
		s_Player->vertSpd = 0;
		s_Player->horzSpd = 0;
		s_Player->actCount = 0;
		s_Player->effect = nullptr;
		s_Player->elev = nullptr;

		s_Player->stamina = PLAYER_STAMINA_MAX;
		s_Player->health = PLAYER_HEALTH_MAX;
		s_Player->money = 0;
	}

	for (int i = 0; i < ILLUSION_MAX; i++)
	{
		ILLUSION* s_Illusion = g_Illusion + i;

		s_Illusion->illusion.use = FALSE;
	}

	g_Load = TRUE;	 
	return S_OK;
}

void UninitPlayer(void)
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

void UpdatePlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* player = g_Player + i;
		if (player->use == TRUE)
		{
			if (player->godCount) player->godCount--;

			if (player->elev)
			{
				player->pos.y += player->elev->vertSpd;
			}

			GetTrigger(player);

			switch (player->state)
			{
			case RUN:
			{
				if (!(GetFrameCount() % DUST_GEN_LAG))
				{
					PlaySound(SOUND_LABEL_SE_walk);
					SetEffect(player->pos.x, player->pos.y + player->h / 2, DUST, 0);
				}
				if (player->elev)
				{
					if (player->pos.x < player->elev->pos.x ||
						player->pos.x > player->elev->pos.x + player->elev->w)
					{
						player->state = FALL;
						player->countAnim = 0.0f; player->patternAnim = 0;
						player->elev = nullptr;
					}
				}
				else if (!GetTerrain(player->pos.x, player->pos.y + player->h / 2))
				{
					player->state = FALL;
					player->countAnim = 0.0f; player->patternAnim = 0;
				}
				break;
			}
			case STAND:
			{
				break;
			}
			case FALL:
			{
				player->pos.y += player->vertSpd;
				player->vertSpd += GRAVITATIONAL_CONST;
				if (player->vertSpd > FALL_LIMIT)
				{
					player->vertSpd = FALL_LIMIT;
				}

				if (GetTerrain(player->pos.x, player->pos.y + player->h / 2))
				{
					player->vertSpd = 0;
					player->state = STAND;
					player->countAnim = 0.0f; player->patternAnim = 0;

					player->pos = ReloacteObj(player->pos.x, player->pos.y, player->w, player->h);
					for (int i = 0; i < ELEV_MAX; i++)
					{
						ELEVATOR* s_Elevator = GetElev() + i;
						if (PECollision(&player->pos, &s_Elevator->pos,
							player->w, s_Elevator->w, player->h, s_Elevator->h))
						{
							player->state = STAND;
							player->countAnim = 0.0f; player->patternAnim = 0;
							player->elev = s_Elevator;
						}
					}

					PlaySound(SOUND_LABEL_SE_bounce);
				}
				break;
			}
			case JUMP:
			case BIG_JUMP:
			{
				player->pos.y += player->vertSpd;
				player->vertSpd += GRAVITATIONAL_CONST;
				if (player->vertSpd > FALL_LIMIT)
				{
					player->vertSpd = FALL_LIMIT;
				}
				if (player->vertSpd >= 0)
				{
					player->state = FALL;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->elev = nullptr;
				}
				if (GetTerrain(player->pos.x, player->pos.y - player->h / 2))
				{
					player->pos.y += 5.0f;
					player->vertSpd = 1;
					player->state = FALL;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->elev = nullptr;
				}
				break;
			}
			case DASH:
			{
				if ((player->actCount % ILLUSION_GEN_LAG) == 0)        
				{
					for (int i = 0; i < ILLUSION_MAX; i++)
					{
						if (!(g_Illusion + i)->illusion.use)
						{
							(g_Illusion + i)->illusion = *player;
							(g_Illusion + i)->life = ILLUSION_LIFE_SPAN;
							g_Illusion->illusion.use = TRUE;
							break;
						}
					}
				}

				if (player->actCount == DASH_FRAME - DASH_GOD_ST)
				{
					player->godCount = DASH_GOD_END - DASH_GOD_ST;
				}

				if (GetFrameCount() % 2)
				{
					SetEffect(player->pos.x, player->pos.y, DUST_CIRCLE, 0);
				}

				switch (player->orient)
				{
				case LEFT:								   
					if (!GetTerrain(player->pos.x - player->w / 2, player->pos.y))
					{
						player->pos.x -= DASH_SPEED;
					}
					break;
				case RIGHT:								   
					if (!GetTerrain(player->pos.x + player->w / 2, player->pos.y))
					{
						player->pos.x += DASH_SPEED;
					}
					break;
				default:
					break;
				}

				if (!player->actCount--)
				{
					player->state = FALL;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->elev = nullptr;
					player->vertSpd = 0;
				}
				break;
			}
			case ATTACK:
			{
				if (player->actCount == ATK_FRAME)
				{
					if (GetKeyboardPress(DIK_W))
					{
						player->atkOrient = UP;
					}
					else if (GetKeyboardPress(DIK_S))
					{
						player->atkOrient = DOWN;
						if (player->elev || GetTerrain(player->pos.x, player->pos.y + player->h / 2))
						{
							player->stamina += ATK_COST;
							player->actCount = 0;
							player->state = FALL;
							player->countAnim = 0.0f;
							player->patternAnim = 0;
							break;
						}
					}
					else if (GetKeyboardPress(DIK_D))
					{
						player->atkOrient = RIGHT;
					}
					else if (GetKeyboardPress(DIK_A))
					{
						player->atkOrient = LEFT;
					}
					else
					{
						player->atkOrient = player->orient;
					}
					player->vertSpd = 0;
					player->atkDetect = FALSE;

					PlaySound(SOUND_LABEL_SE_attck);
				}

				if (player->actCount == ATK_FRAME - ATK_DETECTION)
				{
					int rand_effect = rand() % 2;
					player->effect = SetEffect(player->pos.x, player->pos.y,
						PLAYER_BLADE + rand_effect, player->atkOrient);
				}

				if (!player->atkDetect &&
					player->actCount < ATK_FRAME - ATK_DETECTION && player->actCount > ATK_FRAME - ATK_END)
				{
					EFFECT* s_Effect = player->effect;

					CheckHitEnemy(player, s_Effect);

					CheckHitTerra(player, s_Effect);
				}

				if (!player->actCount--)
				{
					player->atkOrient = player->orient;
					player->effect = nullptr;
					player->state = FALL;
					player->countAnim = 0.0f;
					player->patternAnim = 0;
					player->elev = nullptr;
				}
				break;
			}
			case SLASH:
			{
				if (player->actCount == SLASH_FRAME)
				{
					SetSlowMotion(1);
					PlaySound(SOUND_LABEL_SE_pasueReturn);
					player->pryDetect = FALSE;
					player->atkOrient = player->orient;
					player->effect = SetEffect(player->pos.x, player->pos.y,
						PLAYER_SLASH, player->atkOrient);

					player->vertSpd = 0;
					player->atkDetect = FALSE;
				}

				if (player->actCount > SLASH_DETECTION && player->actCount <= SLASH_END)
				{
					EFFECT* s_Effect = player->effect;

					for (int i = 0; i < ENEMY_MAX; i++)
					{
						ENEMY* s_Enemy = GetEnemy() + i;
						if (BBCollision(&s_Effect->pos, &s_Enemy->pos,
							s_Effect->w, s_Enemy->w, s_Effect->h, s_Enemy->h) &&
							s_Enemy->slashed == FALSE && s_Enemy->state != DEAD)  
						{
							SetEffect(s_Enemy->pos.x, s_Enemy->pos.y,
								PLAYER_HIT, player->atkOrient);
							HitEnemy(s_Enemy, 2, player->atkOrient);

							SetShake(SLASH_SHAKE);
						}
					}
				}

				switch (player->orient)
				{
				case LEFT:								   
					if (!GetTerrain(player->pos.x - player->w / 2, player->pos.y))
					{
						player->pos.x -= SLASH_SPD;
					}
					break;
				case RIGHT:								   
					if (!GetTerrain(player->pos.x + player->w / 2, player->pos.y))
					{
						player->pos.x += SLASH_SPD;
					}
					break;
				default:
					break;
				}

				if (!player->actCount--)
				{
					player->effect = nullptr;
					player->state = FALL;
					player->countAnim = 0.0f;
					player->patternAnim = 0;
					player->elev = nullptr;
				}
				break;
			}
			case PARRY:
			{
				if (!player->actCount--)
				{
					player->vertSpd = 0;
					player->state = FALL;
					player->countAnim = 0.0f;
					player->patternAnim = 0;
					player->elev = nullptr;
				}
				break;
			}
			case STUN:
			{
				if (player->actCount--)
				{
					if (player->horzSpd > 0)
					{
						if (!GetTerrain(player->pos.x + player->w / 2, player->pos.y))
						{
							player->pos.x += player->horzSpd;
						}
					}
					else if (player->horzSpd < 0)
					{
						if (!GetTerrain(player->pos.x - player->w / 2, player->pos.y))
						{
							player->pos.x += player->horzSpd;
						}
					}

					player->horzSpd = (int)(0.9f * player->horzSpd);

					if (!GetTerrain(player->pos.x, player->pos.y + player->h / 2))
					{
						player->pos.y += player->vertSpd;
						player->vertSpd += GRAVITATIONAL_CONST;
						if (player->vertSpd > FALL_LIMIT)
						{
							player->vertSpd = FALL_LIMIT;
						}
					}
				}
				else
				{
					player->state = FALL;
					player->countAnim = 0.0f;
					player->patternAnim = 0;
					player->elev = nullptr;
				}
				break;
			}
			case DEAD:
			{
				if (!player->actCount--)
				{
					SetFade(FADE_OUT, MODE_GAME);
				}
				else
				{
					SetEffect(player->pos.x, player->pos.y, DUST_CIRCLE, rand() % 4);
				}
				break;
			}
			case HEAL:
			{
				if (player->actCount--)
				{
					if (player->actCount == HEAL_FRAME - HEAL_START)
					{
						if (player->money >= HEAL_COST)
						{
							player->effect = SetEffect(player->pos.x, player->pos.y, PLAYER_HEAL, 0);
							player->money -= HEAL_COST;
						}
						else
						{
							player->state = FALL;
							player->vertSpd = 0;
							player->countAnim = 0.0f;
							player->patternAnim = 0;
						}
					}
					if (player->actCount == HEAL_FRAME - HEAL_TAKE_EFFECT)
					{
						PlaySound(SOUND_LABEL_SE_heal);
						player->health += (player->health < PLAYER_HEALTH_MAX) ? 1 : 0;
					}
				}
				else
				{
					player->effect = nullptr;
					player->vertSpd = 0;
					player->state = FALL;
					player->countAnim = 0.0f;
					player->patternAnim = 0;
					player->elev = nullptr;
				}
				break;
			}
			default:
				break;
			}

			for (int i = 0; i < ILLUSION_MAX; i++)
			{
				if ((g_Illusion + i)->illusion.use)
				{
					if (!--(g_Illusion + i)->life)
						(g_Illusion + i)->illusion.use = FALSE;
				}
			}

			if (player->countAnim++ > ANIM_WAIT)
			{
				player->countAnim = 0.0f;
				if (++player->patternAnim >= c_TexInfo[player->state].frame)
				{
					player->patternAnim = 0;
				}
			}

#ifdef _DEBUG
#endif
		}
	}
}

void DrawPlayer(void)
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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;
		if (s_Player->use == TRUE && s_Player->state != DASH)		 
		{									 
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Player->texNo]);

			float px = s_Player->pos.x - bg->pos.x;	 
			float py = s_Player->pos.y - bg->pos.y;	 
			float pw = s_Player->w;		 
			float ph = s_Player->h;		 

			float tw = 1.0f / PLAYER_PNG_W * TEXTURE_WIDTH;
			float th = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
			float tx = tw * s_Player->patternAnim;
			float sumH = 0.0f;
			float ty = 1.0f / PLAYER_PNG_H * c_TexInfo[s_Player->state].startY;

			switch (s_Player->state)
			{
			case HEAL:
				break;

			case ATTACK:
				if (s_Player->atkOrient == UP)
				{
					ty = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT * UP_ATTACK_Y;
				}
				else if (s_Player->atkOrient == DOWN)
				{
					ty = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT * DOWN_ATTACK_Y;
				}
				else
				{
					ty += (!s_Player->orient) ? 0 : 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
				}
				break;

			default:
				ty += (!s_Player->orient) ? 0 : 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
				break;
			}

			SetSpriteColorRotation(g_VertexBuffer, px, py, s_Player->w, s_Player->h,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				s_Player->rot.z);

			GetDeviceContext()->Draw(4, 0);
		}
	}

	for (int i = 0; i < ILLUSION_MAX; i++)
	{
		ILLUSION* s_Illusion = g_Illusion + i;
		if (s_Illusion->illusion.use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Illusion->illusion.texNo]);

			float px = s_Illusion->illusion.pos.x - bg->pos.x;	 
			float py = s_Illusion->illusion.pos.y - bg->pos.y;	 
			float pw = s_Illusion->illusion.w;		 
			float ph = s_Illusion->illusion.h;		 

			float tw = 1.0f / PLAYER_PNG_W * TEXTURE_WIDTH;
			float th = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
			float tx = tw * s_Illusion->illusion.patternAnim;
			float sumH = 0.0f;
			float ty = 1.0f / PLAYER_PNG_H * c_TexInfo[s_Illusion->illusion.state].startY;

			switch (s_Illusion->illusion.state)
			{
			case HEAL:
				break;

			case ATTACK:
				if (s_Illusion->illusion.atkOrient == UP)
				{
					ty = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT * UP_ATTACK_Y;
				}
				else if (s_Illusion->illusion.atkOrient == DOWN)
				{
					ty = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT * DOWN_ATTACK_Y;
				}
				else
				{
					ty += (!s_Illusion->illusion.orient) ? 0 : 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
				}
				break;

			default:
				ty += (!s_Illusion->illusion.orient) ? 0 : 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
				break;
			}

			float color_fa = 1.0f - 0.3f * ILLUSION_LIFE_SPAN / s_Illusion->life;
			SetSpriteColorRotation(g_VertexBuffer, px, py, s_Illusion->illusion.w, s_Illusion->illusion.h,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, color_fa),
				s_Illusion->illusion.rot.z);

			GetDeviceContext()->Draw(4, 0);
		}
	}
}

PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

void HitPlayer(ENEMY* enemy, PLAYER* player, int damge, int orient)
{
	if (!player->godCount)
	{
		if (player->state == PARRY &&
			player->actCount <= PARRY_FRAME - PARRY_DETECTION &&
			player->actCount >= PARRY_FRAME - PARRY_END)
		{
			HitEnemy(enemy, 0, 0);
			player->pryDetect = TRUE;
			PlaySound(SOUND_LABEL_SE_timePause);
			if (player->pos.x > enemy->pos.x)
			{
				SetEffect(player->pos.x, player->pos.y, PLAYER_PARRY, LEFT);
			}
			else
			{
				SetEffect(player->pos.x, player->pos.y, PLAYER_PARRY, RIGHT);
			}
			return;
		}

		if (player->effect)
		{
			player->effect->use = FALSE;
			player->effect = nullptr;
		}
		SetEffect(player->pos.x, player->pos.y, BLOOD_SPLASH, orient);
		player->state = STUN;
		player->countAnim = 0.0f;
		player->patternAnim = 0;
		player->actCount = BIG_STUN_FRAME;
		player->health -= damge;
		PlaySound(SOUND_LABEL_SE_hit);
		SetStunFrame();

		if (player->health <= 0)
		{
			player->health = 0;
			player->state = DEAD;
			player->countAnim = 0.0f;
			player->patternAnim = 0;
			player->actCount = DEAD_FRAME;
		}
		switch (orient)
		{
		case RIGHT:
			player->horzSpd = BIG_STUN_HSPD;
			break;

		case LEFT:
			player->horzSpd = -BIG_STUN_HSPD;
			break;

		case DOWN:
			player->vertSpd = BIG_STUN_VSPD;
			player->horzSpd = 0;

			break;

		case UP:
			player->vertSpd = -BIG_STUN_VSPD;
			player->horzSpd = 0;

			break;

		default:
			break;
		}
		SetShake(HITBYENEMY_SHAKE);
		player->godCount = GOD_FRAME;
		for (int i = 0; i < GET_HIT_PARTICLE; i++)
		{
			SetEffect(player->pos.x, player->pos.y, DUST_CIRCLE, RIGHT);
		}
	}
	return;
}

void CheckHitTerra(PLAYER* player, EFFECT* effect)
{
	if (!player->atkDetect)
	{
		float cof = -1.0f;
		float radius = effect->w / 2;
		const float rcosrot = radius * (float)cos(effect->rot.z);
		const float rsinrot = radius * (float)sin(effect->rot.z);
		while (cof <= 1.0f)
		{
			if (GetTerrain(effect->pos.x + cof * rcosrot, effect->pos.y + cof * rsinrot))
			{
				PlaySound(SOUND_LABEL_SE_reflect);

				switch (player->atkOrient)
				{
				case RIGHT:
				case LEFT:
					SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
						PLAYER_REFLECT, player->atkOrient);
					player->effect->use = FALSE;
					player->effect = nullptr;

					player->state = STUN;
					player->countAnim = 0.0f; player->patternAnim = 0;
					if (player->atkOrient % 4)
						player->horzSpd = MINI_STUN_HSPD;
					else
						player->horzSpd = -MINI_STUN_HSPD;
					player->actCount = MINI_STUN_FRAME;
					SetShake(HIT_SHAKE);
					break;

				case DOWN:
					if (!GetTerrain(player->pos.x, player->pos.y + player->h / 2))
					{
						SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
							PLAYER_REFLECT, player->atkOrient);
						player->vertSpd = HITJUMP_SPD;
						player->state = BIG_JUMP;
						player->countAnim = 0.0f; player->patternAnim = 0;
						SetShake(HIT_SHAKE);
					}
					break;

				case UP:
					SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
						PLAYER_REFLECT, player->atkOrient);
					player->vertSpd = -HITJUMP_SPD / 2;
					player->state = FALL;
					player->countAnim = 0.0f; player->patternAnim = 0;
					SetShake(HIT_SHAKE);
					break;

				default:
					break;
				}
				player->atkDetect = TRUE;
				break;
			}
			cof += 0.5f;
		}
	}
}

void CheckHitEnemy(PLAYER* player, EFFECT* effect)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		ENEMY* s_Enemy = GetEnemy() + i;
		if (s_Enemy->use && s_Enemy->state != DEAD &&
			BBCollision(&effect->pos, &s_Enemy->pos, effect->w, s_Enemy->w, effect->h, s_Enemy->h))  
		{
			int rand_effect = rand() % 2;
			SetEffect(player->pos.x, player->pos.y,
				PLAYER_HIT + rand_effect, player->atkOrient);

			HitEnemy(s_Enemy, 1, player->atkOrient);

			SetShake(HIT_SHAKE);
			player->atkDetect = TRUE;

			switch (player->atkOrient)
			{
			case RIGHT:
			case LEFT:

				player->state = STUN;
				player->countAnim = 0.0f; player->patternAnim = 0;
				if (player->atkOrient / 2) player->horzSpd = MINI_STUN_HSPD;
				else player->horzSpd = -MINI_STUN_HSPD;
				player->actCount = MINI_STUN_FRAME;
				break;

			case DOWN:
				player->vertSpd = HITJUMP_SPD;
				player->state = BIG_JUMP;
				player->countAnim = 0.0f; player->patternAnim = 0;
				break;

			case UP:
				player->vertSpd = 10;
				player->state = FALL;
				player->countAnim = 0.0f; player->patternAnim = 0;
				break;

			default:
				break;
			}
			player->atkDetect = TRUE;
			break;
		}
	}
}

void GetTrigger(PLAYER* player)
{
	if (player->state < DASH || player->state > ATTACK)
	{
		if (player->stamina < PLAYER_STAMINA_MAX)
		{
			player->stamina++;
		}

		if (GetKeyboardPress(DIK_D))
		{
			player->orient = RIGHT;

			if (!GetTerrain(player->pos.x + player->w / 2, player->pos.y))
			{
				player->pos.x += RUN_SPEED;
				if (player->state == STAND)
				{
					player->state = RUN;
					player->countAnim = 0.0f; player->patternAnim = 0;
				}
			}
			else
			{
				if (GetTerrain(player->pos.x, player->pos.y + player->h / 2))
				{
					player->state = STAND;
					player->countAnim = 0.0f; player->patternAnim = 0;
				}
			}
		}
		else if (GetKeyboardPress(DIK_A))		     
		{
			player->orient = LEFT;
			if (!GetTerrain(player->pos.x - player->w / 2, player->pos.y))
			{
				player->pos.x -= RUN_SPEED;
				if (player->state == STAND)
				{
					player->state = RUN;
					player->countAnim = 0.0f; player->patternAnim = 0;
				}
			}
			else
			{
				if (GetTerrain(player->pos.x, player->pos.y + player->h / 2))
				{
					player->state = STAND;
					player->countAnim = 0.0f; player->patternAnim = 0;
				}
			}
		}
		else
		{
			if (player->state == RUN)
			{
				player->state = STAND;
				player->countAnim = 0.0f; player->patternAnim = 0;
			}
		}

		if (GetKeyboardTrigger(DIK_LSHIFT))
		{
			if (player->stamina >= DASH_COST)
			{
				player->stamina -= DASH_COST;
				player->state = DASH;
				player->elev = nullptr;
				player->actCount = DASH_FRAME;

				PlaySound(SOUND_LABEL_SE_dash);
			}
		}
		else if (GetKeyboardTrigger(DIK_J))
		{
			if (player->stamina >= ATK_COST)
			{
				player->stamina -= ATK_COST;

				if (player->pryDetect)
				{
					player->state = SLASH;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->actCount = SLASH_FRAME;

					PlaySound(SOUND_LABEL_SE_dash);
				}
				else
				{
					player->state = ATTACK;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->actCount = ATK_FRAME;
				}
			}
		}
		else if (GetKeyboardTrigger(DIK_K))
		{
			if (player->stamina >= PARRY_COST)
			{
				player->stamina -= PARRY_COST;
				player->state = PARRY;
				player->countAnim = 0.0f; player->patternAnim = 0;
				player->actCount = PARRY_FRAME;
			}
		}
		else if (GetKeyboardTrigger(DIK_R))
		{
			player->state = HEAL;
			player->countAnim = 0.0f; player->patternAnim = 0;
			player->actCount = HEAL_FRAME;
		}
		else if (player->state < DASH && GetKeyboardTrigger(DIK_SPACE))
		{
			if (player->stamina >= JUMP_COST)
			{
				player->elev = nullptr;
				player->stamina -= JUMP_COST;
				player->state = JUMP;
				player->countAnim = 0.0f; player->patternAnim = 0;
				player->vertSpd = JUMP_SPEED;
			}
		}

		if (player->state == JUMP && GetKeyboardPress(DIK_SPACE) &&
			player->vertSpd <= JUMP_SPEED * 1 / 2 && player->vertSpd >= JUMP_SPEED * 4 / 7)
		{
			player->state = BIG_JUMP;

			player->countAnim = 0.0f; player->patternAnim = 0;
			player->vertSpd += JUMP_SPEED / 3;
		}
	}
}