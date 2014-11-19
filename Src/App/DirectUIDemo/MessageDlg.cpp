#include "stdafx.h"
#include "MessageDlg.h"


CMessageDlg::CMessageDlg(void)
{
}

CMessageDlg::~CMessageDlg(void)
{
}

void CMessageDlg::Init(HWND hWndParent)
{
	this->CreateDuiWindow(hWndParent,_T("MainWnd"),WS_OVERLAPPEDWINDOW | WS_POPUP);
	this->CenterWindow();
	this->ShowWindow();

	this->AddNotify(this);
}

void CMessageDlg::Notify(TNotifyUI *pMsg)
{

}

void CMessageDlg::OnFinalMessage(HWND hWnd)
{
		//this->RemoveNotify(this);
}
