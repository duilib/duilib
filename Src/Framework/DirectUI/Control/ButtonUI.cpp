#include "stdafx.h"
#include "ButtonUI.h"

UI_IMPLEMENT_DYNCREATE(CButtonUI);

CButtonUI::CButtonUI(void)
{
}


CButtonUI::~CButtonUI(void)
{
}

LPCTSTR CButtonUI::GetClass() const
{
	return _T("Button");
}

LPVOID CButtonUI::GetInterface(LPCTSTR lpszClass)
{
	if ( _tcscmp(lpszClass,_T("Button")))
		return this;
	else
		return CControlUI::GetInterface(lpszClass);
}

UINT CButtonUI::GetControlFlags() const
{
	return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
}

bool CButtonUI::Activate()
{
	if( !CControlUI::Activate() )
		return false;
	if( m_pManager != NULL )
		m_pManager->SendNotify(this, UINOTIFY_LBTNCLICK);
	return true;
}

void CButtonUI::SetEnabled(bool bEnable /*= true*/)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() )
	{
		m_uState = UISTATE_Normal;
	}
}

void CButtonUI::Render(IUIRender* pRender,LPCRECT pRcPaint)
{
	CDuiRect rcPaint;
	rcPaint.IntersectRect(&m_rcItem,pRcPaint);
	CControlUI::Render(pRender,&rcPaint);
}

bool CButtonUI::EventHandler(TEventUI& event)
{
	if( !IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND )
	{
		if( m_pParent != NULL )
			m_pParent->EventHandler(event);
		else
			CControlUI::EventHandler(event);
		return true;
	}

	if( event.dwType == UIEVENT_SETFOCUS ) 
	{
		Invalidate();
	}
	if( event.dwType == UIEVENT_KILLFOCUS ) 
	{
		Invalidate();
	}
	if( event.dwType == UIEVENT_KEYDOWN )
	{
		if (IsKeyboardEnabled())
		{
			if( event.chKey == VK_SPACE || event.chKey == VK_RETURN )
			{
				Activate();
				return true;
			}
		}
	}
	if( event.dwType == UIEVENT_LBUTTONDOWN || event.dwType == UIEVENT_LDBLCLICK )
	{
		if( m_rcItem.PtInRect(event.ptMouse) && IsEnabled() )
		{
			m_uState |= UISTATE_Pushed | UISTATE_Captured;
			Invalidate();
		}
		return true;
	}
	if( event.dwType == UIEVENT_LBUTTONUP )
	{
		if( (m_uState & UISTATE_Captured) != 0 )
		{
			if( m_rcItem.PtInRect(event.ptMouse) )
				Activate();
			m_uState &= ~(UISTATE_Pushed | UISTATE_Captured);
			Invalidate();
		}
		return true;
	}
	if( event.dwType == UIEVENT_CONTEXTMENU )
	{
		if( IsContextMenuUsed() )
		{
			m_pManager->SendNotify(this,UINOTIFY_CONTEXTMENU, event.wParam, event.lParam);
		}
		return true;
	}
	if( event.dwType == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() )
		{
			m_uState |= UISTATE_Hover;
			Invalidate();
		}
		// return;
	}
	if( event.dwType == UIEVENT_MOUSEMOVE )
	{
		if( (m_uState & UISTATE_Captured) != 0 )
		{
			if( m_rcItem.PtInRect( event.ptMouse) )
				m_uState |= UISTATE_Pushed;
			else
				m_uState &= ~UISTATE_Pushed;
			Invalidate();
		}
		return true;
	}
	if( event.dwType == UIEVENT_MOUSELEAVE )
	{
		if( IsEnabled() )
		{
			m_uState &= ~UISTATE_Hover;
			Invalidate();
		}
		// return;
	}
	if( event.dwType == UIEVENT_SETCURSOR )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return true;
	}
	return CControlUI::EventHandler(event);
}

void CButtonUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CControlUI::SetAttribute(lpszName,lpszValue);
}
