#include "StdAfx.h"
#include "UIText.h"

namespace DuiLib
{
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
		return DUI_CTR_TEXT;
	}

	LPVOID CTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_TEXT) == 0 ) return static_cast<CTextUI*>(this);
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

	void CTextUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					Invalidate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
					return;
				}
			}
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		// When you move over a link
		if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE && IsEnabled() ) {
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
		if( event.Type == UIEVENT_MOUSELEAVE ) {
			if( m_nLinks > 0 && IsEnabled() ) {
				if(m_nHoverLink != -1) {
                    if( !::PtInRect(&m_rcLinks[m_nHoverLink], event.ptMouse) ) {
                        m_nHoverLink = -1;
                        Invalidate();
                        if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
                    }
                    else {
                        if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
                        return;
                    }
				}
			}
		}

		CLabelUI::DoEvent(event);
	}

	void CTextUI::PaintText(HDC hDC)
	{
		if( m_sText.IsEmpty() ) {
			m_nLinks = 0;
			return;
		}

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;

		m_nLinks = lengthof(m_rcLinks);
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;
		if( IsEnabled() ) {
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
				m_rcLinks, m_sLinks, m_nLinks, m_iFont, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor, \
				m_iFont, m_uTextStyle);
		}
		else {
			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
				m_rcLinks, m_sLinks, m_nLinks, m_iFont, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);
		}
	}
}
