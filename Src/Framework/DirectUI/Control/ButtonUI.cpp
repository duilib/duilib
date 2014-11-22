#include "stdafx.h"
#include "ButtonUI.h"

UI_IMPLEMENT_DYNCREATE(CButtonUI);

CButtonUI::CButtonUI(void)
	: m_pHoverImage(NULL)
	, m_pPushedImage(NULL)
	, m_pFocusedImage(NULL)
	, m_pDisabledImage(NULL)
	, m_pForeHoverImage(NULL)
	, m_pForePushedImage(NULL)
{
}


CButtonUI::~CButtonUI(void)
{
	
	if ( m_pHoverImage != NULL)
		delete m_pHoverImage;
	if ( m_pPushedImage != NULL)
		delete m_pPushedImage;
	if ( m_pFocusedImage != NULL)
		delete m_pFocusedImage;
	if ( m_pDisabledImage != NULL)
		delete m_pDisabledImage;
	if ( m_pForeHoverImage != NULL)
		delete m_pForeHoverImage;
	if ( m_pForePushedImage != NULL)
		delete m_pForePushedImage;
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
		m_dwState = 0;
	}
}

void CButtonUI::Render(IUIRender* pRender,LPCRECT pRcPaint)
{
	CDuiRect rcPaint;
	if ( !rcPaint.IntersectRect(&m_rcControl,pRcPaint) )
		return;

	if( IsFocused())
		ModifyState(UISTATE_Focused,0);
	else
		ModifyState(0,UISTATE_Focused);
	if( !IsEnabled() )
		ModifyState(UISTATE_Disabled,0);
	else
		ModifyState(0,UISTATE_Disabled);

	CUIPaint::GetInstance()->DrawButton(pRender,this,&m_rcControl);
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
		if( m_rcControl.PtInRect(event.ptMouse) && IsEnabled() )
		{
			ModifyState(UISTATE_Pushed | UISTATE_Captured);
			Invalidate();
		}
		return true;
	}
	if( event.dwType == UIEVENT_LBUTTONUP )
	{
		if( (m_dwState & UISTATE_Captured) != 0 )
		{
			if( m_rcControl.PtInRect(event.ptMouse) )
				Activate();
			ModifyState(0,UISTATE_Pushed | UISTATE_Captured);
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
			ModifyState(UISTATE_Hover);
			Invalidate();
		}
		// return;
	}
	if( event.dwType == UIEVENT_MOUSEMOVE )
	{
		if( CheckState(UISTATE_Captured) )
		{
			if( m_rcControl.PtInRect( event.ptMouse) )
				ModifyState(UISTATE_Pushed);
			else
				ModifyState(0,UISTATE_Pushed);
			Invalidate();
		}
		return true;
	}
	if( event.dwType == UIEVENT_MOUSELEAVE )
	{
		if( IsEnabled() )
		{
			ModifyState(0,UISTATE_Hover);
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

void CButtonUI::SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState)
{
	ImageObject **pImageObj = NULL;

	if ( propType == UIProperty_Fore_Image )
	{
		switch (dwState)
		{
		case UISTATE_Hover:
			pImageObj = &m_pForeHoverImage;
			break;
		case UISTATE_Pushed:
			pImageObj = &m_pForePushedImage;
			break;
		}

		if ( pImageObj != NULL)
		{
			*pImageObj = ImageObject::CreateFromString(lpszImageString);
			return;
		}
	}

	if ( propType == UIProperty_Back_Image)
	{
		switch (dwState)
		{
		case UISTATE_Hover:
			pImageObj = &m_pHoverImage;
			break;
		case UISTATE_Pushed:
			pImageObj = &m_pPushedImage;
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
			*pImageObj = ImageObject::CreateFromString(lpszImageString);
			return;
		}
	}

	CControlUI::SetImage(lpszImageString,propType,dwState);
}

ImageObject* CButtonUI::GetImageProperty(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	if ( propType == UIProperty_Fore_Image )
	{
		switch (dwState)
		{
		case UISTATE_Hover:
			return m_pForeHoverImage;
		case UISTATE_Pushed:
			return m_pForePushedImage;
			break;
		}
	}

	if ( propType == UIProperty_Back_Image)
	{
		switch (dwState)
		{
		case UISTATE_Hover:
			return m_pHoverImage;
			break;
		case UISTATE_Pushed:
			return m_pPushedImage;
			break;
		case UISTATE_Focused:
			return m_pFocusedImage;
			break;
		case UISTATE_Disabled:
			return m_pDisabledImage;
			break;
		}
	}
	return CControlUI::GetImageProperty(propType,dwState);
}

