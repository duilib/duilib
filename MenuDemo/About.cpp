#include "StdAfx.h"
#include "About.h"

const TCHAR* const kSysBtnControlName = _T("SysButton");
const TCHAR* const kCloseButtonControlName = _T("closebtn");

const TCHAR* const kLogoControlName = _T("logo_btn");
const TCHAR* const kTitleControlName = _T("title");
const TCHAR* const kBackgroundControlName = _T("Background");
const TCHAR* const kClientRectControlName = _T("ClientRect");

CAbout::CAbout(HWND hWndParent,const RECT rc)
{

	m_bModal = false;

    Create(hWndParent, _T("About"), WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW,rc, 0);

	MoveWindow(m_hWnd,rc.left, rc.top, rc.right, rc.bottom, FALSE);

	ShowWindow(true);

	//ShowModal();
}


CAbout::~CAbout(void)
{
}

LPCTSTR CAbout::GetWindowClassName() const 
{ 
	return _T("AboutWindow");
}

void CAbout::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CAbout::OnPrepare(TNotifyUI& msg)
{

}
void CAbout::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{

	}
	else if( msg.sType == _T("click") ) 
	{

		if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
		{
			Close();
			return; 
		}
	}
}

void CAbout::InitWindow()
{

}

CDuiString CAbout::GetSkinFile()
{
	return CSkinSetting::GetSkinFile();
}

CDuiString CAbout::GetSkinFolder()
{
	return CSkinSetting::GetSkinFolder();
}

UILIB_RESOURCETYPE CAbout::GetResourceType() const
{
	return  CSkinSetting::GetResourceType();
}

LPCTSTR CAbout::GetResourceID() const
{
	return  CSkinSetting::GetResourceID();
}

CDuiString CAbout::GetZIPFileName() const
{
	return CSkinSetting::GetZIPFileName();
}

DWORD CAbout::GetBkColor()
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
		return background->GetBkColor();

	return 0;
}

void CAbout::SetBkColor(DWORD dwBackColor)
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		background->SetBkImage(_T(""));
		background->SetBkColor(dwBackColor);
		background->NeedUpdate();
	}
}

void CAbout::SetBkImage(LPCTSTR szImageName)
{

	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
	{

		background->SetBkImage(szImageName);

	}
}

LRESULT CAbout::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	POINT pt; 

	pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);

	RECT btnRect;

	CButtonUI* linkbtn;
	
	linkbtn = (CButtonUI*)m_PaintManager.FindControl(_T("www.131qz.com"));

	if (linkbtn != NULL)
	{

		btnRect = linkbtn->GetPos();

		if(pt.x > btnRect.left && pt.x < btnRect.right && pt.y > btnRect.top && pt.y < btnRect.bottom)
		{

			linkbtn->SetTextColor(0xffff0000);
			linkbtn->NeedUpdate();

		}else{

			linkbtn->SetTextColor(0xff0000ff);
			linkbtn->NeedUpdate();
		}

	}

	bHandled = FALSE;
	return 0;
}

LRESULT CAbout::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;
}

LRESULT CAbout::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;

	return 0;
}

LRESULT CAbout::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//Close();
	return 0;
}

LRESULT CAbout::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{

	//wParam为0时表示是失去焦点，为非0时表示得到焦点
	if (m_bModal)
	{
		if (! wParam)
		{


		}else{


		}
	}

	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

DWORD CAbout::DoModal()
{
	if(! this->m_hWnd)
		return -1;
	m_bModal = true;
	return ShowModal();
}
