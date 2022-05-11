//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : LI ZIZHEN liruntu2333@gmail.com
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "map.h"
#include "bullet.h"
#include "file.h"
#include "effect.h"
#include "collision.h"
#include "elevator.h"
#include "fade.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(75.0f)	// キャラサイズ
#define TEXTURE_HEIGHT				(75.0f)	//
#define TEXTURE_MAX					(1)		// テクスチャの数
#define	PLAYER_TEX_NO				(0)

#define PLAYER_PNG_W				900
#define PLAYER_PNG_H				1050

#define UP_ATTACK_Y					13
#define DOWN_ATTACK_Y				12

//#define TEXTURE_PATTERN_DIVIDE_X	(5)		// アニメパターンのテクスチャ内分割数（X)
//#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
//#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(3)		// アニメーションの切り替わるWait値

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

// Starting point in texture & animation frame
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

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

void CheckHitTerra(PLAYER* player, EFFECT* effect);
void CheckHitEnemy(PLAYER* player, EFFECT* effect);
void GetTrigger(PLAYER* player);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = nullptr;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	// テクスチャ情報

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/player.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体

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

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
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

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;

		s_Player->use = TRUE;
		s_Player->pos = D3DXVECTOR3(TEXTURE_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 0.0f);	// 中心点から表示
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

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
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

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;
		// Only proceed with active player
		if (s_Player->use == TRUE)
		{
			if (s_Player->godCount) s_Player->godCount--;

			if (s_Player->elev)
			{
				s_Player->pos.y += s_Player->elev->vertSpd;
			}

			// Input that changes current state of player.
			GetTrigger(s_Player);

			switch (s_Player->state)
			{
			case RUN:
			{
				if (!(GetFrameCount() % DUST_GEN_LAG))
				{
					PlaySound(SOUND_LABEL_SE_walk);
					SetEffect(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2, DUST, 0);
				}
				// Object is in the air, therefore should start to fall.
				if (s_Player->elev)
				{
					if (s_Player->pos.x < s_Player->elev->pos.x ||
						s_Player->pos.x > s_Player->elev->pos.x + s_Player->elev->w)
					{
						s_Player->state = FALL;
						s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;
						s_Player->elev = nullptr;
					}
				}
				else if (!GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
				{
					// Start to fall when walk across the edge.
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;
				}
				break;
			}
			case STAND:
			{
				break;
			}
			//case STAND_ELEV:
			//{
			//	//s_Player->pos.y += s_Player->elev->vertSpd;
			//	break;
			//}
			//case RUN_ELEV:
			//{
			//	if (!(GetFrameCount() % DUST_GEN_LAG))
			//	{
			//		SetEffect(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2, DUST, 0);
			//	}

			////	s_Player->pos.y += s_Player->elev->vertSpd;

			//	if (s_Player->pos.x < s_Player->elev->pos.x ||
			//		s_Player->pos.x > s_Player->elev->pos.x + s_Player->elev->w)
			//	{
			//		s_Player->state = FALL;
			//		s_Player->elev = NULL;
			//	}
			//	break;
			//}
			case FALL:
			{
				// Vertical speed effected by gravity when in the air.
				s_Player->pos.y += s_Player->vertSpd;
				s_Player->vertSpd += GRAVITATIONAL_CONST;
				if (s_Player->vertSpd > FALL_LIMIT)
				{
					s_Player->vertSpd = FALL_LIMIT;
				}

				// If there is obstacle under player, stand still & stop falling.
				if (GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
				{
					s_Player->vertSpd = 0;
					s_Player->state = STAND;
					s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;

					s_Player->pos = ReloacteObj(s_Player->pos.x, s_Player->pos.y, s_Player->w, s_Player->h);
					// Check if stand on the ground obstacle of elevator.
					for (int i = 0; i < ELEV_MAX; i++)
					{
						ELEVATOR* s_Elevator = GetElev() + i;
						// Elevator's coordinate is start from left up corner,
						// so cannot directly call BBCollision.
						if (PECollision(&s_Player->pos, &s_Elevator->pos,
							s_Player->w, s_Elevator->w, s_Player->h, s_Elevator->h))
						{
							s_Player->state = STAND;
							s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;
							s_Player->elev = s_Elevator;
						}
					}

					PlaySound(SOUND_LABEL_SE_bounce);
				}
				break;
			}
			case JUMP:
			case BIG_JUMP:
			{
				// Vertical speed effected by gravity when in the air.
				s_Player->pos.y += s_Player->vertSpd;
				s_Player->vertSpd += GRAVITATIONAL_CONST;
				if (s_Player->vertSpd > FALL_LIMIT)
				{
					s_Player->vertSpd = FALL_LIMIT;
				}
				if (s_Player->vertSpd >= 0)
				{
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				// If there is obstacle above player, reverse vertical speed to +1.
				if (GetTerrain(s_Player->pos.x, s_Player->pos.y - s_Player->h / 2))
				{
					s_Player->pos.y += 5.0f;
					s_Player->vertSpd = 1;
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				break;
			}
			case DASH:
			{
				// Dash process.
				//
				// Copy player's data to illusion array. Illusion generates every 10 frames.
				if ((s_Player->actCount % ILLUSION_GEN_LAG) == 0) // Check if it's time to generate illusion
				{
					for (int i = 0; i < ILLUSION_MAX; i++)
					{
						if (!(g_Illusion + i)->illusion.use)
						{
							(g_Illusion + i)->illusion = *s_Player;
							(g_Illusion + i)->life = ILLUSION_LIFE_SPAN;
							g_Illusion->illusion.use = TRUE;
							break;
						}
					}
				}

				if (s_Player->actCount == DASH_FRAME - DASH_GOD_ST)
				{
					s_Player->godCount = DASH_GOD_END - DASH_GOD_ST;
				}

				if (GetFrameCount() % 2)
				{
					SetEffect(s_Player->pos.x, s_Player->pos.y, DUST_CIRCLE, 0);
				}

				switch (s_Player->orient)
				{
				case LEFT:								// Dash to left
					if (!GetTerrain(s_Player->pos.x - s_Player->w / 2, s_Player->pos.y))
					{
						s_Player->pos.x -= DASH_SPEED;
					}
					break;
				case RIGHT:								// Dash to right
					if (!GetTerrain(s_Player->pos.x + s_Player->w / 2, s_Player->pos.y))
					{
						s_Player->pos.x += DASH_SPEED;
					}
					break;
				default:
					break;
				}

				// While dashing, dashCount degresses per frame until reaches 0
				if (!s_Player->actCount--)
				{
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f; s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
					s_Player->vertSpd = 0;
				}
				break;
			}
			case ATTACK:
			{
				// Attack process.
				if (s_Player->actCount == ATK_FRAME)
				{
					// Check atk direction.
					if (GetKeyboardPress(DIK_W))
					{
						s_Player->atkOrient = UP;
					}
					else if (GetKeyboardPress(DIK_S))
					{
						s_Player->atkOrient = DOWN;
						// Player can't attack downward when isn't in air, nor on elevator.
						if (s_Player->elev || GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
						{
							s_Player->stamina += ATK_COST;
							s_Player->actCount = 0;
							s_Player->state = FALL;
							s_Player->countAnim = 0.0f;
							s_Player->patternAnim = 0;
							break;
						}
					}
					else if (GetKeyboardPress(DIK_D))
					{
						s_Player->atkOrient = RIGHT;
					}
					else if (GetKeyboardPress(DIK_A))
					{
						s_Player->atkOrient = LEFT;
					}
					else
					{
						s_Player->atkOrient = s_Player->orient;
					}
					s_Player->vertSpd = 0;
					s_Player->atkDetect = FALSE;

					PlaySound(SOUND_LABEL_SE_attck);
				}

				if (s_Player->actCount == ATK_FRAME - ATK_DETECTION)
				{
					// blade effect bonds to start frame of detection
					int rand_effect = rand() % 2;
					s_Player->effect = SetEffect(s_Player->pos.x, s_Player->pos.y,
						PLAYER_BLADE + rand_effect, s_Player->atkOrient);
				}

				// Attack dectecting process, hit detection should do only once.
				if (!s_Player->atkDetect &&
					s_Player->actCount < ATK_FRAME - ATK_DETECTION && s_Player->actCount > ATK_FRAME - ATK_END)
				{
					EFFECT* s_Effect = s_Player->effect;

					// Do enemy detection first.
					CheckHitEnemy(s_Player, s_Effect);

					// Do enviroment detection next.
					CheckHitTerra(s_Player, s_Effect);
				}

				// While attacking, dashCount degresses per frame until reaches 0
				if (!s_Player->actCount--)
				{
					s_Player->atkOrient = s_Player->orient;
					s_Player->effect = nullptr;
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f;
					s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				break;
			}
			case SLASH:
			{
				// Attack process.
				if (s_Player->actCount == SLASH_FRAME)
				{
					// Super slash after sucessfully parried.
					// Return to 60 FPS
					SetSlowMotion(1);
					PlaySound(SOUND_LABEL_SE_pasueReturn);
					// Set next hit to normal attack.
					s_Player->pryDetect = FALSE;
					s_Player->atkOrient = s_Player->orient;
					// slash effect
					s_Player->effect = SetEffect(s_Player->pos.x, s_Player->pos.y,
						PLAYER_SLASH, s_Player->atkOrient);

					s_Player->vertSpd = 0;
					s_Player->atkDetect = FALSE;
				}

				// Slash dectecting process, hit detection has no limit.
				if (s_Player->actCount > SLASH_DETECTION && s_Player->actCount <= SLASH_END)
				{
					EFFECT* s_Effect = s_Player->effect;

					// Do enemy detection only.
					for (int i = 0; i < ENEMY_MAX; i++)
					{
						ENEMY* s_Enemy = GetEnemy() + i;
						if (BBCollision(&s_Effect->pos, &s_Enemy->pos,
							s_Effect->w, s_Enemy->w, s_Effect->h, s_Enemy->h) &&
							s_Enemy->slashed == FALSE && s_Enemy->state != DEAD) // Bullseye
						{
							// Hit effect.
							SetEffect(s_Enemy->pos.x, s_Enemy->pos.y,
								PLAYER_HIT, s_Player->atkOrient);
							// Do damage to enemy.
							HitEnemy(s_Enemy, 2, s_Player->atkOrient);

							// Shake effect.
							SetShake(SLASH_SHAKE);
						}
					}
				}

				// Dashing while slashing
				switch (s_Player->orient)
				{
				case LEFT:								// Dash to left
					if (!GetTerrain(s_Player->pos.x - s_Player->w / 2, s_Player->pos.y))
					{
						s_Player->pos.x -= SLASH_SPD;
					}
					break;
				case RIGHT:								// Dash to right
					if (!GetTerrain(s_Player->pos.x + s_Player->w / 2, s_Player->pos.y))
					{
						s_Player->pos.x += SLASH_SPD;
					}
					break;
				default:
					break;
				}

				// While attacking, dashCount degresses per frame until reaches 0
				if (!s_Player->actCount--)
				{
					s_Player->effect = nullptr;
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f;
					s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				break;
			}
			case PARRY:
			{
				// Parry process.

				// While parrying, actCount degresses per frame until reaches 0
				if (!s_Player->actCount--)
				{
					s_Player->vertSpd = 0;
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f;
					s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				break;
			}
			case STUN:
			{
				// Stun process.
				if (s_Player->actCount--)
				{
					// Bounce to right.
					if (s_Player->horzSpd > 0)
					{
						if (!GetTerrain(s_Player->pos.x + s_Player->w / 2, s_Player->pos.y))
						{
							s_Player->pos.x += s_Player->horzSpd;
						}
					}
					// Bouce to left.
					else if (s_Player->horzSpd < 0)
					{
						if (!GetTerrain(s_Player->pos.x - s_Player->w / 2, s_Player->pos.y))
						{
							s_Player->pos.x += s_Player->horzSpd;
						}
					}

					s_Player->horzSpd = (int)(0.9f * s_Player->horzSpd);

					if (!GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
					{
						s_Player->pos.y += s_Player->vertSpd;
						s_Player->vertSpd += GRAVITATIONAL_CONST;
						if (s_Player->vertSpd > FALL_LIMIT)
						{
							s_Player->vertSpd = FALL_LIMIT;
						}
					}
				}
				else
				{
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f;
					s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				break;
			}
			case DEAD:
			{
				// Death process.
				if (!s_Player->actCount--)
				{
					SetFade(FADE_OUT, MODE_GAME);
				}
				else
				{
					SetEffect(s_Player->pos.x, s_Player->pos.y, DUST_CIRCLE, rand() % 4);
				}
				break;
			}
			case HEAL:
			{
				// Heal process.
				if (s_Player->actCount--)
				{
					// Set Heal effect at frame no.60
					if (s_Player->actCount == HEAL_FRAME - HEAL_START)
					{
						if (s_Player->money >= HEAL_COST)
						{
							s_Player->effect = SetEffect(s_Player->pos.x, s_Player->pos.y, PLAYER_HEAL, 0);
							s_Player->money -= HEAL_COST;
						}
						else
						{
							s_Player->state = FALL;
							s_Player->vertSpd = 0;
							s_Player->countAnim = 0.0f;
							s_Player->patternAnim = 0;
						}
					}
					if (s_Player->actCount == HEAL_FRAME - HEAL_TAKE_EFFECT)
					{
						PlaySound(SOUND_LABEL_SE_heal);
						s_Player->health += (s_Player->health < PLAYER_HEALTH_MAX) ? 1 : 0;
					}
				}
				else
				{
					s_Player->effect = nullptr;
					s_Player->vertSpd = 0;
					s_Player->state = FALL;
					s_Player->countAnim = 0.0f;
					s_Player->patternAnim = 0;
					s_Player->elev = nullptr;
				}
				break;
			}
			default:
				break;
			}

			// Illusion's life decrease to 0
			for (int i = 0; i < ILLUSION_MAX; i++)
			{
				if ((g_Illusion + i)->illusion.use)
				{
					if (!--(g_Illusion + i)->life)
						(g_Illusion + i)->illusion.use = FALSE;
				}
			}

			// Animation
			if (s_Player->countAnim++ > ANIM_WAIT)
			{
				s_Player->countAnim = 0.0f;
				// Change pattern and check if it's the end of animation.
				if (++s_Player->patternAnim >= c_TexInfo[s_Player->state].frame)
				{
					s_Player->patternAnim = 0;
				}
			}

#ifdef _DEBUG
			// デバッグ表示
			//PrintDebugProc("Player X:%f Y:%f vS: %d BGd: %d Health: %d State: %d \n",
			//	s_Player->pos.x,
			//	s_Player->pos.y,
			//	s_Player->vertSpd,
			//	GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2),
			//	s_Player->health,
			//	s_Player->state);
#endif
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG* bg = GetMap();
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;
		if (s_Player->use == TRUE && s_Player->state != DASH)		// このプレイヤーが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Player->texNo]);

			// Calculate the relative position of player
			float px = s_Player->pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = s_Player->pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = s_Player->w;		// プレイヤーの表示幅
			float ph = s_Player->h;		// プレイヤーの表示高さ

			// Calculate the parameter for animation
			float tw = 1.0f / PLAYER_PNG_W * TEXTURE_WIDTH;
			float th = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
			float tx = tw * s_Player->patternAnim;
			float sumH = 0.0f;
			float ty = 1.0f / PLAYER_PNG_H * c_TexInfo[s_Player->state].startY;

			// Considering state and orient, set the param to right row.
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
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[s_Illusion->illusion.texNo]);

			// Calculate the relative position of player
			float px = s_Illusion->illusion.pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = s_Illusion->illusion.pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = s_Illusion->illusion.w;		// プレイヤーの表示幅
			float ph = s_Illusion->illusion.h;		// プレイヤーの表示高さ

			// Calculate the parameter for animation
			float tw = 1.0f / PLAYER_PNG_W * TEXTURE_WIDTH;
			float th = 1.0f / PLAYER_PNG_H * TEXTURE_HEIGHT;
			float tx = tw * s_Illusion->illusion.patternAnim;
			float sumH = 0.0f;
			float ty = 1.0f / PLAYER_PNG_H * c_TexInfo[s_Illusion->illusion.state].startY;

			// Considering state and orient, set the param to right row.
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

//=============================================================================
// プレイヤー構造体の先頭アドレスを取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

//
// @brief	Decrease player's health and switch state to STUN.
// @param	enemy pointer, dam, orient
// @return
//
void HitPlayer(ENEMY* enemy, PLAYER* player, int damge, int orient)
{
	// do nothing if player's in god mode.
	if (!player->godCount)
	{
		// Sucessfully parried.
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

		// Enemy deals damage to player, player steps backwards, turn to god mode
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

//
// @brief	Do attack detection in terrain, such as hitting
//			on wall get a reverse force
// @param	Player pointer, attack effect used as collision box.
//
void CheckHitTerra(PLAYER* player, EFFECT* effect)
{
	if (!player->atkDetect)
	{
		// Process to check hitting on enviroment
		float cof = -1.0f;
		float radius = effect->w / 2;
		// Math use to find hit point in enviroment.
		const float rcosrot = radius * (float)cos(effect->rot.z);
		const float rsinrot = radius * (float)sin(effect->rot.z);
		while (cof <= 1.0f)
		{
			// Check if hit on undestroyable object (enviroment bloack).
			if (GetTerrain(effect->pos.x + cof * rcosrot, effect->pos.y + cof * rsinrot))
			{
				PlaySound(SOUND_LABEL_SE_reflect);

				// Varies from direction to direction, step back, jump.
				switch (player->atkOrient)
				{
				case RIGHT:
				case LEFT:
					// Reflect effect.
					SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
						PLAYER_REFLECT, player->atkOrient);
					// stop playing Blade effect cause hit on the wall
					player->effect->use = FALSE;
					player->effect = nullptr;

					player->state = STUN;
					player->countAnim = 0.0f; player->patternAnim = 0;
					if (player->atkOrient % 4)
						player->horzSpd = MINI_STUN_HSPD;
					else
						player->horzSpd = -MINI_STUN_HSPD;
					player->actCount = MINI_STUN_FRAME;
					// Screen shake effect
					SetShake(HIT_SHAKE);
					break;

				case DOWN:
					if (!GetTerrain(player->pos.x, player->pos.y + player->h / 2))
					{
						// Reflect effect.
						SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
							PLAYER_REFLECT, player->atkOrient);
						player->vertSpd = HITJUMP_SPD;
						player->state = BIG_JUMP;
						player->countAnim = 0.0f; player->patternAnim = 0;
						// Screen shake effect
						SetShake(HIT_SHAKE);
					}
					break;

				case UP:
					// Reflect effect.
					SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
						PLAYER_REFLECT, player->atkOrient);
					player->vertSpd = -HITJUMP_SPD / 2;
					player->state = FALL;
					player->countAnim = 0.0f; player->patternAnim = 0;
					// Screen shake effect
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

//
// @brief	Do attack detection on enemy. Just like what happens when hit the terrain,
//			also has feedback suck as reverse force and hit effect.
// @param	Player pointer, attack effect used as collision box.
//
void CheckHitEnemy(PLAYER* player, EFFECT* effect)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		ENEMY* s_Enemy = GetEnemy() + i;
		if (s_Enemy->use && s_Enemy->state != DEAD &&
			BBCollision(&effect->pos, &s_Enemy->pos, effect->w, s_Enemy->w, effect->h, s_Enemy->h)) // Bullseye
		{
			// Hit effect.
			int rand_effect = rand() % 2;
			SetEffect(player->pos.x, player->pos.y,
				PLAYER_HIT + rand_effect, player->atkOrient);

			// Do damage to enemy.
			HitEnemy(s_Enemy, 1, player->atkOrient);

			// Shake effect.
			SetShake(HIT_SHAKE);
			player->atkDetect = TRUE;

			// Varies from direction to direction, step back, jump.
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

//
// @brief	Gathering KeyBoard inputs, immediately change player's state
//			to ATTACK / DASH / JUMP / PARRY etc in this frame.
// @param	Player pointer
//
void GetTrigger(PLAYER* player)
{
	// While in uncontrollable states like DASH ~ ATTACK,
	// you can't do anything until act finished.
	if (player->state < DASH || player->state > ATTACK)
	{
		// Stanima regen.
		if (player->stamina < PLAYER_STAMINA_MAX)
		{
			player->stamina++;
		}

		//
		// INSTRUCTION PRIORITY:
		// dash > attack > parry > jump >> move rht > move lft > stand(no input)
		//
		// Move trigger.
		if (GetKeyboardPress(DIK_D))
		{
			player->orient = RIGHT;

			if (!GetTerrain(player->pos.x + player->w / 2, player->pos.y))
			{
				player->pos.x += RUN_SPEED;
				// Player's state should be RUN / RUN ON ELEV
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
		else if (GetKeyboardPress(DIK_A))		// Move right overrides move left.
		{
			player->orient = LEFT;
			if (!GetTerrain(player->pos.x - player->w / 2, player->pos.y))
			{
				player->pos.x -= RUN_SPEED;
				// Player's state should be RUN / RUN ON ELEV
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
			// No instruction coming from keyboard, player does nothing but stand.
			if (player->state == RUN)
			{
				player->state = STAND;
				player->countAnim = 0.0f; player->patternAnim = 0;
			}
		}

		// Dash trigger.
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
		// Attack trigger.
		else if (GetKeyboardTrigger(DIK_J))
		{
			if (player->stamina >= ATK_COST)
			{
				player->stamina -= ATK_COST;

				// Super slash after successfully parried
				if (player->pryDetect)
				{
					player->state = SLASH;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->actCount = SLASH_FRAME;

					PlaySound(SOUND_LABEL_SE_dash);
				}
				// Normal attack.
				else
				{
					player->state = ATTACK;
					player->countAnim = 0.0f; player->patternAnim = 0;
					player->actCount = ATK_FRAME;
				}
			}
		}
		// Parry trigger.
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
		// Heal trigger.
		else if (GetKeyboardTrigger(DIK_R))
		{
			player->state = HEAL;
			player->countAnim = 0.0f; player->patternAnim = 0;
			player->actCount = HEAL_FRAME;
		}
		// Jump only triggers when player's on the ground(STAND ~ STAND_ELEV).
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

		// Big Jump trigger
		if (player->state == JUMP && GetKeyboardPress(DIK_SPACE) &&
			player->vertSpd <= JUMP_SPEED * 1 / 2 && player->vertSpd >= JUMP_SPEED * 4 / 7)
		{
			player->state = BIG_JUMP;

			player->countAnim = 0.0f; player->patternAnim = 0;
			player->vertSpd += JUMP_SPEED / 3;
		}
	}
}