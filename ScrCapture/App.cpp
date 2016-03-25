// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ScrCaptureWnd.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CScrCaptureWnd* pFrame = CScrCaptureWnd::Instance();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T(""), WS_VISIBLE | WS_POPUP, /*WS_EX_TOOLWINDOW|WS_EX_TOPMOST*/0);
	pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
