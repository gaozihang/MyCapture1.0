#include "D3DCaptureClass.h"

D3DCaptureClass::D3DCaptureClass()
{
	pd3dDevice    = NULL;
	pd3dTexture   = NULL;
	pMouse        = NULL;
	pd3dFont      = NULL;
	ButtonStatus  = 0;
	MouseStatus   = 0;
	MouseStatus_L = 0;
	FrameCnt      = 0;
	CurrentTime   = 0;
	LastTime      = 0;
	FPS           = 0;
	StrFPS        = NULL;
	MouseStr      = NULL;
	AdapterName   = NULL;
	ZeroMemory(&Rect, sizeof(Rect));
	ZeroMemory(&LastMouse, sizeof(LastMouse));
	ZeroMemory(&CurrentMouse, sizeof(CurrentMouse));
}

D3DCaptureClass::~D3DCaptureClass()
{
	SAFE_RELEASE(pd3dDevice);
	SAFE_RELEASE(pd3dTexture);
	pMouse->Unacquire();
	SAFE_RELEASE(pMouse);
	delete [] StrFPS;
	delete [] MouseStr;
	delete [] RectStr;
	delete [] AdapterName;
	
}

bool D3DCaptureClass::D3DCaptureClassInit(HWND _hwnd, HINSTANCE _hinstance)
{
	hwnd        = _hwnd;
	hInstance   = _hinstance;
	StrFPS      = new wchar_t [30];
	MouseStr    = new wchar_t[30];
	RectStr     = new wchar_t[200];
	AdapterName = new wchar_t [30];
	
	ButtonCoordinate.left   = 0;
	ButtonCoordinate.right  = ButtonCoordinate.left + BUTTON_WIDTH;
	ButtonCoordinate.top    = 0;
	ButtonCoordinate.bottom = ButtonCoordinate.top + BUTTON_HEIGHT;

    // D3D Init /////////////////////////////////////////////////////////////////////

	LPDIRECT3D9 pD3D = NULL;
	if( NULL == ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) 
		return false;

	D3DCAPS9 caps; int vp = 0;
	if( FAILED( pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ) )
	{
		return false;
	}
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = WINDOW_WIDTH;
	d3dpp.BackBufferHeight           = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	D3DADAPTER_IDENTIFIER9 pId;

	pD3D->GetAdapterIdentifier(0, 0, &pId);

	MultiByteToWideChar(CP_ACP, 0, pId.Description, strlen(pId.Description)+1, AdapterName,
							sizeof(wchar_t) * 64 / sizeof(StrFPS[0]));   

	if(FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		hwnd, vp, &d3dpp, &pd3dDevice)))
		return false;

	SAFE_RELEASE(pD3D);

	pd3dDevice->SetSamplerState(0,D3DSAMP_MAXANISOTROPY, caps.MaxAnisotropy);

	// Mouse Init /////////////////////////////////////////////////////////////////////

	LPDIRECTINPUT8 pDirectInput = NULL;

	if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, 
								 (void**)&pDirectInput, NULL)))
		return false;

	pDirectInput->CreateDevice(GUID_SysMouse, &pMouse, NULL);
	pMouse->SetDataFormat(&c_dfDIMouse);
	pMouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	pMouse->Poll();
	pMouse->Acquire();

	ClientToScreen(hwnd, &pt);
	SetCursorPos(pt.x, pt.y);
