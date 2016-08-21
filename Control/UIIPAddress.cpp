#include "stdafx.h"
#include "UIIPAddress.h"
#include <windowsx.h>

#ifndef MAKEIPADDRESS
#define MAKEIPADDRESS(b1,b2,b3,b4)  ((LPARAM)(((DWORD)(b1)<<24)+((DWORD)(b2)<<16)+((DWORD)(b3)<<8)+((DWORD)(b4))))
#endif // !MAKEIPADDRESS

namespace DuiLib
{
UI_IMPLEMENT_DYNCREATE(CIPAddressUI);

//////////////////////////////////////////////////////////////////////////
// CIPAddressWnd
class CIPAddressWnd
    : public CWindowWnd
{
public:
    CIPAddressWnd();
    ~CIPAddressWnd();

    void Init(CIPAddressUI *pOwner);

    RECT CalPos();

    LPCTSTR GetWindowClassName() const;
    LPCTSTR GetSuperClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

public:
    // Determines if all fields in the IP address control are blank.
    BOOL IsBlank() const;

    // Clears the contents of the IP address control.
    void ClearAddress();

    // Retrieves the address values for all four fields in the IP address control.
    int GetAddress(_Out_ BYTE & nField0, _Out_ BYTE & nField1, _Out_ BYTE & nField2, _Out_ BYTE & nField3) const;
    int GetAddress(_Out_ DWORD & dwAddress) const;

    // Sets the address values for all four fields in the IP address control.
    void SetAddress(_In_ DWORD dwAddress);
    void SetAddress(_In_ BYTE nField0, _In_ BYTE nField1, _In_ BYTE nField2, _In_ BYTE nField3);

    // Sets the keyboard focus to the specified field in the IP address control.
    void SetFieldFocus(_In_ WORD nField);

    // Sets the valid range for the specified field in the IP address control.
    void SetFieldRange(_In_ int nField, _In_ BYTE nLower, _In_ BYTE nUpper);

private:
    bool m_bInit;
    CIPAddressUI *m_pOwner;
};

CIPAddressWnd::CIPAddressWnd()
    : m_bInit(false)
    , m_pOwner(NULL)
{
}

CIPAddressWnd::~CIPAddressWnd()
{
}

void CIPAddressWnd::Init(CIPAddressUI *pOwner)
{
    m_pOwner = pOwner;
    CDuiRect rcPos = CalPos();
    // the IP Address Control must be a child
    UINT uStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD;
    Create(m_pOwner->GetManager()->GetHWND(), NULL, uStyle, 0, rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight());

	this->SetAddress(m_pOwner->m_nField0,m_pOwner->m_nField1,m_pOwner->m_nField2,m_pOwner->m_nField3);
	//m_pOwner->SetFocus();
	::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	::SetFocus(m_hWnd);
	m_bInit = true;
}

RECT CIPAddressWnd::CalPos()
{
    CDuiRect rcPos = m_pOwner->GetPos();
    return rcPos;
}

LPCTSTR CIPAddressWnd::GetWindowClassName() const
{
    return _T("IPAddressWnd");
}

LPCTSTR CIPAddressWnd::GetSuperClassName() const
{
    return WC_IPADDRESS;
}

void CIPAddressWnd::OnFinalMessage(HWND hWnd)
{
	m_pOwner->Invalidate();

	m_pOwner->m_pWindow = NULL;
	delete this;
}

LRESULT CIPAddressWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	if(uMsg == WM_KILLFOCUS)
	{
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
	}
	//else if(uMsg == WM_SETFOCUS)
	//{
	//	lRes = OnSetFocus(uMsg, wParam, lParam, bHandled);
	//}
	else if(uMsg == WM_NOTIFY)
	{
		LPNMHDR pNMHDR=(LPNMHDR)lParam;
		if ( pNMHDR->code == IPN_FIELDCHANGED)
		{
			int i = 0;
		}
	}
	else if(uMsg == OCM_COMMAND)
	{
		if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_SETFOCUS)
		{
			//lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		}
		else if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_KILLFOCUS)
		{
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		else if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
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
	else if(uMsg == WM_KEYDOWN)
	{
		if(TCHAR(wParam) == VK_RETURN)
		{
			bHandled = TRUE;
			m_pOwner->GetManager()->SendNotify(m_pOwner, UINOTIFY_EDIT_RETURN);
		}
		else
		{
			bHandled = FALSE;
		}
	}
	else if(uMsg == WM_KEYDOWN)
	{
		if(TCHAR(wParam) == VK_RETURN)
		{
			bHandled = TRUE;
			m_pOwner->GetManager()->SendNotify(m_pOwner, UINOTIFY_EDIT_RETURN);
		}
		else
		{
			bHandled = FALSE;
		}
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

LRESULT CIPAddressWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if(!m_bInit)
	{
		return 0;
	}

	if(m_pOwner == NULL)
	{
		return 0;
	}

	LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);


	DWORD dwIPAddress = MAKEIPADDRESS(m_pOwner->m_nField0, m_pOwner->m_nField1, m_pOwner->m_nField2, m_pOwner->m_nField3);
	CDuiString strIPAddress;
	CDuiCodeOperation::DWORDToIPAddress(dwIPAddress,strIPAddress);
	m_pOwner->SetText(strIPAddress.c_str());
	PostMessage(WM_CLOSE);
	return lRes;
}

LRESULT CIPAddressWnd::OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if(!m_bInit)
	{
		return 0;
	}

	if(m_pOwner == NULL)
	{
		return 0;
	}

