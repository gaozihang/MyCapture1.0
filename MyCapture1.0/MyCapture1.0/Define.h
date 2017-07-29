#ifndef D3DUTIL
#define D3D_DEFINE

#define DIRECTINPUT_VERSION 0X800
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <Dinput.h>
#include <cstdio>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#define WINDOW_WIDTH	600					
#define WINDOW_HEIGHT	400	
#define WINDOW_TITLE	_T("Capture.Demo")
#define BUTTON_WIDTH	60				
#define BUTTON_HEIGHT	40
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define Max(a,b) (a>b?a:b)
#define Min(a,b) (a>b?b:a)

const POINT InitWindowPt= {700, 100};

struct CUSTOMVERTEX 
{
	FLOAT x, y, z, rhw;
	unsigned long color;
	FLOAT u, v;
	CUSTOMVERTEX(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _rhw, unsigned long _color, FLOAT _u, FLOAT _v)
		: x(_x), y(_y), z(_z),rhw(_rhw), color(_color), u(_u), v(_v) {}
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#endif