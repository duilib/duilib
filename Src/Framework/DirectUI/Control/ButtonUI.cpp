#include "stdafx.h"
#include "ButtonUI.h"

UI_IMPLEMENT_DYNCREATE(CButtonUI);

CButtonUI::CButtonUI(void)
	: m_pNormalImage(NULL)
	, m_pHoverImage(NULL)
	, m_pPushedImage(NULL)
	, m_pFocusedImage(NULL)
	, m_pDisabledImage(NULL)
	, m_pForeHoverImage(NULL)
	, m_pForePushedImage(NULL)
{
}


CButtonUI::~CButtonUI(void)
{
	if ( m_pNormalImage != NULL)
	{
		m_pNormalImage->Release();
		delete m_pNormalImage;
	}
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

ImageObject *ParseImageString(LPCTSTR lpszImageString)
{
	ImageObject *pImage = new ImageObject;
	StringMap attributeMap;
	if ( !CDuiStringOperation::parseAttributeString(lpszImageString,attributeMap))
	{
		CDuiString strFullPath;
		CResourceManager::GetInstance()->GetAbsolutePath(strFullPath,lpszImageString);
		pImage->SetImagePath(strFullPath.c_str());
	}
	else
	{
		CDuiString strTemp;
		// file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
		// file
		strTemp = FindAttrubuteKey(attributeMap,_T("file"));
		CDuiString strFullPath;
		CResourceManager::GetInstance()->GetAbsolutePath(strFullPath,strTemp.c_str());
		pImage->SetImagePath(strFullPath.c_str());
		// corner
		// dest
		// source
		// corner
		// mask
		// fade
		// hole
		// xtiled ytiled
	}
	return pImage;
}

void CButtonUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcscmp(lpszName, _T("normalimage")) == 0 )
		SetNormalImage(lpszValue);
	else
		CControlUI::SetAttribute(lpszName,lpszValue);
}

void CButtonUI::SetNormalImage(LPCTSTR lpszImageString)
{
	if ( CDuiStringOperation::compareNoCase(m_strNormalImage.c_str(),lpszImageString) != 0)
	{
		m_pNormalImage->Release();
	}
	m_strNormalImage = lpszImageString;
	m_pNormalImage = ParseImageString(lpszImageString);
}


