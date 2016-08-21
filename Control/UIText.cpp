#include "StdAfx.h"
#include "UIText.h"

namespace DuiLib
{

	UI_IMPLEMENT_DYNCREATE(CTextUI);
	CTextUI::CTextUI() : m_nLinks(0), m_nHoverLink(-1)
	{
		m_uTextStyle = DT_WORDBREAK;
		m_rcTextPadding.left = 2;
		m_rcTextPadding.right = 2;
		::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
	}

	CTextUI::~CTextUI()
	{
	}

	LPCTSTR CTextUI::GetClass() const
	{
		return _T("TextUI");
	}

	LPVOID CTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Text")) == 0 ) return static_cast<CTextUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CTextUI::GetControlFlags() const
	{
		if( IsEnabled() && m_nLinks > 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	CDuiString* CTextUI::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
		return NULL;
	}

	void CTextUI::EventHandler(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->EventHandler(event);
			else CLabelUI::EventHandler(event);
			return;
		}

		if( event.dwType == UIEVENT_SETCURSOR ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		if( event.dwType == UIEVENT_LBUTTONDOWN || event.dwType == UIEVENT_LDBLCLICK && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					Invalidate();
					return;
				}
			}
		}
		if( event.dwType == UIEVENT_LBUTTONUP && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					m_pManager->SendNotify(this, UINOTIFY_ACTIVEX_LINK, i);
					return;
				}
			}
		}
		if( event.dwType == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		// When you move over a link
		if( m_nLinks > 0 && event.dwType == UIEVENT_MOUSEMOVE && IsEnabled() ) {
			int nHoverLink = -1;
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					nHoverLink = i;
					break;
				}
			}

			if(m_nHoverLink != nHoverLink) {
				m_nHoverLink = nHoverLink;
				Invalidate();
				return;
			}      
		}
		if( event.dwType == UIEVENT_MOUSELEAVE ) {
			if( m_nLinks > 0 && IsEnabled() ) {
				if(m_nHoverLink != -1) {
					m_nHoverLink = -1;
					Invalidate();
					return;
				}
			}
		}

		CLabelUI::EventHandler(event);
	}

	SIZE CTextUI::EstimateSize(SIZE szAvailable)
	{
		RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		if( m_bShowHtml )
		{   
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), m_dwTextColor, NULL, NULL, nLinks, DT_CALCRECT | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText.c_str(), m_dwTextColor, m_strFontID.c_str(), DT_CALCRECT | m_uTextStyle);
		}
		SIZE cXY = {rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
			rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom};

		if( m_cxyFixed.cy != 0 ) cXY.cy = m_cxyFixed.cy;
		return cXY;
	}

	void CTextUI::PaintText(HDC hDC)
	{
		if( m_sText.empty() ) {
			m_nLinks = 0;
			return;
		}

		IResEngine* pResEngine = GetResEngine();
		if( m_dwTextColor == 0 )
			m_dwTextColor = pResEngine->GetDefaultColor(_T("FontColor"));
		if( m_dwDisabledTextColor == 0 )
			m_dwDisabledTextColor = pResEngine->GetDefaultColor(_T("DisableColor"));

		if( m_sText.empty() )
			return;

		m_nLinks = lengthof(m_rcLinks);
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;
		if( IsEnabled() )
		{
		if( m_bShowHtml )
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
		m_rcLinks, m_sLinks, m_nLinks, m_uTextStyle);
		else
		CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwTextColor, \
		m_strFontID.c_str(), m_uTextStyle);
		}
		else {
		if( m_bShowHtml )
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
		m_rcLinks, m_sLinks, m_nLinks, m_uTextStyle);
		else
		CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText.c_str(), m_dwDisabledTextColor, \
		m_strFontID.c_str(), m_uTextStyle);
		}
	}
}
