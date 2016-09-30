#include "StdAfx.h"
#include "MainWnd.h"
#include "ButtonUIEx.h"

CMainWnd::CMainWnd()
{
}

CMainWnd::~CMainWnd()
{
}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	if(_tcscmp(pstrClass, _T("ButtonEx")) == 0)
	{
		return new CButtonUIEx;
	}

	return NULL;
}

LRESULT CMainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;	//ȡ�������
	styleValue &= ~WS_SIZEBOX;	//RWH ȡ���Զ������߿�ķ��,���Է��������Զ����
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_PM.Init(m_hWnd);
	CDialogBuilder builder; 
	CControlUI* pRoot = builder.Create(_T("MainWnd.xml"), (UINT)0,  this, &m_PM);		//ֱ���޸�ResĿ¼�����MainWnd - ����.xmlΪ����ļ���,���ɿ�������һ��Ч����
	ASSERT(pRoot && "Failed to parse XML");
	m_PM.AttachDialog(pRoot);
	m_PM.AddNotifier(this);

	InitDlg();

	return 0;
}

void CMainWnd::InitDlg()
{
}

LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	switch( uMsg ) 
	{
	case WM_CREATE:				lRes = OnCreate(uMsg, wParam, lParam, bHandled);		break;
	case WM_NCHITTEST:			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);		break;
	case WM_SYSCOMMAND:			lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);	break;
	case WM_TIMER:
		{
			if (wParam == 1)
			{
				RECT rcCurPos = m_pTestBtn->GetPos();
				static bool bZoomBiging = true;
				RECT rc = {0,0,0,0};
				if (rcCurPos.left == 0)
				{//���
					bZoomBiging = false;
					rc.left = rcCurPos.left+1;
					rc.top = rcCurPos.top+1;
					rc.right = rcCurPos.right-1;
					rc.bottom = rcCurPos.bottom-1;
				}
				else if(rcCurPos.left == 16)
				{//��С
					bZoomBiging = true;
					rc.left = rcCurPos.left-1;
					rc.top = rcCurPos.top-1;
					rc.right = rcCurPos.right+1;
					rc.bottom = rcCurPos.bottom+1;
				}
				else
				{
					if (bZoomBiging)
					{
						rc.left = rcCurPos.left-1;
						rc.top = rcCurPos.top-1;
						rc.right = rcCurPos.right+1;
						rc.bottom = rcCurPos.bottom+1;
					}
					else
					{
						rc.left = rcCurPos.left+1;
						rc.top = rcCurPos.top+1;
						rc.right = rcCurPos.right-1;
						rc.bottom = rcCurPos.bottom-1;
					}
				}
				m_pTestBtn->SetPos(rc);
			}
			bHandled = FALSE;
		}
		break;
	case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				Close();
			}
			else
				bHandled = FALSE;
		}
	default:
		bHandled = FALSE;
	}
	if( bHandled ) return lRes;
	if( m_PM.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	if ( msg.sType == DUI_MSGTYPE_CLICK)
	{
	}
	else if (msg.sType == DUI_MSGTYPE_TIMER)
	{
		if (_tcscmp(msg.pSender->GetClass(),_T("CButtonUIEx"))==0)
		{
			CButtonUIEx* pBtnEx = (CButtonUIEx*)msg.pSender;
			pBtnEx->OnTimer(msg.wParam);
		}
	}
}

LRESULT CMainWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_PM.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_PM.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("CButtonUIEx")) != 0)
				return HTCAPTION;
	}
	return HTCLIENT;
}

LRESULT CMainWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == SC_CLOSE ) 
	{
		Close(0);

		bHandled = TRUE;
		return 0;
	}
	else if (wParam == SC_KEYMENU)	//RWH �����ϵͳ�˵�����Ӧ,���ε�ALT+SPACE����ϵͳ�˵��Ĳ���
		return 0;

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);//���뱣����������Ϣ�ַ��ͳ������ˡ�
}