//	SetCursorPos(InitWindowPt.x, InitWindowPt.y);

	SAFE_RELEASE(pDirectInput);

	// Vertex Create /////////////////////////////////////////////////////////////////////

	if(FAILED(pd3dDevice->CreateVertexBuffer(12 * sizeof(CUSTOMVERTEX), NULL, D3DFVF_CUSTOMVERTEX, 
								   D3DPOOL_MANAGED, &pd3dVertexBuffer, NULL)))
		return false;

	CUSTOMVERTEX * ppData;

	float offset  = 0.15;

	pd3dVertexBuffer->Lock(0, 0, (void**)&ppData, NULL);

	ppData[0]	= CUSTOMVERTEX(0.0f, (float)BUTTON_HEIGHT, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 0.0f, 1.0f - offset);
	ppData[1]	= CUSTOMVERTEX(0.0f, 0.0f, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 0.0f, 0.666f);
	ppData[2]	= CUSTOMVERTEX((float)BUTTON_WIDTH, (float)BUTTON_HEIGHT, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 1.0f, 1.0f - offset);
	ppData[3]	= CUSTOMVERTEX((float)BUTTON_WIDTH, 0.0f, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 1.0f, 0.666f);
	ppData[4]	= CUSTOMVERTEX(0.0f, (float)BUTTON_HEIGHT, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 0.0f, 0.666f - offset);

	ppData[5]	= CUSTOMVERTEX(0.0f, 0.0f, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 0.0f, 0.333f);
	ppData[6]	= CUSTOMVERTEX((float)BUTTON_WIDTH, (float)BUTTON_HEIGHT, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 1.0f, 0.666f - offset);
	ppData[7]	= CUSTOMVERTEX((float)BUTTON_WIDTH, 0.0f, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 1.0f, 0.333f);

	ppData[8]	= CUSTOMVERTEX(0.0f, (float)BUTTON_HEIGHT, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 0.0f, 0.333f - offset);
	ppData[9]	= CUSTOMVERTEX(0.0f, 0.0f, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 0.0f, 0.0f);
	ppData[10]	= CUSTOMVERTEX((float)BUTTON_WIDTH, (float)BUTTON_HEIGHT, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 1.0f, 0.333f - offset);
	ppData[11]	= CUSTOMVERTEX((float)BUTTON_WIDTH, 0.0f, 0.0f, 1.0f,
				  D3DCOLOR_XRGB(255,255,255), 1.0f, 0.0f );

	pd3dVertexBuffer->Unlock();

	// Create Font /////////////////////////////////////////////////////////////////////

	if(FAILED(D3DXCreateFont( pd3dDevice, 0, 0, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				DEFAULT_QUALITY, 0, L"Calibri" , &pd3dFont )))
		return false;

	// Load Texture /////////////////////////////////////////////////////////////////////

	if(FAILED(D3DXCreateTextureFromFile(pd3dDevice, _T("Texture.png"), &pd3dTexture)))
		return false;
	
	pd3dDevice->SetTexture(0, pd3dTexture);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);

	return true;
}

void D3DCaptureClass::MeasureCoordinate()
{
	pMouse->GetDeviceState(sizeof(CurrentMouse), (LPVOID)&CurrentMouse);


/*
*   通过DIRECTINPUTDEVICE8计算鼠标偏移量,发现误差极大,后转用Win32 API 
*
*	LastMouse.lX += CurrentMouse.lX;
*	LastMouse.lY += CurrentMouse.lY;
*/
	GetCursorPos(&pt);

	ScreenToClient(hwnd, &pt);

	char Tmp[200];
//	sprintf(Tmp, "X: %d  Y: %d", LastMouse.lX, LastMouse.lY);
	sprintf(Tmp, "MouseX: %d  MouseY: %d", pt.x, pt.y);

	MultiByteToWideChar(CP_ACP, 0, Tmp, strlen(Tmp)+1, MouseStr,
	sizeof(wchar_t) * 64 / sizeof(MouseStr[0]));   

	MouseStatus_L = MouseStatus;
	if(CurrentMouse.rgbButtons[0] & 0x80)
		MouseStatus = 1;
	else
		MouseStatus = 0;
/*
*	if(LastMouse.lX >= ButtonCoordinate.left && LastMouse.lX <= ButtonCoordinate.right &&
*	   LastMouse.lY >= ButtonCoordinate.top && LastMouse.lY <= ButtonCoordinate.bottom)
*/
	if(pt.x >= ButtonCoordinate.left && pt.x <= ButtonCoordinate.right &&
	   pt.y >= ButtonCoordinate.top && pt.y <= ButtonCoordinate.bottom)
	{
		if(MouseStatus_L == 0 && MouseStatus == 1)
		{
			if(ButtonStatus == 0)
				ButtonStatus = 1;
			else
				ButtonStatus = 0;
		}
	}

	ClientToScreen(hwnd, &pt);

	if(ButtonStatus == 1)
	{
/*
*		if(MouseStatus_L == 0 && MouseStatus == 1)
*		{
*			Rect.left   = LastMouse.lX;
*			Rect.right  = LastMouse.lX;
*			Rect.top    = LastMouse.lY;
*			Rect.bottom = LastMouse.lY;
*		}
*		else if(MouseStatus_L == 1 && MouseStatus == 1)
*		{
*			Rect.left   = Min(Rect.left,   LastMouse.lX);
*			Rect.right  = Max(Rect.right,  LastMouse.lX);
*			Rect.top    = Min(Rect.top,	   LastMouse.lY);
*			Rect.bottom = Max(Rect.bottom, LastMouse.lY);
*		}
*/
		if(MouseStatus_L == 0 && MouseStatus == 1)
		{
			Rect.left   = pt.x;
			Rect.right  = pt.x;
			Rect.top    = pt.y;
			Rect.bottom = pt.y;
		}
		else if(MouseStatus_L == 1 && MouseStatus == 1)
		{
			Rect.left   = Min(Rect.left,   pt.x);
			Rect.right  = Max(Rect.right,  pt.x);
			Rect.top    = Min(Rect.top,	   pt.y);
			Rect.bottom = Max(Rect.bottom, pt.y);
		}
	}
	sprintf(Tmp, "Rect.Top :%d\nRect.left :%d\nRect.bottom :%d\nRect.right :%d",
		    Rect.top, Rect.left, Rect.bottom, Rect.right);
	MultiByteToWideChar(CP_ACP, 0, Tmp, strlen(Tmp)+1, RectStr,
	sizeof(wchar_t) * 64 / sizeof(RectStr[0]));

}

