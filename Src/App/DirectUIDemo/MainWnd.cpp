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

bool CMainWnd::Notify(TNotifyUI *pMsg)
{
	//if ( pMsg->dwType == UINOTIFY_LBTNCLICK )
	//{
	//	CDuiString strControlName = pMsg->pSender->GetName();
	//	if ( CDuiStringOperation::compareNoCase(strControlName.c_str(),_T("btnClose")) ==0 )
	//	{
	//		CMessageDlg *pDlg = new CMessageDlg;
	//		pDlg->SetAutoDelete(true);
	//		pDlg->Init(m_hWnd);
	//		pDlg->DoModal();	// 设置了自动删除，这个函数返回后就不能再访问模态窗口指针了
	//		return;
	//	}
	//}
	return false;
}
