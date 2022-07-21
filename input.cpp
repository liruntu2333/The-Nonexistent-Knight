#include "input.h"
#include "debugproc.h"

#define	NUM_KEY_MAX			(256)

#define DEADZONE		2500			 
#define RANGE_MAX		1000			 
#define RANGE_MIN		-1000			 

HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
void UninitKeyboard(void);
HRESULT UpdateKeyboard(void);

HRESULT InitializeMouse(HINSTANCE hInst, HWND hWindow);  
void UninitMouse();						 
HRESULT UpdateMouse();					 

HRESULT InitializePad(void);			 
void UpdatePad(void);
void UninitPad(void);

LPDIRECTINPUT8			g_pDInput = nullptr;					 
LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = nullptr;			 
BYTE					g_keyState[NUM_KEY_MAX];			 
BYTE					g_keyStateTrigger[NUM_KEY_MAX];		 
BYTE					g_keyStateRepeat[NUM_KEY_MAX];		 
BYTE					g_keyStateRelease[NUM_KEY_MAX];		 
int						g_keyStateRepeatCnt[NUM_KEY_MAX];	 

static LPDIRECTINPUTDEVICE8 pMouse = nullptr;  

static DIMOUSESTATE2   mouseState;		 
static DIMOUSESTATE2   mouseTrigger;	 

static LPDIRECTINPUTDEVICE8	pGamePad[GAMEPADMAX] = {nullptr, nullptr, nullptr, nullptr}; 

static DWORD	padState[GAMEPADMAX];	 
static DWORD	padTrigger[GAMEPADMAX];
static int		padCount = 0;			 

HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if (!g_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&g_pDInput, nullptr);
	}

	InitKeyboard(hInst, hWnd);

	InitializeMouse(hInst, hWnd);

	InitializePad();

	return S_OK;
}

void UninitInput(void)
{
	UninitKeyboard();

	UninitMouse();

	UninitPad();

	if (g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = nullptr;
	}
}

void UpdateInput(void)
{
	UpdateKeyboard();

	UpdateMouse();

	UpdatePad();
}

HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, nullptr);
	if (FAILED(hr) || g_pDIDevKeyboard == nullptr)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

void UninitKeyboard(void)
{
	if (g_pDIDevKeyboard)
	{
		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard = nullptr;
	}
}

HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];

	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if (SUCCEEDED(hr))
	{
		for (int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			g_keyStateTrigger[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & g_keyState[cnt];
			g_keyStateRelease[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & ~g_keyState[cnt];
			g_keyStateRepeat[cnt] = g_keyStateTrigger[cnt];

			if (g_keyState[cnt])
			{
				g_keyStateRepeatCnt[cnt]++;
				if (g_keyStateRepeatCnt[cnt] >= 20)
				{
					g_keyStateRepeat[cnt] = g_keyState[cnt];
				}
			}
			else
			{
				g_keyStateRepeatCnt[cnt] = 0;
				g_keyStateRepeat[cnt] = 0;
			}
		}
	}
	else
	{
		g_pDIDevKeyboard->Acquire();
	}

	return S_OK;
}

bool GetKeyboardPress(int key)
{
	return (g_keyState[key] & 0x80) ? true : false;
}

bool GetKeyboardTrigger(int key)
{
	return (g_keyStateTrigger[key] & 0x80) ? true : false;
}

bool GetKeyboardRepeat(int key)
{
	return (g_keyStateRepeat[key] & 0x80) ? true : false;
}

bool GetKeyboardRelease(int key)
{
	return (g_keyStateRelease[key] & 0x80) ? true : false;
}

HRESULT InitializeMouse(HINSTANCE hInst, HWND hWindow)
{
	HRESULT result;
	result = g_pDInput->CreateDevice(GUID_SysMouse, &pMouse, nullptr);
	if (FAILED(result) || pMouse == nullptr)
	{
		MessageBox(hWindow, "No mouse", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}
	result = pMouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		MessageBox(hWindow, "Can't setup mouse", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}
	result = pMouse->SetCooperativeLevel(hWindow, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(result))
	{
		MessageBox(hWindow, "Mouse mode error", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}

	DIPROPDWORD prop;

	prop.diph.dwSize = sizeof(prop);
	prop.diph.dwHeaderSize = sizeof(prop.diph);
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = DIPROPAXISMODE_REL;

	result = pMouse->SetProperty(DIPROP_AXISMODE, &prop.diph);
	if (FAILED(result))
	{
		MessageBox(hWindow, "Mouse property error", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}

	pMouse->Acquire();
	return result;
}
void UninitMouse()
{
	if (pMouse)
	{
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = nullptr;
	}
}
HRESULT UpdateMouse()
{
	HRESULT result;
	DIMOUSESTATE2 lastMouseState = mouseState;
	result = pMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	if (SUCCEEDED(result))
	{
		mouseTrigger.lX = mouseState.lX;
		mouseTrigger.lY = mouseState.lY;
		mouseTrigger.lZ = mouseState.lZ;
		for (int i = 0; i < 8; i++)
		{
			mouseTrigger.rgbButtons[i] = ((lastMouseState.rgbButtons[i] ^
				mouseState.rgbButtons[i]) & mouseState.rgbButtons[i]);
		}
	}
	else	 
	{
		result = pMouse->Acquire();
	}
	return result;
}

BOOL IsMouseLeftPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[0] & 0x80);	 
}
BOOL IsMouseLeftTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[0] & 0x80);
}
BOOL IsMouseRightPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[1] & 0x80);
}
BOOL IsMouseRightTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[1] & 0x80);
}
BOOL IsMouseCenterPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[2] & 0x80);
}
BOOL IsMouseCenterTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[2] & 0x80);
}
long GetMouseX(void)
{
	return GetMousePosX();
}
long GetMouseY(void)
{
	return GetMousePosY();
}
long GetMouseZ(void)
{
	return mouseState.lZ;
}
BOOL CALLBACK SearchGamePadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID)
{
	HRESULT result;

	result = g_pDInput->CreateDevice(lpddi->guidInstance, &pGamePad[padCount++], nullptr);
	return DIENUM_CONTINUE;	 
}
HRESULT InitializePad(void)			 
{
	HRESULT		result;
	int			i;

	padCount = 0;
	g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, nullptr, DIEDFL_ATTACHEDONLY);
	for (i = 0; i < padCount; i++) {
		result = pGamePad[i]->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
			return false;  

		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = RANGE_MIN;
		diprg.lMax = RANGE_MAX;
		diprg.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		diprg.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = DEADZONE;
		dipdw.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		dipdw.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		pGamePad[i]->Acquire();
	}

	return true;
}
void UninitPad(void)
{
	for (int i = 0; i < GAMEPADMAX; i++) {
		if (pGamePad[i])
		{
			pGamePad[i]->Unacquire();
			pGamePad[i]->Release();
		}
	}
}

float GY, GX;

void UpdatePad(void)
{
	HRESULT			result;
	DIJOYSTATE2		dijs;
	int				i;

	for (i = 0; i < padCount; i++)
	{
		DWORD lastPadState;
		lastPadState = padState[i];
		padState[i] = 0x00000000l;	 

		result = pGamePad[i]->Poll();	 
		if (FAILED(result)) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		result = pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE), &dijs);	 
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		if (dijs.lY < 0)					padState[i] |= BUTTON_UP;
		if (dijs.lY > 0)					padState[i] |= BUTTON_DOWN;
		if (dijs.lX < 0)					padState[i] |= BUTTON_LEFT;
		if (dijs.lX > 0)					padState[i] |= BUTTON_RIGHT;
		if (dijs.rgbButtons[rgbButtons_X] & 0x80)	padState[i] |= BUTTON_X;
		if (dijs.rgbButtons[rgbButtons_A] & 0x80)	padState[i] |= BUTTON_A;
		if (dijs.rgbButtons[rgbButtons_B] & 0x80)	padState[i] |= BUTTON_B;
		if (dijs.rgbButtons[rgbButtons_Y] & 0x80)	padState[i] |= BUTTON_Y;
		if (dijs.rgbButtons[rgbButtons_L] & 0x80)	padState[i] |= BUTTON_L;
		if (dijs.rgbButtons[rgbButtons_R] & 0x80)	padState[i] |= BUTTON_R;
		if (dijs.rgbButtons[rgbButtons_L2] & 0x80)	padState[i] |= BUTTON_L2;
		if (dijs.rgbButtons[rgbButtons_R2] & 0x80)	padState[i] |= BUTTON_R2;
		if (dijs.rgbButtons[rgbButtons_SELECT] & 0x80)	padState[i] |= BUTTON_SELECT;
		if (dijs.rgbButtons[rgbButtons_START] & 0x80)	padState[i] |= BUTTON_START;

		padTrigger[i] = ((lastPadState ^ padState[i])	 
			& padState[i]);					 
	}
}
BOOL IsButtonPressed(int padNo, DWORD button)
{
	return (button & padState[padNo]);
}

BOOL IsButtonTriggered(int padNo, DWORD button)
{
	return (button & padTrigger[padNo]);
}