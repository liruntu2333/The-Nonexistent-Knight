//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 
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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(800/8)	// キャラサイズ
#define TEXTURE_HEIGHT				(800/8)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(5)		// アニメーションの切り替わるWait値

#define ILLUSION_MAX				(20)

#define RUN_SPEED					(8.0f)

#define JUMP_SPEED					(-25)
#define HITJUMP_SPD					(-20)

#define DASH_SPEED					(15)
#define DASH_FRAME					(20)
#define DASH_GOD_ST					(0)
#define DASH_GOD_END				(15)
#define ATK_FRAME					(8)
#define ATK_DETECTION				(3)
#define ATK_END						(7)

#define STAMINA_MAX					(50)
#define ATK_COST					(30)
#define DASH_COST					(20)
#define JUMP_COST					(10)

#define MINI_STUN_FRAME				(10)
#define MINI_STUN_HSPD				(10)
#define HIT_STUN_FRAME				(20)
#define HIT_STUN_HSPD				(20)
#define HIT_STUN_VSPD				(10)

#define GOD_FRAME					(120)
#define GOD_FLASH					(5)

#define	ILLUSION_LIFE_SPAN			(30)
#define ILLUSION_GEN_LAG			(4)

#define HIT_SHAKE					(10)
#define HITBYENEMY_SHAKE			(20)

enum DETECTION
{
	LIGHT_ATK,
	DETECTION_MAX,
};

struct ILLUSION
{
	PLAYER illusion;
	int life;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/runningman000.png",
	"data/TEXTURE/runningman002.png",
};

static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体

