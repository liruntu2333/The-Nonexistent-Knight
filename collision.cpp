#include "collision.h"

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

bool PEColiision(D3DXVECTOR3* ppos, D3DXVECTOR3* epos, float ew)
{
	if (ppos->x > epos->x && 
		ppos->x < epos->x + ew)
	{
		return true;
	}
	return false;
}
