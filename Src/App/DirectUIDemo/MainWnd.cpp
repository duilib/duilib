#include "stdafx.h"
#include "MainWnd.h"
#include "MessageDlg.h"

CMainWnd::CMainWnd(void)
{
	
}


CMainWnd::~CMainWnd(void)
{
}

void CMainWnd::OnFinalMessage(HWND hWnd)
{
	this->RemoveNotify(this);
	::PostQuitMessage(0);
}

void CMainWnd::Init()
{
	this->CreateDuiWindow(NULL,_T("MainWnd"),WS_OVERLAPPEDWINDOW);
	this->CenterWindow();
	this->ShowWindow();

	this->AddNotify(this);
}

void CMainWnd::Notify(TNotifyUI *pMsg)
{
	if ( pMsg->dwType == UINOTIFY_LBTNCLICK )
	{
		CDuiString strControlName = pMsg->pSender->GetName();
		if ( CDuiStringOperation::compareNoCase(strControlName.c_str(),_T("btnClose")) ==0 )
		{
			CMessageDlg dlg;
			dlg.Init(m_hWnd);
			dlg.ShowModal();
			//this->CloseWindow();
			return;
		}
	}
}
