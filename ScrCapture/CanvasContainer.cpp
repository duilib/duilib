#include "StdAfx.h"
#include "ScrCaptureWnd.h"
#include "CanvasContainer.h"

CCanvasContainerUI::CCanvasContainerUI() : m_iCursor(0), m_uButtonState(0)
{
	m_ptClipBasePoint.x = m_ptClipBasePoint.y = 0;
	m_ptLastMouse.x = m_ptLastMouse.y = 0;
}

LPCTSTR CCanvasContainerUI::GetClass() const
{
	return DUI_CTR_CANVASCONTAINER;
}

LPVOID CCanvasContainerUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_CANVASCONTAINER) == 0 ) return static_cast<CCanvasContainerUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

UINT CCanvasContainerUI::GetControlFlags() const
{
	return UIFLAG_SETCURSOR;
}

RECT CCanvasContainerUI::GetSizerRect(int iIndex)
{
	LONG lMiddleX = (m_rcItem.left + m_rcItem.right) / 2;
	LONG lMiddleY = (m_rcItem.top + m_rcItem.bottom) / 2;
	LONG SIZER_WIDTH = m_rcInset.left*2;
	switch( iIndex ) {
	case 0:
		return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left + SIZER_WIDTH, m_rcItem.top + SIZER_WIDTH);
	case 1:
		return CDuiRect(lMiddleX - SIZER_WIDTH/2, m_rcItem.top, lMiddleX + SIZER_WIDTH/2, m_rcItem.top + SIZER_WIDTH);
	case 2:
		return CDuiRect(m_rcItem.right - SIZER_WIDTH, m_rcItem.top, m_rcItem.right, m_rcItem.top + SIZER_WIDTH);
	case 3:
		return CDuiRect(m_rcItem.left, lMiddleY - SIZER_WIDTH/2, m_rcItem.left + SIZER_WIDTH, lMiddleY + SIZER_WIDTH/2);
	case 4:
		break;
	case 5:
		return CDuiRect(m_rcItem.right - SIZER_WIDTH, lMiddleY - SIZER_WIDTH/2, m_rcItem.right, lMiddleY + SIZER_WIDTH/2);
	case 6:
		return CDuiRect(m_rcItem.left, m_rcItem.bottom - SIZER_WIDTH, m_rcItem.left + SIZER_WIDTH, m_rcItem.bottom);
	case 7:
		return CDuiRect(lMiddleX - SIZER_WIDTH/2, m_rcItem.bottom - SIZER_WIDTH, lMiddleX + SIZER_WIDTH/2, m_rcItem.bottom);
	case 8:
		return CDuiRect(m_rcItem.right - SIZER_WIDTH, m_rcItem.bottom - SIZER_WIDTH, m_rcItem.right, m_rcItem.bottom);
	default:
		break;
	}
	return CDuiRect();
}

LPTSTR CCanvasContainerUI::GetSizerCursor(POINT& pt, int& iCursor)
{
	LONG SIZER_WIDTH = m_rcInset.left*2;
	LONG SIZER_TO_ROOT = 20;	
	RECT rcRoot = m_pManager->GetRoot()->GetPos();

	iCursor = -1;
	for( int i = 8; i >= 0; --i ) {
		RECT rc = GetSizerRect(i);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = i;
			break;
		}
	}
	if (iCursor < 0 && rcRoot.bottom - m_rcItem.bottom < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.left, m_rcItem.bottom-SIZER_WIDTH, m_rcItem.right, m_rcItem.bottom);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 7;
		}
	}
	if (iCursor < 0 && rcRoot.right - m_rcItem.right < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.right-SIZER_WIDTH, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 5;
		}
	}
	if (iCursor < 0 && m_rcItem.left - rcRoot.left < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left+SIZER_WIDTH, m_rcItem.bottom);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 3;
		}
	}
	if (iCursor < 0 && m_rcItem.top - rcRoot.top < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top+SIZER_WIDTH);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 1;
		}
	}
	if (iCursor < 0) iCursor = 4;
	switch( iCursor ) {
	case 0:
		return IDC_SIZENWSE;
	case 1:
		return IDC_SIZENS;
	case 2:
		return IDC_SIZENESW;
	case 3:
		return IDC_SIZEWE;
	case 4:
		return IDC_SIZEALL;
	case 5:
		return IDC_SIZEWE;
	case 6:
		return IDC_SIZENESW;
	case 7:
		return IDC_SIZENS;
	case 8:
		return IDC_SIZENWSE;
	default:
		return IDC_SIZEALL;
	}
}

