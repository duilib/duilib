#include "stdafx.h"
#include "EditUI.h"
#include <WindowsX.h>

//////////////////////////////////////////////////////////////////////////
// CEditWnd

class DIRECTUI_API CEditWnd : public CWindowWnd
{
public:
	CEditWnd();

	void Init(CEditUI *pOwner);
	RECT CalPos();

	LPCTSTR GetWindowClassName() const;
	LPCTSTR GetSuperClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);


protected:
	CEditUI *m_pOwner;
	HBRUSH m_hBkBrush;
	bool m_bInit;
};

UI_IMPLEMENT_DYNCREATE(CEditUI);

CEditUI::CEditUI(void)
	: m_pWindow(NULL)
	, m_uMaxChar(255)
	, m_bReadOnly(false)
	, m_bPasswordMode(false)
	, m_cPasswordChar(_T('*'))
	, m_dwEditbkColor(0xFFFFFFFF)
	, m_iWindowStyls(0)
	, m_bShowDefault(FALSE)
	, m_clrDefaultText(0)
	, m_backupPasswordChar(0)
	, m_backupTextClr(0)
	, m_backupPasswordMode(false)
{
	CDuiRect rect(4, 3, 4, 3);
	SetTextPadding(&rect);
	
	//SetBkColor(0xFFFFFFFF);
}


CEditUI::~CEditUI(void)
{
}

LPCTSTR CEditUI::GetClass() const
{
	 return _T("Edit");
}

LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
{
	if(_tcscmp(pstrName, _T("Edit")) == 0)
	{
		return static_cast<CEditUI *>(this);
	}

	return CLabelUI::GetInterface(pstrName);
}

UINT CEditUI::GetControlFlags() const
{
	if(!IsEnabled())
	{
		return CControlUI::GetControlFlags();
	}

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void CEditUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (lpszValue == NULL || lpszValue[0] == NULL)
		return;

	VecString vec;
	CDuiStringOperation::splite(lpszName, _T("."), vec);
	if (vec.size() == 2)
	{
		LPCTSTR lpszStateName = vec[0].c_str();
		LPCTSTR lpszPropertyName = vec[1].c_str();

		DWORD dwState;
		// 为dwState赋值
		if (_tcsicmp(lpszStateName, _T("normal")) == 0)
			dwState = UISTATE_Normal;
		else if (_tcsicmp(lpszStateName, _T("hover")) == 0)
			dwState = UISTATE_Hover;
		else if (_tcsicmp(lpszStateName, _T("pushed")) == 0)
			dwState = UISTATE_Pushed;
		else if (_tcsicmp(lpszStateName, _T("focused")) == 0)
			dwState = UISTATE_Focused;
		else if (_tcsicmp(lpszStateName, _T("disabled")) == 0)
			dwState = UISTATE_Disabled;

		UIProperty propType;
		// 为propType赋值
		if (_tcsicmp(lpszPropertyName, _T("bkcolor1")) == 0)
			propType = UIProperty_Back_Color1;
		else if (_tcsicmp(lpszPropertyName, _T("bkcolor2")) == 0)
			propType = UIProperty_Back_Color2;
		else if (_tcsicmp(lpszPropertyName, _T("bkcolor3")) == 0)
			propType = UIProperty_Back_Color3;
		else if (_tcsicmp(lpszPropertyName, _T("bkimage")) == 0)
			propType = UIProperty_Back_Image;
		else if (_tcsicmp(lpszPropertyName, _T("foreimage")) == 0)
			propType = UIProperty_Fore_Image;
		else if (_tcsicmp(lpszPropertyName, _T("bordercolor")) == 0)
			propType = UIProperty_Border_Color;
		else if (_tcsicmp(lpszPropertyName, _T("borderwidth")) == 0)
			propType = UIProperty_Border_Wdith;
		else if (_tcsicmp(lpszPropertyName, _T("borderstyle")) == 0)
			propType = UIProperty_Border_Style;

		SetPropertyForState(lpszValue, propType, dwState);
	}

	if ( _tcsicmp(lpszName,_T("hotimage")) == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Image, UISTATE_Hover);
	else if ( _tcsicmp(lpszName,_T("pushedimage")) == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Image, UISTATE_Pushed);
	else if ( _tcsicmp(lpszName,_T("focusedimage")) == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Image, UISTATE_Focused);
	else if ( _tcsicmp(lpszName,_T("disabledimage")) == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Image, UISTATE_Disabled);

	CLabelUI::SetAttribute(lpszName,lpszValue);
}