	this->GetAddress(m_pOwner->m_nField0,m_pOwner->m_nField1,m_pOwner->m_nField2,m_pOwner->m_nField3);
	return S_OK;
}

BOOL CIPAddressWnd::IsBlank() const
{
    ASSERT(::IsWindow(m_hWnd));
    return (BOOL) ::SendMessage(m_hWnd, IPM_ISBLANK, 0, 0L);
}

void CIPAddressWnd::ClearAddress()
{
    ASSERT(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, IPM_CLEARADDRESS, 0, 0L);
}

int CIPAddressWnd::GetAddress(_Out_ BYTE & nField0, _Out_ BYTE & nField1, _Out_ BYTE & nField2, _Out_ BYTE & nField3) const
{
    ASSERT(::IsWindow(m_hWnd));
    DWORD dwAddress;
    LRESULT nRetVal = ::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM) &dwAddress);
    nField0 = (BYTE) FIRST_IPADDRESS(dwAddress);
    nField1 = (BYTE) SECOND_IPADDRESS(dwAddress);
    nField2 = (BYTE) THIRD_IPADDRESS(dwAddress);
    nField3 = (BYTE) FOURTH_IPADDRESS(dwAddress);
    //IA64: only four fields in an IP address, so an int won't overflow
    return int(nRetVal);
}

int CIPAddressWnd::GetAddress(_Out_ DWORD & dwAddress) const
{
    ASSERT(::IsWindow(m_hWnd));
    return (int) ::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM) &dwAddress);
}

void CIPAddressWnd::SetAddress(_In_ DWORD dwAddress)
{
    ASSERT(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, IPM_SETADDRESS, 0, (LPARAM) dwAddress);
}

void CIPAddressWnd::SetAddress(_In_ BYTE nField0, _In_ BYTE nField1, _In_ BYTE nField2, _In_ BYTE nField3)
{
    ASSERT(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, IPM_SETADDRESS, 0, (LPARAM) MAKEIPADDRESS(nField0, nField1, nField2, nField3));
}

void CIPAddressWnd::SetFieldFocus(_In_ WORD nField)
{
    ASSERT(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, IPM_SETFOCUS, (WPARAM) nField, 0);
}

void CIPAddressWnd::SetFieldRange(_In_ int nField, _In_ BYTE nLower, _In_ BYTE nUpper)
{
    ASSERT(::IsWindow(m_hWnd));
    ::SendMessage(m_hWnd, IPM_SETRANGE, (WPARAM) nField, MAKEIPRANGE(nLower, nUpper));
}

//////////////////////////////////////////////////////////////////////////
// CIPAddressUI
CIPAddressUI::CIPAddressUI(void)
	: m_uButtonState(0)
	, m_pWindow(NULL)
	, m_nField0(0)
	, m_nField1(0)
	, m_nField2(0)
	, m_nField3(0)
{
	m_dwBackColor = 0xFF00FF00;
}


CIPAddressUI::~CIPAddressUI(void)
{
}

bool CIPAddressUI::IsClientArea()
{
	return true;
}

UINT CIPAddressUI::GetControlFlags() const
{
	if(!IsEnabled())
	{
		return CControlUI::GetControlFlags();
	}

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

LPCTSTR CIPAddressUI::GetClass() const
{
    return _T("IPAddress");
}

LPVOID CIPAddressUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("IPAddress")) == 0)
    {
        return static_cast<CIPAddressUI *>(this);
    }

    return CControlUI::GetInterface(pstrName);
}

void CIPAddressUI::EventHandler(TEventUI& event)
{
	if(!IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND)
	{
		if(m_pParent != NULL)
		{
			m_pParent->EventHandler(event);
		}
		else
		{
			CLabelUI::EventHandler(event);
		}

		return;
	}

	if(event.dwType == UIEVENT_SETCURSOR && IsEnabled())
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
		return;
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
			return;
		}
	}

	if(event.dwType == UIEVENT_SETFOCUS)
	{
		if(!IsEnabled())
		{
			return;
		}

		if(m_pWindow)
		{
			return;
		}

		m_pWindow = new CIPAddressWnd();
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
		if( ::PtInRect(&m_rcItem, event.ptMouse) )
		{
			if(IsEnabled())
			{
				GetManager()->ReleaseCapture();

				if(IsFocused() && m_pWindow == NULL)
				{
					m_pWindow = new CIPAddressWnd();
					ASSERT(m_pWindow);
					m_pWindow->Init(this);
				}
				else if(m_pWindow != NULL)
				{

				}
			}
		}
		else
		{
			if ( m_pManager->GetFocus() == this )
				m_pManager->SetFocus(NULL);
		}

		return;
	}

	if(event.dwType == UIEVENT_MOUSEMOVE)
	{
		return;
	}

	if(event.dwType == UIEVENT_LBUTTONUP)
	{
		return;
	}

	if(event.dwType == UIEVENT_CONTEXTMENU)
	{
		return;
	}

	if(event.dwType == UIEVENT_MOUSEENTER)
	{
		if(IsEnabled())
		{
			m_uButtonState |= UISTATE_Hover;
			Invalidate();
		}

		return;
	}

	if(event.dwType == UIEVENT_MOUSELEAVE)
	{
		if(IsEnabled())
		{
			m_uButtonState &= ~UISTATE_Hover;
			Invalidate();
		}

		return;
	}

	CLabelUI::EventHandler(event);
}

void CIPAddressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    CLabelUI::SetAttribute(pstrName, pstrValue);
}

}
