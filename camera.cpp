#include "main.h"
#include "renderer.h"
#include "camera.h"

D3DXMATRIX				g_CameraViewMatrix;
D3DXMATRIX				g_CameraInvViewMatrix;
D3DXMATRIX				g_CameraProjectionMatrix;

D3DXVECTOR3				g_CameraPosition;
D3DXVECTOR3				g_CameraTarget;

D3DXVECTOR3				g_CameraRotation;

D3DXMATRIX GetCameraViewMatrix()
{
	return g_CameraViewMatrix;
}

D3DXMATRIX GetCameraInvViewMatrix()
{
	return g_CameraInvViewMatrix;
}

D3DXMATRIX GetCameraProjectionMatrix()
{
	return g_CameraProjectionMatrix;
}

D3DXVECTOR3 GetCameraPosition()
{
	return g_CameraPosition;
}

void InitCamera(void)
{
	g_CameraPosition = D3DXVECTOR3(0.0f, 50.0f, -100.0f);
	g_CameraTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_CameraRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void UninitCamera(void)
{
}

void UpdateCamera(void)
{
	D3DXVECTOR3 playerPosition;

	playerPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	 

	g_CameraTarget.x = playerPosition.x * 0.5f;
	g_CameraTarget.z = playerPosition.z * 0.5f;

	g_CameraPosition.x = g_CameraTarget.x + sinf(g_CameraRotation.y) * 100.0f;
	g_CameraPosition.z = g_CameraTarget.z - cosf(g_CameraRotation.y) * 100.0f;
}

void SetCamera(void)
{
	D3DXMatrixLookAtLH(&g_CameraViewMatrix, &g_CameraPosition, &g_CameraTarget, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	SetViewMatrix(&g_CameraViewMatrix);

	float det;
	D3DXMatrixInverse(&g_CameraInvViewMatrix, &det, &g_CameraViewMatrix);

	D3DXMatrixPerspectiveFovLH(&g_CameraProjectionMatrix, 1.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);

	SetProjectionMatrix(&g_CameraProjectionMatrix);
}