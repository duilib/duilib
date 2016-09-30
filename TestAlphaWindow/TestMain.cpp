#include "StdAfx.h"
#include "MainWnd.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("Res"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CMainWnd theMainWnd;
	theMainWnd.Create(NULL,_T(""),UI_WNDSTYLE_DIALOG, 0L, 0, 0, 0,0,NULL);		//UI_WNDSTYLE_FRAME居然有窗口最小大小限制？
	theMainWnd.CenterWindow();
	theMainWnd.ShowModal();

	::CoUninitialize();

	return 0;
}
