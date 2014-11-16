#include "stdafx.h"
#include "MainWnd.h"


CMainWnd::CMainWnd(void)
{
	
}


CMainWnd::~CMainWnd(void)
{
}

void CMainWnd::OnFinalMessage(HWND hWnd)
{
	::PostQuitMessage(0);
}

void CMainWnd::Init()
{
	this->CreateDuiWindow(NULL,_T("MainWnd"),WS_OVERLAPPEDWINDOW);
	this->CenterWindow();
	this->ShowWindow();
}
