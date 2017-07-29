/*----------------------------------------------------------- 
   file:    MyCapture1.0.cpp
   author:  gaozihang2007@gmail.com
   date:    2014-8-15 
------------------------------------------------------------*/ 

#include "D3DCaptureClass.h"

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

D3DCaptureClass g_d3dCapture;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };						
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;			
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;					
	wndClass.cbClsExtra		= 0;							
	wndClass.cbWndExtra		= 0;							
	wndClass.hInstance = hInstance;						
	wndClass.hIcon=(HICON)::LoadImage(NULL,L"icon.ico",IMAGE_ICON,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE);  
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );    
	wndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);  
	wndClass.lpszMenuName = NULL;						
	wndClass.lpszClassName = L"Capture.Demo";		

	if( !RegisterClassEx( &wndClass ) )			
		return -1;		

	HWND hwnd = CreateWindow( _T("Capture.Demo"),WINDOW_TITLE,				
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT,
		CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL );

	MoveWindow(hwnd, InitWindowPt.x, InitWindowPt.y, WINDOW_WIDTH, WINDOW_HEIGHT, true);

	if(!g_d3dCapture.D3DCaptureClassInit(hwnd, hInstance))
	{
		MessageBox(hwnd, _T("D3DCaptureClassInit Error"), _T("msg"), 0);
		exit(0);
	}

	ShowWindow( hwnd, nShowCmd );
	UpdateWindow(hwnd);
	
	MSG msg = { 0 }; 
	while( msg.message != WM_QUIT )		
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )  
		{
			TranslateMessage( &msg );		
			DispatchMessage( &msg );		
		}
		else
		{
			g_d3dCapture.Render();
		}
	}

	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance); 

	return 0;   
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )    
{
	switch( message )			
	{
	case WM_PAINT:    
		ValidateRect(hwnd, NULL);
		break;								

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;								

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;						

	default:
		return DefWindowProc( hwnd, message, wParam, lParam );
	}

	return 0;
}