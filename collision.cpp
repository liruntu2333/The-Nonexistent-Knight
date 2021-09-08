#include "collision.h"
#include "map.h"

bool BBCollision(D3DXVECTOR3* lpos, D3DXVECTOR3* rpos, float lw, float rw, float lh, float rh)
{
	if (lpos->x > rpos->x - rw / 2 - lw / 2 &&
		lpos->x < rpos->x + rw / 2 + lw / 2 &&
		lpos->y > rpos->y - rh / 2 - lh / 2 &&
		lpos->y < rpos->y + rh / 2 + lh / 2)
	{
		return true;
	}
	return false;
}

bool BCCollision(D3DXVECTOR3* lpos, D3DXVECTOR3* rpos, float lr, float rr)
{
	D3DXVECTOR3 diff = *lpos - *rpos;
	
	//TODO: complete BC
	return false;
}

bool PECollision(D3DXVECTOR3* ppos, D3DXVECTOR3* epos, float pw, float ew, float ph, float eh)
{
	if (ppos->x > epos->x && 
		ppos->x < epos->x + ew &&
		ppos->y > epos->y - ph / 2 - BLK_LGTH &&
		ppos->y < epos->y + eh)
	{
		return true;
	}
	return false;
}