bool D3DCaptureClass::CaptureForFront()
{
	if(ButtonStatus == 0)
		return true;

	D3DDISPLAYMODE mode;

	LPDIRECT3DSURFACE9 front, back;

	if(FAILED(pd3dDevice->GetDisplayMode(0, &mode)))
		return false;

	if(FAILED(pd3dDevice->CreateOffscreenPlainSurface(mode.Width,mode.Height,
		D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &front, NULL)))
		return false;  
	
	if(FAILED(pd3dDevice->GetFrontBufferData(0, front)))
		return false;

	if(FAILED(D3DXSaveSurfaceToFile(_T("tmp.bmp"), D3DXIFF_BMP, 
						front, NULL, &Rect)))
		return false;

	if(FAILED(pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back)))
		return false;

	RECT TmpRect;
	TmpRect.left   = Rect.left;
	TmpRect.top    = Rect.top;
	TmpRect.right  = Min(Rect.right, Rect.left + WINDOW_WIDTH);
	TmpRect.bottom = Min(Rect.bottom, TmpRect.top + WINDOW_HEIGHT);

	POINT p;
	p.x = p.y = 0;

	if(FAILED(pd3dDevice->UpdateSurface(front, &TmpRect, back, &p)))
		return false;

	SAFE_RELEASE(front);
	SAFE_RELEASE(back);

	return true;
}

/*
*	由BackBuffer得到截屏数据,可用于窗口截屏,未完善
*/

bool D3DCaptureClass::CaptureForBack()
{
	if(ButtonStatus != 2)
		return false;

	D3DDISPLAYMODE mode;

	if(FAILED(pd3dDevice->GetDisplayMode(0, &mode)))
		return false; 
	
	LPDIRECT3DSURFACE9 back;

	if(FAILED(pd3dDevice->CreateOffscreenPlainSurface(WINDOW_WIDTH,WINDOW_HEIGHT,
		D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &back, NULL)))
		return false;

	if(FAILED(pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back)))
		return false;

	if(FAILED(D3DXSaveSurfaceToFile(_T("tmp"), D3DXIFF_BMP, 
						back, NULL, &Rect)))
		return false;

	SAFE_RELEASE(back);
	
	return true;
}

void D3DCaptureClass::CalcFPS()
{
	FrameCnt++;

	CurrentTime = timeGetTime();

	if(CurrentTime - LastTime >= 1000)
	{
		FPS = (float) FrameCnt * 1000 /  (CurrentTime - LastTime);
		FrameCnt = 0;
		LastTime = CurrentTime;
		char Tmp[20];
		sprintf(Tmp, "FPS: %0.3f", FPS);
		MultiByteToWideChar(CP_ACP, 0, Tmp, strlen(Tmp)+1, StrFPS,
							sizeof(wchar_t) * 64 / sizeof(StrFPS[0]));   
	}
}

void D3DCaptureClass::Render()
{
	CalcFPS();

	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
					  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	pd3dDevice->BeginScene();

	MeasureCoordinate();

	RECT rect;

	GetWindowRect(hwnd, &rect); 

	if(ButtonStatus == 1)
		SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top, NULL);
	else
		SetWindowPos(hwnd, HWND_NOTOPMOST, rect.left, rect.top, rect.right - rect.left,
		rect.bottom - rect.top, NULL);

	CaptureForFront();

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	pd3dDevice->SetStreamSource(0, pd3dVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (ButtonStatus + 1) * 4, 2);

	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	rect.left   = WINDOW_HEIGHT / 4 * 1 + 30;
	rect.top	= 0;
	rect.right  = WINDOW_WIDTH;
	rect.bottom = WINDOW_HEIGHT;

	pd3dFont->DrawText(0, MouseStr, -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(238, 249, 43));

	rect.left   = WINDOW_HEIGHT / 4 * 3 + 30;
	pd3dFont->DrawText(0, AdapterName, -1, &rect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(238, 249, 43));

	pd3dFont->DrawText(0, StrFPS, -1, &rect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(87, 170, 244));

	rect.left   = WINDOW_HEIGHT / 2 - 50;
	rect.top	= 50;
	rect.right  = WINDOW_WIDTH;
	rect.bottom = WINDOW_HEIGHT;

	pd3dFont->DrawText(0, RectStr, -1, &rect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(87, 170, 244));

	pd3dDevice->EndScene();
	pd3dDevice->Present(NULL, NULL, NULL, NULL);
}