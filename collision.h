#pragma once
#include "main.h"

bool BBCollision(D3DXVECTOR3* lpos, D3DXVECTOR3* rpos, float lw, float rw, float lh, float rh);
bool BCCollision(D3DXVECTOR3* lpos, D3DXVECTOR3* rpos, float lr, float rr);
bool PEColiision(D3DXVECTOR3* ppos, D3DXVECTOR3* epos, float ew);