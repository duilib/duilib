// FileTree.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "FileTree.h"
#include "MainDialog.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow)

{

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("\\FileTree\\"));
	//CPaintManagerUI::SetResourceZip(_T("Control.zip"),true);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	
	MainDialog* pFrame = new MainDialog();
	if( pFrame == NULL ) 
		return 0;

	pFrame->Create(NULL, _T("TokenControl"), 
		UI_WNDSTYLE_FRAME,
		UI_WNDSTYLE_EX_FRAME, 0, 0, 300, 500);

	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}