static ILLUSION	g_Illusion[ILLUSION_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		PLAYER* s_Player = g_Player + i;

		s_Player->use = TRUE;
		s_Player->pos = D3DXVECTOR3(TEXTURE_WIDTH/2 + 50, SCREEN_HEIGHT /2 , 0.0f);	// 中心点から表示
		s_Player->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		s_Player->w   = TEXTURE_WIDTH;
		s_Player->h   = TEXTURE_HEIGHT;
		s_Player->texNo = 1;

		s_Player->countAnim = 0;
		s_Player->patternAnim = 0;

		s_Player->state = FALL;
		s_Player->orient = RIGHT;
		s_Player->atkOrient = RIGHT;
		s_Player->atkDetect = TRUE;
		s_Player->vertSpd = 0;
		s_Player->horzSpd = 0;
		s_Player->actCount = 0;
		s_Player->atk = NULL;
		s_Player->elev = NULL;

		s_Player->stamina = STAMINA_MAX;
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

			// While in uncontrollable states like DASH ~ ATTACK,
			// you can't do anything until act finished.
			if (s_Player->state < DASH || s_Player->state > ATTACK)
			{
				// Stanima regen.
				if (s_Player->stamina < STAMINA_MAX)
				{
					s_Player->stamina++;
				}

				// Move right.
				if (GetKeyboardPress(DIK_D))
				{
					s_Player->orient = RIGHT;

					if (!GetTerrain(s_Player->pos.x + s_Player->w / 2, s_Player->pos.y))
					{
						s_Player->pos.x += RUN_SPEED;
					}
				}
				// Move left.
				else if (GetKeyboardPress(DIK_A))
				{
					s_Player->orient = LEFT;
					if (!GetTerrain(s_Player->pos.x - s_Player->w / 2, s_Player->pos.y))
					{
						s_Player->pos.x -= RUN_SPEED;
					}
				}
				// Dash trigger.
				if (GetKeyboardTrigger(DIK_LSHIFT))
				{
					if (s_Player->stamina >= DASH_COST)
					{
						s_Player->elev = NULL;
						s_Player->stamina -= DASH_COST;
						s_Player->state = DASH;
						s_Player->actCount = DASH_FRAME;
					}
				}
				// Attack trigger.
				else if (GetKeyboardTrigger(DIK_J))
				{
					if (s_Player->stamina >= ATK_COST)
					{
						s_Player->elev = NULL;
						s_Player->stamina -= ATK_COST;
						s_Player->state = ATTACK;
						s_Player->actCount = ATK_FRAME;
					}
				}

				// Jump only triggers when player's on the ground(STAND ~ STAND_ELEV).
				if (s_Player->state < DASH && GetKeyboardTrigger(DIK_SPACE))
				{
					if (s_Player->stamina >= JUMP_COST)
					{
						s_Player->stamina -= JUMP_COST;
						s_Player->state = JUMP;
						s_Player->vertSpd = JUMP_SPEED;
					}
				}
				// Big Jump trigger
				if (s_Player->state == JUMP && GetKeyboardPress(DIK_SPACE) &&
					s_Player->vertSpd <= JUMP_SPEED * 1 / 2 && s_Player->vertSpd >= JUMP_SPEED * 4 / 7)
				{
					s_Player->state = BIG_JUMP;
					s_Player->vertSpd += JUMP_SPEED / 3;
				}
			}

			switch (s_Player->state)
			{
			case RUN:
			case STAND:
			{
				// Object is in the air, therefore should start to fall.
				if (!GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
				{
					// Start to fall when walk across the edge.
					s_Player->state = FALL;
				}
				break;
			}
			case STAND_ELEV:
			{
				s_Player->pos.y += s_Player->elev->vertSpd;

				if (s_Player->pos.x < s_Player->elev->pos.x ||
					s_Player->pos.x > s_Player->elev->pos.x + s_Player->elev->w)
				{
					s_Player->state = FALL;
					s_Player->elev = NULL;
				}
				break;
			}

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
					s_Player->pos = ReloacteObj(s_Player->pos.x, s_Player->pos.y, s_Player->w, s_Player->h);

					// Check if stand on the ground obstacle of elevator.
					for (int i = 0; i < ELEV_MAX; i++)
					{
						ELEVATOR* s_Elevator = GetElev() + i;
						if (BBCollision(&s_Player->pos, &s_Elevator->pos,
							s_Player->w, s_Elevator->w, s_Player->h, s_Elevator->h))
						{
							s_Player->state = STAND_ELEV;
							s_Player->elev = s_Elevator;
						}
					}
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
				}
				// If there is obstacle above player, reverse vertical speed to +1.
				if (GetTerrain(s_Player->pos.x, s_Player->pos.y - s_Player->h / 2))
				{
					s_Player->pos.y += 5.0f;
					s_Player->vertSpd = 1;
					s_Player->state = FALL;
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
				if (!--s_Player->actCount)
				{
					s_Player->state = FALL;
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
					s_Player->atk = SetEffect(s_Player->pos.x, s_Player->pos.y,
						PLAYER_BLADE, s_Player->atkOrient);
					s_Player->vertSpd = 0;
					s_Player->atkDetect = FALSE;
				}

				// Attack dectecting process, hit detection should do only once.
				if (!s_Player->atkDetect &&
					s_Player->actCount > ATK_DETECTION && s_Player->actCount <= ATK_END)
				{
					EFFECT* s_Effect = s_Player->atk;

					// Do enemy detection first.
					for (int i = 0; i < ENEMY_MAX; i++)
					{
						ENEMY* s_Enemy = GetEnemy() + i;
						if (BBCollision(&s_Effect->pos, &s_Enemy->pos,
							s_Effect->w, s_Enemy->w, s_Effect->h, s_Enemy->h)) // Bullseye
						{
							// Hit effect.
							SetEffect(s_Player->pos.x, s_Player->pos.y,
								PLAYER_HIT, s_Player->atkOrient);
							// Do damage to enemy.
							HitEnemy(s_Enemy, 0, s_Player->atkOrient);
							
							// Shake effect.
							SetShake(HIT_SHAKE);
							s_Player->atkDetect = TRUE;

							// Varies from direction to direction, step back, jump.
							switch (s_Player->atkOrient)
							{
							case RIGHT:
							case LEFT:

								s_Player->state = STUN;
								if (s_Player->atkOrient / 2) s_Player->horzSpd = MINI_STUN_HSPD;
								else s_Player->horzSpd = -MINI_STUN_HSPD;
								s_Player->actCount = MINI_STUN_FRAME;
								break;

							case DOWN:
								s_Player->vertSpd = -20;
								s_Player->state = BIG_JUMP;
								break;

							case UP:
								s_Player->vertSpd = 10;
								s_Player->state = FALL;
								break;

							default:
								break;
							}
							s_Player->atkDetect = TRUE;
							break;
						}
					}

					// Do enviroment detection next.
					if (!s_Player->atkDetect)
					{
						// Process to check hitting on enviroment
						float cof = -1.0f;
						float radius = s_Effect->w / 2;
						// Math use to find hit point in enviroment.
						const float rcosrot = radius * (float)cos(s_Effect->rot.z);
						const float rsinrot = radius * (float)sin(s_Effect->rot.z);
						while (cof <= 1.0f)
						{
							// Check if hit on undestroyable object (enviroment bloack).
							if (GetTerrain(s_Effect->pos.x + cof * rcosrot, s_Effect->pos.y + cof * rsinrot))
							{
								// Varies from direction to direction, step back, jump.
								switch (s_Player->atkOrient)
								{
								case RIGHT:
								case LEFT:
									// Reflect effect.
									SetEffect(s_Player->pos.x + cof * rcosrot, s_Player->pos.y + cof * rsinrot,
										PLAYER_REFLECT, s_Player->atkOrient);

									s_Player->state = STUN;
									if (s_Player->atkOrient % 4)
										s_Player->horzSpd = MINI_STUN_HSPD;
									else
										s_Player->horzSpd = -MINI_STUN_HSPD;
									s_Player->actCount = MINI_STUN_FRAME;
									// Screen shake effect
									SetShake(HIT_SHAKE);
									break;

								case DOWN:
									if (!GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
									{
										// Reflect effect.
										SetEffect(s_Player->pos.x + cof * rcosrot, s_Player->pos.y + cof * rsinrot,
											PLAYER_REFLECT, s_Player->atkOrient);
										s_Player->vertSpd = HITJUMP_SPD;
										s_Player->state = BIG_JUMP;
										// Screen shake effect
										SetShake(HIT_SHAKE);
									}
									break;

								case UP:
									// Reflect effect.
									SetEffect(s_Player->pos.x + cof * rcosrot, s_Player->pos.y + cof * rsinrot,
										PLAYER_REFLECT, s_Player->atkOrient);
									s_Player->vertSpd = -HITJUMP_SPD / 2;
									s_Player->state = FALL;
									// Screen shake effect
									SetShake(HIT_SHAKE);
									break;

								default:
									break;
								}
								s_Player->atkDetect = TRUE;
								break;
							}
							cof += 0.5f;
						}
					}

				}

				// While attacking, dashCount degresses per frame until reaches 0
				if (!s_Player->actCount--)
				{
					s_Player->atkOrient = s_Player->orient;
					s_Player->atk = NULL;
					s_Player->state = FALL;
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
			s_Player->countAnim += 1.0f;
			if (s_Player->countAnim > ANIM_WAIT)
			{
				s_Player->countAnim = 0.0f;
				// パターンの切り替え
				s_Player->patternAnim = (s_Player->patternAnim + 1) % ANIM_PATTERN_NUM;
			}


#ifdef _DEBUG
			// デバッグ表示
			PrintDebugProc("Player X:%f Y:%f vS: %d BGd: %d Orient: %d State: %d \n", 
				s_Player->pos.x, 
				s_Player->pos.y, 
				s_Player->vertSpd, 
				GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2),
				s_Player->orient, 
				s_Player->state);
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

			//プレイヤーの位置やテクスチャー座標を反映
			float px = s_Player->pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = s_Player->pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = s_Player->w;		// プレイヤーの表示幅
			float ph = s_Player->h;		// プレイヤーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(s_Player->patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(s_Player->patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			//float tx = s_Player->patternAnim * tw;	// テクスチャの左上X座標
			//float ty = s_Player->patternAnim * th;	// テクスチャの左上Y座標
			float fa = 1.0f;
			if ((s_Player->godCount / GOD_FLASH) % 2) fa = 0.3f;
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, fa),
				s_Player->rot.z);

			// ポリゴン描画
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

			//プレイヤーの位置やテクスチャー座標を反映
			float px = s_Illusion->illusion.pos.x - bg->pos.x;	// プレイヤーの表示位置X
			float py = s_Illusion->illusion.pos.y - bg->pos.y;	// プレイヤーの表示位置Y
			float pw = s_Illusion->illusion.w;		// プレイヤーの表示幅
			float ph = s_Illusion->illusion.h;		// プレイヤーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(s_Illusion->illusion.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(s_Illusion->illusion.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			//float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			//float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			//float tx = s_Player->patternAnim * tw;	// テクスチャの左上X座標
			//float ty = s_Player->patternAnim * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f / ILLUSION_LIFE_SPAN * s_Illusion->life),
				s_Illusion->illusion.rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// プレイヤー構造体の先頭アドレスを取得
//=============================================================================
PLAYER *GetPlayer(void)
{

	return &g_Player[0];
}

void HitPlayer(PLAYER* player, int damge, int orient)
{
	if (!player->godCount)
	{
		player->state = STUN;
		player->actCount = HIT_STUN_FRAME;
		switch (orient)
		{
		case RIGHT:
			player->horzSpd = HIT_STUN_HSPD;
			break;

		case LEFT:
			player->horzSpd = -HIT_STUN_HSPD;
			break;

		case DOWN:
			player->vertSpd = HIT_STUN_VSPD;
			player->horzSpd = 0;

			break;

		case UP:
			player->vertSpd = -HIT_STUN_VSPD;
			player->horzSpd = 0;

			break;

		default:
			break;
		}
		SetShake(HITBYENEMY_SHAKE);
		player->godCount = GOD_FRAME;
	}
	return;
}