void CEditUI::SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState)
{
	ImageObject **pImageObj = NULL;

	if ( propType == UIProperty_Back_Image)
	{
		switch (dwState)
		{
		case UISTATE_Hover:
			pImageObj = &m_pHoverImage;
			break;
		case UISTATE_Focused:
			pImageObj = &m_pFocusedImage;
			break;
		case UISTATE_Disabled:
			pImageObj = &m_pDisabledImage;
			break;
		}
		if ( pImageObj != NULL)
		{
			if ( *pImageObj != NULL)
				delete *pImageObj;
			*pImageObj = ImageObject::CreateFromString(lpszImageString);
			return;
		}
	}

	CControlUI::SetImage(lpszImageString,propType,dwState);
}

void CEditUI::SetPosition(LPCRECT rc)
{
	CControlUI::SetPosition(rc);

	if(m_pWindow != NULL)
	{
		RECT rcPos = m_pWindow->CalPos();
		::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
			rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CEditUI::SetEnabled(bool bEnable)
{
	CControlUI::SetEnabled(bEnable);

	if(!IsEnabled())
	{
		m_dwState = 0;
	}
}

void CEditUI::SetText(LPCTSTR pstrText)
{
	m_sText = pstrText;

	if(m_pWindow != NULL)
	{
		Edit_SetText(*m_pWindow, m_sText.c_str());
	}

	Invalidate();
}

void CEditUI::SetMaxChar(UINT uMax)
{
	m_uMaxChar = uMax;

	if(m_pWindow != NULL)
	{
		Edit_LimitText(*m_pWindow, m_uMaxChar);
	}
}

UINT CEditUI::GetMaxChar()
{
	return m_uMaxChar;
}

void CEditUI::SetReadOnly(bool bReadOnly)
{
	if(m_bReadOnly == bReadOnly)
	{
		return;
	}

	m_bReadOnly = bReadOnly;

	if(m_pWindow != NULL)
	{
		Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
	}

	Invalidate();
}

bool CEditUI::IsReadOnly() const
{
	return m_bReadOnly;
}

void CEditUI::SetNumberOnly(bool bNumberOnly)
{
	if(bNumberOnly)
	{
		m_iWindowStyls |= ES_NUMBER;
	}
	else
	{
		m_iWindowStyls |= ~ES_NUMBER;
	}
}

bool CEditUI::IsNumberOnly() const
{
	return m_iWindowStyls & ES_NUMBER ? true : false;
}

int CEditUI::GetWindowStyls() const
{
	return m_iWindowStyls;
}

void CEditUI::SetPasswordMode(bool bPasswordMode)
{
	if(m_bPasswordMode == bPasswordMode)
	{
		return;
	}

	m_bPasswordMode = bPasswordMode;
	Invalidate();
}

bool CEditUI::IsPasswordMode() const
{
	return m_bPasswordMode;
}

void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
{
	if(m_cPasswordChar == cPasswordChar)
	{
		return;
	}

	m_cPasswordChar = cPasswordChar;

	if(m_pWindow != NULL)
	{
		Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
	}

	Invalidate();
}

TCHAR CEditUI::GetPasswordChar() const
{
	return m_cPasswordChar;
}

void CEditUI::SetVisible(bool bVisible /*= true*/)
{
	CControlUI::SetVisible(bVisible);

	if(!IsVisible() && m_pWindow != NULL)
	{
		m_pManager->SetFocus(NULL);
	}
}

void CEditUI::SetNativeEditBkColor(DWORD dwBkColor)
{
	m_dwEditbkColor = dwBkColor;
}

DWORD CEditUI::GetNativeEditBkColor() const
{
	return m_dwEditbkColor;
}

void CEditUI::SetInternVisible(bool bVisible /*= true*/)
{
	if(!IsVisible() && m_pWindow != NULL)
	{
		m_pManager->SetFocus(NULL);
	}
}

SIZE CEditUI::EstimateSize(SIZE szAvailable)
{
	if(m_cxyFixed.cy == 0)
	{
		return CDuiSize(m_cxyFixed.cx, CUIEngine::GetInstance()->GetFontObject(GetFontName())->GetTextMetric().tmHeight + 6);
	}

	return CControlUI::EstimateSize(szAvailable);
}

void CEditUI::Init()
{
	if(m_sText.empty())
	{
		ShowDefaultText(TRUE);
	}

	CLabelUI::Init();
}

bool CEditUI::EventHandler(TEventUI& event)
{
	if(!IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND)
	{
		if(m_pParent != NULL)
		{
			return m_pParent->EventHandler(event);
		}
		else
		{
			return CLabelUI::EventHandler(event);
		}
	}

	if(event.dwType == UIEVENT_SETCURSOR && IsEnabled())
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		return true;
	}

	if(event.dwType == UIEVENT_WINDOWSIZE)
	{
		if(m_pWindow != NULL)
		{
			m_pManager->SetFocusNeeded(this);
		}
	}

	if(event.dwType == UIEVENT_SCROLLWHEEL)
	{
		if(m_pWindow != NULL)
		{
			return true;
		}
	}

	if(event.dwType == UIEVENT_SETFOCUS)
	{
		if(!IsEnabled())
		{
			return true;
		}

		if(m_pWindow)
		{
			return true;
		}

		m_pWindow = new CEditWnd();
		ASSERT(m_pWindow);
		m_pWindow->Init(this);
		Invalidate();
	}

	if(event.dwType == UIEVENT_KILLFOCUS && IsEnabled())
	{
		Invalidate();
	}

	if(event.dwType == UIEVENT_LBUTTONDOWN || event.dwType == UIEVENT_LDBLCLICK || event.dwType == UIEVENT_RBUTTONDOWN)
	{
		if(IsEnabled())
		{
			GetManager()->ReleaseCapture();

			if(IsFocused() && m_pWindow == NULL)
			{
				m_pWindow = new CEditWnd();
				ASSERT(m_pWindow);
				m_pWindow->Init(this);

				if(PtInRect(&m_rcControl, event.ptMouse))
				{
					int nSize = GetWindowTextLength(*m_pWindow);

					if(nSize == 0)
					{
						nSize = 1;
					}

					Edit_SetSel(*m_pWindow, 0, nSize);
				}
			}
			else
				if(m_pWindow != NULL)
				{
#if 1
					int nSize = GetWindowTextLength(*m_pWindow);

					if(nSize == 0)
					{
						nSize = 1;
					}

					Edit_SetSel(*m_pWindow, 0, nSize);
#else
					POINT pt = event.ptMouse;
					pt.x -= m_rcItem.left + m_rcTextPadding.left;
					pt.y -= m_rcItem.top + m_rcTextPadding.top;
					::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
				}
		}

		return true;
	}

	if(event.dwType == UIEVENT_MOUSEMOVE)
	{
		return true;
	}

	if(event.dwType == UIEVENT_LBUTTONUP)
	{
		return true;
	}

	if(event.dwType == UIEVENT_CONTEXTMENU)
	{
		return true;
	}

	if(event.dwType == UIEVENT_MOUSEENTER)
	{
		if(IsEnabled())
		{
			m_dwState |= UISTATE_Hover;
			Invalidate();
		}

		return true;
	}

	if(event.dwType == UIEVENT_MOUSELEAVE)
	{
		if(IsEnabled())
		{
			m_dwState &= ~UISTATE_Hover;
			Invalidate();
		}

		return true;
	}

	return CLabelUI::EventHandler(event);
}

void CEditUI::ShowDefaultText(BOOL bShow)
{
	if(bShow)
	{
		m_bShowDefault = TRUE;
		m_backupTextClr = m_dwTextColor;
		m_backupPasswordChar = m_cPasswordChar;
		m_backupPasswordMode = m_bPasswordMode;

		if(m_backupPasswordMode)
		{
			SetPasswordMode(false);
			SetPasswordChar(0);
		}

		SetTextColor(m_clrDefaultText);
		SetText(m_sDefaultText.c_str());
	}
	else
	{
		if(m_bShowDefault == TRUE)
		{
			if(m_backupPasswordMode == true)
			{
				SetPasswordMode(true);
				SetPasswordChar(m_backupPasswordChar);
			}

			SetText(TEXT(""));
			m_bShowDefault = FALSE;
			SetTextColor(m_backupTextClr);
		}
	}
}

void CEditUI::Render(IUIRender* pRender, LPCRECT pRcPaint)
{
	CUIPaint::GetInstance()->DrawEdit(pRender,this,pRcPaint);
}

//////////////////////////////////////////////////////////////////////////
// CEditWnd Implement

CEditWnd::CEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
{
}

void CEditWnd::Init(CEditUI *pOwner)
{
	m_pOwner = pOwner;
	CDuiRect rcPos = CalPos();
	UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;

	if(m_pOwner->IsPasswordMode())
	{
		uStyle |= ES_PASSWORD;
	}

	Create(m_pOwner->GetManager()->GetHWND(), NULL, uStyle, 0, rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight());
	HFONT hFont = NULL;
	FontObject* pFont = NULL;
	LPCTSTR pFontName = m_pOwner->GetFontName();

	if(pFontName != NULL)
	{
		
		pFont = CResourceManager::GetInstance()->GetFont(pFontName);
	}

	if(pFont == NULL)
	{
		pFont = m_pOwner->GetManager()->GetDefaultFont();
	}

	SetWindowFont(m_hWnd, pFont->GetFont(), TRUE);
	Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());

	if(m_pOwner->IsPasswordMode())
	{
		Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
	}

	Edit_SetText(m_hWnd, m_pOwner->GetText());
	Edit_SetModify(m_hWnd, FALSE);
	SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
	Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
	Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
	//Styls
	LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
	styleValue |= pOwner->GetWindowStyls();
	::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
	m_bInit = true;
}

