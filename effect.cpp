#include "effect.h"
#include "sprite.h"
#include "main.h"
#include "terrain.h"
#include "player.h"
#include "collision.h"
#include "sound.h"

#include <math.h>

#define M_PI				3.14159265358979323846
#define ANIM_WAIT			2
#define EFFECT_PNG_W		3780
#define EFFECT_PNG_H		1714

#define DUST_SPD			3.2f
#define COIN_SPD			10.0f
#define DUST_CIR_SPD		10.0f
#define MAGIC_CIR_SPD		15.0f

#define PARTICLE_ROT		0.1f

enum TEXTURE_INFO
{
	WIDTH,
	HEIGHT,
	DISTANCE,
	FRAME,
	INFO_MAX
};

static const int TEXTURE_MAX = 1; 
static const int EFFECT_MAX = 100;
static const float c_TextureInfo[EFFECT_TYPE_MAX][INFO_MAX] =
{
	{200.0f,	100.0f,		100.0f,		4.0f},
	{200.0f,	100.0f,		100.0f,		4.0f},
	{100.0f,	100.0f,		50.0f,		5.0f},
	{600.0f,	300.0f,		50.0f,		4.0f},
	{600.0f,	300.0f,		50.0f,		4.0f},
	{100.0f,	100.0f,		50.0f,		5.0f},
	{400.0f,	100.0f,		200.0f,		4.0f},
	{100.0f,	100.0f,		0.0f,		1.0f},
	{140.0f,	200.0f,		0.0f,		27.0f},
	{100.0f,	100.0f,		0.0f,		6.0f},
	{50.0f,	    50.0f,		0.0f,		12.0f},
	{87.0f,	    50.0f,		0.0f,		12.0f},
	{50.0f,	    50.0f,		0.0f,		12.0f},
	{64.0f,		64.0f,		0.0f,		16.0f}
};

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/effect.png",
};

static BOOL		g_Load = FALSE;		 
static EFFECT	g_Effect[EFFECT_MAX];

HRESULT InitEffect(void)
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

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		(g_Effect + i)->use = FALSE;
	}

	g_Load = TRUE;	 
	return S_OK;
}

void UninitEffect(void)
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

void UpdateEffect(void)
{
	PLAYER* s_Player = GetPlayer();

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		EFFECT* s_Effect = g_Effect + i;
		if (s_Effect->use)
		{
			switch (s_Effect->type)
			{
			case COIN:
			{
				if (s_Effect->pos.y - s_Effect->h / 2 < 0 ||
					s_Effect->pos.y + s_Effect->h / 2 > GetTerrain()->h)
				{
					s_Effect->use = FALSE;
				}
				if (s_Effect->bounce < 2)
				{
					s_Effect->pos.y += s_Effect->vertSpd;
					s_Effect->pos.x += s_Effect->horzSpd;

					s_Effect->vertSpd += GRAVITATIONAL_CONST;
					if (s_Effect->vertSpd > FALL_LIMIT)
					{
						s_Effect->vertSpd = FALL_LIMIT;
					}

					if (GetTerrain(s_Effect->pos.x, s_Effect->pos.y + s_Effect->h / 2))
					{
						s_Effect->pos = ReloacteObj(s_Effect->pos.x, s_Effect->pos.y, s_Effect->w, s_Effect->h);
						if (!s_Effect->bounce)
						{
							s_Effect->bounce++;
							s_Effect->vertSpd *= -0.5f;
							s_Effect->horzSpd *= -0.5f;
						}
						else
						{
							s_Effect->bounce++;
							s_Effect->vertSpd = 0.0f;
							s_Effect->horzSpd = 0.0f;
						}
					}
				}

				if (s_Effect->countAnim++ > ANIM_WAIT)
				{
					s_Effect->countAnim = 0.0f;
					if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
					{
						s_Effect->patternAnim = 0;
					};
				}

				if (BBCollision(&s_Effect->pos, &s_Player->pos,
					s_Effect->w, s_Player->w, s_Effect->h, s_Player->h))
				{
					PlaySound(SOUND_LABEL_SE_coin);
					GetPlayer()->money++;
					s_Effect->use = FALSE;
				}
				break;
			}
			case PLAYER_BLADE:
			case PLAYER_BLADE2:
			case PLAYER_SLASH:
			case PLAYER_HEAL:
			{
				PLAYER* s_Player = GetPlayer();
				float eX = s_Player->pos.x + c_TextureInfo[s_Effect->type][DISTANCE] * (float)cos(s_Effect->rot.z);
				float eY = s_Player->pos.y + c_TextureInfo[s_Effect->type][DISTANCE] * (float)sin(s_Effect->rot.z);
				s_Effect->pos = D3DXVECTOR3(eX, eY, 0.0f);

				if (s_Effect->countAnim++ > ANIM_WAIT)
				{
					s_Effect->countAnim = 0.0f;
					if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
					{
						s_Effect->use = FALSE;
					};
				}
				break;
			}
			case RED_DOT:
			{
				break;
			}
			case DUST:
				s_Effect->pos.y += s_Effect->vertSpd;
				s_Effect->pos.x += s_Effect->horzSpd;

				s_Effect->vertSpd *= 0.96f;
				s_Effect->horzSpd *= 0.96f;

				if (s_Effect->countAnim++ > ANIM_WAIT)
				{
					s_Effect->countAnim = 0.0f;
					if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
					{
						s_Effect->use = FALSE;
					};
				}
				break;
			case DUST_CIRCLE:
				s_Effect->pos.y += s_Effect->vertSpd;
				s_Effect->pos.x += s_Effect->horzSpd;

				s_Effect->vertSpd *= 0.96f;
				s_Effect->horzSpd *= 0.96f;
				s_Effect->w *= 0.96f;
				s_Effect->h *= 0.96f;
				s_Effect->rot += D3DXVECTOR3(0.0f, 0.0f, PARTICLE_ROT);

				if (s_Effect->countAnim++ > ANIM_WAIT)
				{
					s_Effect->countAnim = 0.0f;
					if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
					{
						s_Effect->use = FALSE;
					};
				}
				break;

			case MAGIC_CIRCLE:
				s_Effect->pos.y += s_Effect->vertSpd;
				s_Effect->pos.x += s_Effect->horzSpd;

				s_Effect->vertSpd *= 0.96f;
				s_Effect->horzSpd *= 0.96f;
				s_Effect->rot += D3DXVECTOR3(0.0f, 0.0f, PARTICLE_ROT);

				if (s_Effect->countAnim++ > ANIM_WAIT)
				{
					s_Effect->countAnim = 0.0f;
					if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
					{
						s_Effect->use = FALSE;
					};
				}
				break;
			default:
			{
				if (s_Effect->countAnim++ > ANIM_WAIT)
				{
					s_Effect->countAnim = 0.0f;
					if (++s_Effect->patternAnim >= (int)c_TextureInfo[s_Effect->type][FRAME])
					{
						s_Effect->use = FALSE;
					};
				}
				break;
			}
			}

#ifdef _DEBUG
#endif
		}
	}
}

