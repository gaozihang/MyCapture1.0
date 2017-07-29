#include "Define.h"

class D3DCaptureClass
{
public:
	D3DCaptureClass();
	~D3DCaptureClass();
	bool D3DCaptureClassInit(HWND _hwnd, HINSTANCE _hinstance);
	void MeasureCoordinate();
	bool CaptureForFront();
	bool CaptureForBack();
	void CalcFPS();
	void Render();
private:
	LPDIRECT3DDEVICE9 pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 pd3dVertexBuffer;
	LPDIRECT3DTEXTURE9 pd3dTexture;
	LPDIRECTINPUTDEVICE8 pMouse;
	LPD3DXFONT pd3dFont;
	LPDIRECT3DSURFACE9 Back;
	HWND hwnd;
	HINSTANCE hInstance;
	RECT Rect;
	RECT ButtonCoordinate;
	DIMOUSESTATE LastMouse, CurrentMouse;
	int ButtonStatus;
	int MouseStatus;
	int MouseStatus_L;
	DWORD FrameCnt;
	DWORD CurrentTime, LastTime;
	float FPS;
	wchar_t * StrFPS;
	wchar_t * MouseStr;
	wchar_t * RectStr;
	wchar_t * AdapterName;
	POINT pt;
};