RECT CEditWnd::CalPos()
{
	CDuiRect rcPos = m_pOwner->GetPosition();
	RECT rcInset = m_pOwner->GetTextPadding();
	rcPos.left += rcInset.left;
	rcPos.top += rcInset.top;
	rcPos.right -= rcInset.right;
	rcPos.bottom -= rcInset.bottom;
	LONG lEditHeight = 0;
	
	FontObject *pFontObj = CUIEngine::GetInstance()->GetFontObject(m_pOwner->GetFontName());
	TEXTMETRIC tm = m_pOwner->GetManager()->GetTM(pFontObj->GetFont());
	lEditHeight = tm.tmHeight;

	if(lEditHeight < rcPos.GetHeight())
	{
		rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
		rcPos.bottom = rcPos.top + lEditHeight;
	}

	return rcPos;
}

LPCTSTR CEditWnd::GetWindowClassName() const
{
	return _T("EditWnd");
}

LPCTSTR CEditWnd::GetSuperClassName() const
{
	return WC_EDIT;
}

void CEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
	m_pOwner->Invalidate();

	// Clear reference and die
	if(m_hBkBrush != NULL)
	{
		::DeleteObject(m_hBkBrush);
	}

	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT CEditWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	if(uMsg == WM_KILLFOCUS)
	{
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == WM_SETFOCUS)
	{
		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled);
	}
	else if(uMsg == OCM_COMMAND)
	{
		if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
		{
			lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		}
		else if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
		{
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			::InvalidateRect(m_hWnd, &rcClient, FALSE);
		}
	}
	else if(uMsg == WM_KEYDOWN )
	{
		if ( TCHAR(wParam) == VK_RETURN )
		{
			bHandled = TRUE;
			m_pOwner->GetManager()->SendNotify(m_pOwner, UINOTIFY_EDIT_RETURN);
		}
		else
		{
			bHandled = FALSE;
		}
	}
	else if(uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC)
	{
		if(m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF)
		{
			return NULL;
		}

		::SetBkMode((HDC)wParam, TRANSPARENT);
		DWORD dwTextColor = m_pOwner->GetTextColor();
		::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

		if(m_hBkBrush == NULL)
		{
			DWORD clrColor = m_pOwner->GetNativeEditBkColor();
			m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		}

		return (LRESULT)m_hBkBrush;
	}
	else
	{
		bHandled = FALSE;
	}

	if(!bHandled)
	{
		return CWindowWnd::WindowProc(uMsg, wParam, lParam);
	}

	return lRes;
}



LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

	if(m_pOwner->m_sText.empty() && m_pOwner->m_sDefaultText.empty() == FALSE)
	{
		m_pOwner->ShowDefaultText(TRUE);
	}

	PostMessage(WM_CLOSE);
	return lRes;
}

LRESULT CEditWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if(m_pOwner->m_bShowDefault)
	{
		m_pOwner->ShowDefaultText(FALSE);
	}

	bHandled = FALSE;
	//LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
	if(!m_bInit)
	{
		return 0;
	}

	if(m_pOwner == NULL)
	{
		return 0;
	}

	// Copy text back
	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	ASSERT(pstr);

	if(pstr == NULL)
	{
		return 0;
	}

	::GetWindowText(m_hWnd, pstr, cchLen);
	m_pOwner->m_sText = pstr;
	m_pOwner->GetManager()->SendNotify(m_pOwner, UINOTIFY_EDIT_CHANGE);
	return 0;
}