void DrawEffect(void)
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

	Terrain* s_Map = GetTerrain();

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		EFFECT* s_Effect = g_Effect + i;
		if (s_Effect->use)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			float ex = s_Effect->pos.x - s_Map->pos.x;
			float ey = s_Effect->pos.y - s_Map->pos.y;

			float tw = 1.0f / EFFECT_PNG_W * c_TextureInfo[s_Effect->type][WIDTH];
			float th = 1.0f / EFFECT_PNG_H * c_TextureInfo[s_Effect->type][HEIGHT];
			float tx = (float)s_Effect->patternAnim * tw;
			float sumH = 0.0f;
			for (int i = 0; i < s_Effect->type; i++)
			{
				sumH += c_TextureInfo[i][HEIGHT];
			}
			float ty = 1.0f / EFFECT_PNG_H * sumH;
			SetSpriteColorRotation(g_VertexBuffer, ex, ey, s_Effect->w, s_Effect->h,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				s_Effect->rot.z);

			GetDeviceContext()->Draw(4, 0);
		}
	}
}

EFFECT* SetEffect(float X, float Y, int Type, int orient)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		EFFECT* s_Effect = g_Effect + i;
		if (!s_Effect->use)
		{
			s_Effect->type = Type;
			s_Effect->rot = D3DXVECTOR3(0.0f, 0.0f, (float)M_PI / 2 * orient);
			float eX = X + c_TextureInfo[Type][DISTANCE] * (float)cos(s_Effect->rot.z);
			float eY = Y + c_TextureInfo[Type][DISTANCE] * (float)sin(s_Effect->rot.z);
			s_Effect->pos = D3DXVECTOR3(eX, eY, 0.0f);
			s_Effect->w = c_TextureInfo[Type][WIDTH];
			s_Effect->h = c_TextureInfo[Type][HEIGHT];
			s_Effect->countAnim = 0.0f;
			s_Effect->patternAnim = 0;
			s_Effect->texNo = 0;
			s_Effect->bounce = 0;
			s_Effect->use = TRUE;

			float rand_angle = ((rand() % 157) + 157) / 100.0f;		    
			float rand_cof_x = (rand() % 200 - 100) / 100.0f;		   
			float rand_cof_y = (rand() % 200 - 100) / 100.0f;		   
			switch (s_Effect->type)
			{
			case COIN:
				s_Effect->horzSpd = COIN_SPD * rand_cof_x;
				s_Effect->vertSpd = COIN_SPD * rand_cof_y;
				break;
			case DUST:
				s_Effect->horzSpd = DUST_SPD * sinf(rand_angle);
				s_Effect->vertSpd = DUST_SPD * cosf(rand_angle);
				s_Effect->horzSpd *= (GetPlayer()->orient) ? 1 : -1;
				break;
			case DUST_CIRCLE:
				s_Effect->horzSpd = DUST_CIR_SPD * rand_cof_x;
				s_Effect->vertSpd = DUST_CIR_SPD * rand_cof_y;
				break;
			case MAGIC_CIRCLE:

				s_Effect->horzSpd = MAGIC_CIR_SPD * rand_cof_x;
				s_Effect->vertSpd = MAGIC_CIR_SPD * rand_cof_y;
				switch (orient)
				{
					s_Effect->horzSpd = MAGIC_CIR_SPD * rand_cof_x;
					s_Effect->vertSpd = MAGIC_CIR_SPD * rand_cof_y;
				case RIGHT:
					if (s_Effect->horzSpd < 0)
					{
						s_Effect->horzSpd = -s_Effect->horzSpd;
					}
					s_Effect->vertSpd /= 2;
					break;
				case LEFT:
					if (s_Effect->horzSpd > 0)
					{
						s_Effect->horzSpd = -s_Effect->horzSpd;
					}
					s_Effect->vertSpd /= 2;
					break;
				case UP:
					if (s_Effect->vertSpd > 0)
					{
						s_Effect->vertSpd = -s_Effect->vertSpd;
					}
					s_Effect->horzSpd /= 2;
					break;
				case DOWN:
					if (s_Effect->vertSpd < 0)
					{
						s_Effect->vertSpd = -s_Effect->vertSpd;
					}
					s_Effect->horzSpd /= 2;
					break;
				default:
					break;
				}

				break;
			default:
				s_Effect->horzSpd = 0;
				s_Effect->vertSpd = 0;
				break;
			}
			return s_Effect;
		}
	}
	return nullptr;
}