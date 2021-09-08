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
#include "fade.h"

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

#define STAMINA_MAX					(50)
#define HEALTH_MAX					(7)

#define ATK_COST					(30)
#define DASH_COST					(20)
#define JUMP_COST					(10)
#define PARRY_COST					(15)

#define MINI_STUN_FRAME				(5)
#define MINI_STUN_HSPD				(8)
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

void CheckHitTerra(PLAYER* player, EFFECT* effect);		
void CheckHitEnemy(PLAYER* player, EFFECT* effect);
void GetTrigger(PLAYER* player);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = 
{
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
		s_Player->health = HEALTH_MAX;
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

			// Input that changes current state of player.
			GetTrigger(s_Player);

			switch (s_Player->state)
			{
			case RUN:
			{
				// Object is in the air, therefore should start to fall.
				if (!GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
				{
					// Start to fall when walk across the edge.
					s_Player->state = FALL;
				}
				break;
			}
			case STAND:
			{
				break;
			}
			case STAND_ELEV:
			{
				s_Player->pos.y += s_Player->elev->vertSpd;
				break;
			}
			case RUN_ELEV:
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
							s_Player->w, s_Elevator->w * 2, s_Player->h, s_Elevator->h))
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
						// Player can't attack downward when isn't in air(stand & attack).
						if (GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2))
						{
							s_Player->actCount = 0;
							s_Player->state = FALL;
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
				}

				if (s_Player->actCount == ATK_FRAME - ATK_DETECTION)
				{
					// blade effect bonds to start frame of detection

					s_Player->atk = SetEffect(s_Player->pos.x, s_Player->pos.y,
						PLAYER_BLADE, s_Player->atkOrient);
				}

				// Attack dectecting process, hit detection should do only once.
				if (!s_Player->atkDetect &&
					s_Player->actCount > ATK_DETECTION && s_Player->actCount <= ATK_END)
				{
					EFFECT* s_Effect = s_Player->atk;

					// Do enemy detection first.
					CheckHitEnemy(s_Player, s_Effect);

					// Do enviroment detection next.
					CheckHitTerra(s_Player, s_Effect);
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
			case SLASH:
			{
				// Attack process.
				if (s_Player->actCount == SLASH_FRAME)
				{
					// Super slash after sucessfully parried.
					// Return to 60 FPS
					SetSlowMotion(1);
					// Set next hit to normal attack.
					s_Player->pryDetect = FALSE;
					s_Player->atkOrient = s_Player->orient;
					// slash effect
					s_Player->atk = SetEffect(s_Player->pos.x, s_Player->pos.y,
						PLAYER_SLASH, s_Player->atkOrient);

					s_Player->vertSpd = 0;
					s_Player->atkDetect = FALSE;
				}

				// Slash dectecting process, hit detection has no limit.
				if (s_Player->actCount > SLASH_DETECTION && s_Player->actCount <= SLASH_END)
				{
					EFFECT* s_Effect = s_Player->atk;

					// Do enemy detection only.
					for (int i = 0; i < ENEMY_MAX; i++)
					{
						ENEMY* s_Enemy = GetEnemy() + i;
						if (BBCollision(&s_Effect->pos, &s_Enemy->pos,
							s_Effect->w, s_Enemy->w, s_Effect->h, s_Enemy->h) &&
							s_Enemy->slashed == FALSE) // Bullseye
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
					s_Player->atk = NULL;
					s_Player->state = FALL;
				}
				break;
			}
			case PARRY:
			{
				// Parry process.

				// While parrying, actCount degresses per frame until reaches 0
				if (!--s_Player->actCount)
				{
					s_Player->vertSpd = 0;
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
			case DEAD:
			{
				// Death process.
				if (!s_Player->actCount--)
				{
					SetFade(FADE_OUT, MODE_GAME);
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
			PrintDebugProc("Player X:%f Y:%f vS: %d BGd: %d Health: %d State: %d \n", 
				s_Player->pos.x, 
				s_Player->pos.y, 
				s_Player->vertSpd, 
				GetTerrain(s_Player->pos.x, s_Player->pos.y + s_Player->h / 2),
				s_Player->health, 
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
		player->state = STUN;
		player->actCount = BIG_STUN_FRAME;
		player->health -= damge;
		if (player->health <= 0)
		{
			player->health = 0;
			player->state = DEAD;
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
				// Varies from direction to direction, step back, jump.
				switch (player->atkOrient)
				{
				case RIGHT:
				case LEFT:
					// Reflect effect.
					SetEffect(player->pos.x + cof * rcosrot, player->pos.y + cof * rsinrot,
						PLAYER_REFLECT, player->atkOrient);
					// stop playing Blade effect cause hit on the wall
					player->atk->use = FALSE;
					player->atk = NULL;

					player->state = STUN;
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
		if (s_Enemy->use && 
			BBCollision(&effect->pos, &s_Enemy->pos, effect->w, s_Enemy->w, effect->h, s_Enemy->h)) // Bullseye
		{
			// Hit effect.
			SetEffect(player->pos.x, player->pos.y,
				PLAYER_HIT, player->atkOrient);
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
				if (player->atkOrient / 2) player->horzSpd = MINI_STUN_HSPD;
				else player->horzSpd = -MINI_STUN_HSPD;
				player->actCount = MINI_STUN_FRAME;
				break;

			case DOWN:
				player->vertSpd = HITJUMP_SPD;
				player->state = BIG_JUMP;
				break;

			case UP:
				player->vertSpd = 10;
				player->state = FALL;
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
		if (player->stamina < STAMINA_MAX)
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
			}
			// Player's state should be RUN / RUN ON ELEV
			if (player->state == STAND || player->state == STAND_ELEV)
			{
				player->state += RUN;
			}
		}
		else if (GetKeyboardPress(DIK_A))		// Move right overrides move left.
		{
			player->orient = LEFT;
			if (!GetTerrain(player->pos.x - player->w / 2, player->pos.y))
			{
				player->pos.x -= RUN_SPEED;
			}
			// Player's state should be RUN / RUN ON ELEV
			if (player->state == STAND || player->state == STAND_ELEV)
			{
				player->state += RUN;
			}
		}
		else
		{
			// No instruction coming from keyboard, player does nothing but stand.
			if (player->state == RUN || player->state == RUN_ELEV)
			{
				player->state -= RUN;
			}
		}

		// Dash trigger.
		if (GetKeyboardTrigger(DIK_LSHIFT))
		{
			if (player->stamina >= DASH_COST)
			{
				player->elev = NULL;
				player->stamina -= DASH_COST;
				player->state = DASH;
				player->actCount = DASH_FRAME;
			}
		}
		// Attack trigger.
		else if (GetKeyboardTrigger(DIK_J))
		{
			if (player->stamina >= ATK_COST)
			{
				player->elev = NULL;
				player->stamina -= ATK_COST;


				// Super slash after successfully parried
				if (player->pryDetect)
				{
					player->state = SLASH;
					player->actCount = SLASH_FRAME;
				}
				// Normal attack.
				else
				{
					player->state = ATTACK;
					player->actCount = ATK_FRAME;
				}
			}
		}
		// Parry trigger.
		else if (GetKeyboardTrigger(DIK_K))
		{
			if (player->stamina >= PARRY_COST)
			{
				player->elev = NULL;
				player->stamina -= PARRY_COST;
				player->state = PARRY;
				player->actCount = PARRY_FRAME;
			}
		}
		// Jump only triggers when player's on the ground(STAND ~ STAND_ELEV).
		else if (player->state < DASH && GetKeyboardTrigger(DIK_SPACE))
		{
			if (player->stamina >= JUMP_COST)
			{
				player->stamina -= JUMP_COST;
				player->state = JUMP;
				player->vertSpd = JUMP_SPEED;
			}
		}
		// Big Jump trigger
		if (player->state == JUMP && GetKeyboardPress(DIK_SPACE) &&
			player->vertSpd <= JUMP_SPEED * 1 / 2 && player->vertSpd >= JUMP_SPEED * 4 / 7)
		{
			player->state = BIG_JUMP;
			player->vertSpd += JUMP_SPEED / 3;
		}
	}
}
