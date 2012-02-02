#include "stdafx.h"
#include "CAppletManager.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	CAppletManager* manager = new CAppletManager();
	manager->Initialize(_T("HeliumG15"),true,true);
	
	MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
	while( msg.message!=WM_QUIT )
    {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
			TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		manager->Update();
		Sleep(1);
	}
}