void CCanvasContainerUI::PaintBorder(HDC hDC) {
	for (int i = 0; i < 9; ++i) {
		CRenderEngine::DrawColor(hDC, GetSizerRect(i), m_dwBorderColor);
	}
	RECT rc = m_rcItem;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.left;
	rc.right -= m_rcInset.left;
	rc.bottom -= m_rcInset.left;
	CRenderEngine::DrawRect(hDC, rc, m_rcBorderSize.left, m_dwBorderColor);
}

void CCanvasContainerUI::DoEvent(TEventUI& event) 
{
	if( event.Type == UIEVENT_SETCURSOR && IsEnabled())
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) return;
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(GetSizerCursor(event.ptMouse, m_iCursor))));
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
	{
		m_uButtonState |= UISTATE_CAPTURED;
		m_ptLastMouse = event.ptMouse;
		RECT rcWindow = CScrCaptureWnd::Instance()->GetWindowRect();
		RECT rcClipPadding = CScrCaptureWnd::Instance()->GetClipPadding();
		switch( m_iCursor ) {
		case 0:
		case 1:
		case 3:
			m_ptClipBasePoint.x = rcWindow.right-rcClipPadding.right;
			m_ptClipBasePoint.y = rcWindow.bottom-rcClipPadding.bottom;
			break;
		case 2:
		case 5:
			m_ptClipBasePoint.x = rcWindow.left+rcClipPadding.left;
			m_ptClipBasePoint.y = rcWindow.bottom-rcClipPadding.bottom;
			break;
		case 6:
		case 7:
			m_ptClipBasePoint.x = rcWindow.right-rcClipPadding.right;
			m_ptClipBasePoint.y = rcWindow.top+rcClipPadding.top;
			break;
		case 8:
		default:
			m_ptClipBasePoint.x = rcWindow.left+rcClipPadding.left;
			m_ptClipBasePoint.y = rcWindow.top+rcClipPadding.top;
			break;
		}
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~UISTATE_CAPTURED;
		}
		return;
	}
	else if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) == 0 ) return;
		RECT rcWindow = CScrCaptureWnd::Instance()->GetWindowRect();
		RECT rcClipPadding = CScrCaptureWnd::Instance()->GetClipPadding();
		switch( m_iCursor ) {
		case 0:
		case 2:
		case 6:
		case 8:
			rcClipPadding.left = min(event.ptMouse.x, m_ptClipBasePoint.x)-rcWindow.left; if (rcClipPadding.left < 0) rcClipPadding.left = 0;
			rcClipPadding.top = min(event.ptMouse.y, m_ptClipBasePoint.y)-rcWindow.top; if (rcClipPadding.top < 0) rcClipPadding.top = 0;
			rcClipPadding.right = rcWindow.right-max(event.ptMouse.x, m_ptClipBasePoint.x); if (rcClipPadding.right < 0) rcClipPadding.right = 0;
			rcClipPadding.bottom = rcWindow.bottom-max(event.ptMouse.y, m_ptClipBasePoint.y); if (rcClipPadding.bottom < 0) rcClipPadding.bottom = 0;
			break;
		case 1:
		case 7:
			rcClipPadding.top = min(event.ptMouse.y, m_ptClipBasePoint.y)-rcWindow.top; if (rcClipPadding.top < 0) rcClipPadding.top = 0;
			rcClipPadding.bottom = rcWindow.bottom-max(event.ptMouse.y, m_ptClipBasePoint.y); if (rcClipPadding.bottom < 0) rcClipPadding.bottom = 0;
			break;
		case 3:
		case 5:
			rcClipPadding.left = min(event.ptMouse.x, m_ptClipBasePoint.x)-rcWindow.left; if (rcClipPadding.left < 0) rcClipPadding.left = 0;
			rcClipPadding.right = rcWindow.right-max(event.ptMouse.x, m_ptClipBasePoint.x); if (rcClipPadding.right < 0) rcClipPadding.right = 0;
			break;
		default:
			{
				LONG cx = event.ptMouse.x - m_ptLastMouse.x;
				LONG cy = event.ptMouse.y - m_ptLastMouse.y;
				if (rcClipPadding.left + cx >= 0 && rcClipPadding.right - cx >=0) {
					rcClipPadding.left += cx;
					rcClipPadding.right -= cx;
				}
				if (rcClipPadding.top + cy >= 0 && rcClipPadding.bottom - cy >=0) {
					rcClipPadding.top += cy;
					rcClipPadding.bottom -= cy;
				}
			}
			break;
		}
		m_ptLastMouse = event.ptMouse;
		CScrCaptureWnd::Instance()->SetClipPadding(rcClipPadding);
	}
	CContainerUI::DoEvent(event);
}
