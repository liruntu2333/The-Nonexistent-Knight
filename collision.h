#pragma once
#include "main.h"

bool BBCollision(D3DXVECTOR3* lpos, D3DXVECTOR3* rpos, float lw, float rw, float lh, float rh);
bool BCCollision(D3DXVECTOR3* lpos, D3DXVECTOR3* rpos, float lr, float rr);
bool PECollision(D3DXVECTOR3* ppos, D3DXVECTOR3* epos, float pw, float ew, float ph, float eh);