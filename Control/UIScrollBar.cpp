#include "stdafx.h"
#include "UIScrollBar.h"

namespace DuiLib
{
	UI_IMPLEMENT_DYNCREATE(CScrollBarUI);

	CScrollBarUI::CScrollBarUI() : m_bHorizontal(false), m_nRange(100), m_nScrollPos(0), m_nLineSize(8), 
		m_pOwner(NULL), m_nLastScrollPos(0), m_nLastScrollOffset(0), m_nScrollRepeatDelay(0), m_uButton1State(0), \
		m_uButton2State(0), m_uThumbState(0), m_bShowButton1(true), m_bShowButton2(true),m_minThumbSize(DEFAULT_SCROLLBAR_SIZE)
	{
		m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
		ptLastMouse.x = ptLastMouse.y = 0;
		::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
		::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
	}

	LPCTSTR CScrollBarUI::GetClass() const
	{
		return _T("ScrollBarUI");
	}

	LPVOID CScrollBarUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ScrollBar")) == 0 ) return static_cast<CScrollBarUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	bool CScrollBarUI::IsClientArea()
	{
		return true;
	}

	CContainerUI* CScrollBarUI::GetOwner() const
	{
		return m_pOwner;
	}

	void CScrollBarUI::SetOwner(CContainerUI* pOwner)
	{
		m_pOwner = pOwner;
	}

	void CScrollBarUI::SetVisible(bool bVisible)
	{
		if( m_bVisible == bVisible ) return;

		bool v = IsVisible();
		m_bVisible = bVisible;
		if( m_bFocused ) m_bFocused = false;
	}

	void CScrollBarUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButton1State = 0;
			m_uButton2State = 0;
			m_uThumbState = 0;
		}
	}

	void CScrollBarUI::SetFocus()
	{
		if( m_pOwner != NULL ) m_pOwner->SetFocus();
		else CControlUI::SetFocus();
	}

	bool CScrollBarUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CScrollBarUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		if( m_bHorizontal ) {
			if( m_cxyFixed.cy == 0 ) {
				m_cxyFixed.cx = 0;
				m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
			}
		}
		else {
			if( m_cxyFixed.cx == 0 ) {
				m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
				m_cxyFixed.cy = 0;
			}
		}

		if( m_pOwner != NULL ) m_pOwner->NeedUpdate(); else NeedParentUpdate();
	}

	int CScrollBarUI::GetScrollRange() const
	{
		return m_nRange;
	}

	void CScrollBarUI::SetScrollRange(int nRange)
	{
		if( m_nRange == nRange ) return;

		m_nRange = nRange;
		if( m_nRange < 0 ) m_nRange = 0;
		if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
		SetPos(m_rcItem,true);
	}

	int CScrollBarUI::GetScrollPos() const
	{
		return m_nScrollPos;
	}

	void CScrollBarUI::SetScrollPos(int nPos)
	{
		if( m_nScrollPos == nPos ) return;

		m_nScrollPos = nPos;
		if( m_nScrollPos < 0 ) m_nScrollPos = 0;
		if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
		SetPos(m_rcItem,true);
	}

	int CScrollBarUI::GetLineSize() const
	{
		return m_nLineSize;
	}

	void CScrollBarUI::SetLineSize(int nSize)
	{
		m_nLineSize = nSize;
	}

	bool CScrollBarUI::GetShowButton1()
	{
		return m_bShowButton1;
	}

	void CScrollBarUI::SetShowButton1(bool bShow)
	{
		m_bShowButton1 = bShow;
		SetPos(m_rcItem);
	}

	LPCTSTR CScrollBarUI::GetButton1NormalImage()
	{
		return m_sButton1NormalImage.c_str();
	}

	void CScrollBarUI::SetButton1NormalImage(LPCTSTR pStrImage)
	{
		m_sButton1NormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetButton1HotImage()
	{
		return m_sButton1HotImage.c_str();
	}

	void CScrollBarUI::SetButton1HotImage(LPCTSTR pStrImage)
	{
		m_sButton1HotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetButton1PushedImage()
	{
		return m_sButton1PushedImage.c_str();
	}

	void CScrollBarUI::SetButton1PushedImage(LPCTSTR pStrImage)
	{
		m_sButton1PushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetButton1DisabledImage()
	{
		return m_sButton1DisabledImage.c_str();
	}

	void CScrollBarUI::SetButton1DisabledImage(LPCTSTR pStrImage)
	{
		m_sButton1DisabledImage = pStrImage;
		Invalidate();
	}

	bool CScrollBarUI::GetShowButton2()
	{
		return m_bShowButton2;
	}

	void CScrollBarUI::SetShowButton2(bool bShow)
	{
		m_bShowButton2 = bShow;
		SetPos(m_rcItem);
	}

	LPCTSTR CScrollBarUI::GetButton2NormalImage()
	{
		return m_sButton2NormalImage.c_str();
	}

	void CScrollBarUI::SetButton2NormalImage(LPCTSTR pStrImage)
	{
		m_sButton2NormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetButton2HotImage()
	{
		return m_sButton2HotImage.c_str();
	}

	void CScrollBarUI::SetButton2HotImage(LPCTSTR pStrImage)
	{
		m_sButton2HotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetButton2PushedImage()
	{
		return m_sButton2PushedImage.c_str();
	}

	void CScrollBarUI::SetButton2PushedImage(LPCTSTR pStrImage)
	{
		m_sButton2PushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetButton2DisabledImage()
	{
		return m_sButton2DisabledImage.c_str();
	}

	void CScrollBarUI::SetButton2DisabledImage(LPCTSTR pStrImage)
	{
		m_sButton2DisabledImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetThumbNormalImage()
	{
		return m_sThumbNormalImage.c_str();
	}

	void CScrollBarUI::SetThumbNormalImage(LPCTSTR pStrImage)
	{
		m_sThumbNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetThumbHotImage()
	{
		return m_sThumbHotImage.c_str();
	}

	void CScrollBarUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		m_sThumbHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetThumbPushedImage()
	{
		return m_sThumbPushedImage.c_str();
	}

	void CScrollBarUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		m_sThumbPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetThumbDisabledImage()
	{
		return m_sThumbDisabledImage.c_str();
	}

	void CScrollBarUI::SetThumbDisabledImage(LPCTSTR pStrImage)
	{
		m_sThumbDisabledImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetRailNormalImage()
	{
		return m_sRailNormalImage.c_str();
	}

	void CScrollBarUI::SetRailNormalImage(LPCTSTR pStrImage)
	{
		m_sRailNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetRailHotImage()
	{
		return m_sRailHotImage.c_str();
	}

	void CScrollBarUI::SetRailHotImage(LPCTSTR pStrImage)
	{
		m_sRailHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetRailPushedImage()
	{
		return m_sRailPushedImage.c_str();
	}

	void CScrollBarUI::SetRailPushedImage(LPCTSTR pStrImage)
	{
		m_sRailPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetRailDisabledImage()
	{
		return m_sRailDisabledImage.c_str();
	}

	void CScrollBarUI::SetRailDisabledImage(LPCTSTR pStrImage)
	{
		m_sRailDisabledImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetBkNormalImage()
	{
		return m_sBkNormalImage.c_str();
	}

	void CScrollBarUI::SetBkNormalImage(LPCTSTR pStrImage)
	{
		m_sBkNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetBkHotImage()
	{
		return m_sBkHotImage.c_str();
	}

	void CScrollBarUI::SetBkHotImage(LPCTSTR pStrImage)
	{
		m_sBkHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetBkPushedImage()
	{
		return m_sBkPushedImage.c_str();
	}

	void CScrollBarUI::SetBkPushedImage(LPCTSTR pStrImage)
	{
		m_sBkPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CScrollBarUI::GetBkDisabledImage()
	{
		return m_sBkDisabledImage.c_str();
	}

	void CScrollBarUI::SetBkDisabledImage(LPCTSTR pStrImage)
	{
		m_sBkDisabledImage = pStrImage;
		Invalidate();
	}

	void CScrollBarUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc,bNeedInvalidate);
		rc = m_rcItem;

		if( m_bHorizontal ) {
			int cx = rc.right - rc.left;
			if( m_bShowButton1 ) cx -= m_cxyFixed.cy;
			if( m_bShowButton2 ) cx -= m_cxyFixed.cy;
			if( cx > m_cxyFixed.cy ) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cy;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if( m_bShowButton2 ) {
					m_rcButton2.left = rc.right - m_cxyFixed.cy;
					m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				m_rcThumb.top = rc.top;
				m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
				if( m_nRange > 0 ) {
					int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
					if( cxThumb < m_minThumbSize ) cxThumb = m_minThumbSize;

					m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
					m_rcThumb.right = m_rcThumb.left + cxThumb;
					if( m_rcThumb.right > m_rcButton2.left ) {
						m_rcThumb.left = m_rcButton2.left - cxThumb;
						m_rcThumb.right = m_rcButton2.left;
					}
				}
				else {
					m_rcThumb.left = m_rcButton1.right;
					m_rcThumb.right = m_rcButton2.left;
				}
			}
			else {
				int cxButton = (rc.right - rc.left) / 2;
				if( cxButton > m_cxyFixed.cy ) cxButton = m_cxyFixed.cy;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + cxButton;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if( m_bShowButton2 ) {
					m_rcButton2.left = rc.right - cxButton;
					m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
			}
		}
		else {//¡ä1?¡À
			int cy = rc.bottom - rc.top;
			if( m_bShowButton1 ) cy -= m_cxyFixed.cx;
			if( m_bShowButton2 ) cy -= m_cxyFixed.cx;
			if( cy > m_cxyFixed.cx ) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cx;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if( m_bShowButton2 ) {
					m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
					m_rcButton2.right = rc.left + m_cxyFixed.cx;
				}
				else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				m_rcThumb.left = rc.left;
				m_rcThumb.right = rc.left + m_cxyFixed.cx;
				if( m_nRange > 0 ) {
					int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
					//if( cyThumb < m_cxyFixed.cx ) cyThumb = m_cxyFixed.cx;
					if(cyThumb < m_minThumbSize) cyThumb = m_minThumbSize;

					m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcThumb.top + cyThumb;
					if( m_rcThumb.bottom > m_rcButton2.top ) {
						m_rcThumb.top = m_rcButton2.top - cyThumb;
						m_rcThumb.bottom = m_rcButton2.top;
					}
				}
				else {
					m_rcThumb.top = m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			}
			else {
				int cyButton = (rc.bottom - rc.top) / 2;
				if( cyButton > m_cxyFixed.cx ) cyButton = m_cxyFixed.cx;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cx;
					m_rcButton1.bottom = rc.top + cyButton;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if( m_bShowButton2 ) {
					m_rcButton2.top = rc.bottom - cyButton;
					m_rcButton2.right = rc.left + m_cxyFixed.cx;
				}
				else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
			}
		}
	}

	void CScrollBarUI::EventHandler(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->EventHandler(event);
			else CControlUI::EventHandler(event);
			return;
		}

		if( event.dwType == UIEVENT_SETFOCUS ) 
		{
			return;
		}
		if( event.dwType == UIEVENT_KILLFOCUS ) 
		{
			return;
		}
		if( event.dwType == UIEVENT_LBUTTONDOWN || event.dwType == UIEVENT_LDBLCLICK )
		{
			if( !IsEnabled() ) return;

			m_nLastScrollOffset = 0;
			m_nScrollRepeatDelay = 0;
			m_pManager->SetTimer(this, DEFAULT_TIMERID, 50U);

			if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
				m_uButton1State |= UISTATE_Pushed;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineUp(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
			}
			else if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
				m_uButton2State |= UISTATE_Pushed;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineDown(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineRight(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
			}
			else if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
				m_uThumbState |= UISTATE_Captured | UISTATE_Pushed;
				ptLastMouse = event.ptMouse;
				m_nLastScrollPos = m_nScrollPos;
			}
			else {
				if( !m_bHorizontal ) {
					if( event.ptMouse.y < m_rcThumb.top ) {
						if( m_pOwner != NULL ) m_pOwner->PageUp(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
					}
					else if ( event.ptMouse.y > m_rcThumb.bottom ){
						if( m_pOwner != NULL ) m_pOwner->PageDown(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
					}
				}
				else {
					if( event.ptMouse.x < m_rcThumb.left ) {
						if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
					}
					else if ( event.ptMouse.x > m_rcThumb.right ){
						if( m_pOwner != NULL ) m_pOwner->PageRight(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
					}
				}
			}
			if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->SendNotify(this, UINOTIFY_SCROLL);
			return;
		}
		if( event.dwType == UIEVENT_LBUTTONUP )
		{
			m_nScrollRepeatDelay = 0;
			m_nLastScrollOffset = 0;
			m_pManager->KillTimer(this, DEFAULT_TIMERID);

			if( (m_uThumbState & UISTATE_Captured) != 0 ) {
				m_uThumbState &= ~( UISTATE_Captured | UISTATE_Pushed );
				Invalidate();
			}
			else if( (m_uButton1State & UISTATE_Pushed) != 0 ) {
				m_uButton1State &= ~UISTATE_Pushed;
				Invalidate();
			}
			else if( (m_uButton2State & UISTATE_Pushed) != 0 ) {
				m_uButton2State &= ~UISTATE_Pushed;
				Invalidate();
			}
			return;
		}
		if( event.dwType == UIEVENT_MOUSEMOVE )
		{
			if( (m_uThumbState & UISTATE_Captured) != 0 ) {
				if( !m_bHorizontal ) {

					int vRange = m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top - 2 * m_cxyFixed.cx;

					if (vRange != 0)
						m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / vRange;
					
				}
				else {

					int hRange = m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left - 2 * m_cxyFixed.cy;

					if (hRange != 0)
						m_nLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_nRange / hRange;
				}
			}
			else {
				if( (m_uThumbState & UISTATE_Hover) != 0 ) {
					if( !::PtInRect(&m_rcThumb, event.ptMouse) ) {
						m_uThumbState &= ~UISTATE_Hover;
						Invalidate();
					}
				}
				else {
					if( !IsEnabled() ) return;
					if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
						m_uThumbState |= UISTATE_Hover;
						Invalidate();
					}
				}
			}
			return;
		}
		if( event.dwType == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.dwType == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID )
		{
			++m_nScrollRepeatDelay;
			if( (m_uThumbState & UISTATE_Captured) != 0 ) {
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->SetScrollPos(CDuiSize(m_pOwner->GetScrollPos().cx, \
						m_nLastScrollPos + m_nLastScrollOffset)); 
					else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->SetScrollPos(CDuiSize(m_nLastScrollPos + m_nLastScrollOffset, \
						m_pOwner->GetScrollPos().cy)); 
					else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
				}
				Invalidate();
			}
			else if( (m_uButton1State & UISTATE_Pushed) != 0 ) {
				if( m_nScrollRepeatDelay <= 5 ) return;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineUp(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
			}
			else if( (m_uButton2State & UISTATE_Pushed) != 0 ) {
				if( m_nScrollRepeatDelay <= 5 ) return;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineDown(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineRight(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
			}
			else {
				if( m_nScrollRepeatDelay <= 5 ) return;
				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_pManager->GetHWND(), &pt);
				if( !m_bHorizontal ) {
					if( pt.y < m_rcThumb.top ) {
						if( m_pOwner != NULL ) m_pOwner->PageUp(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
					}
					else if ( pt.y > m_rcThumb.bottom ){
						if( m_pOwner != NULL ) m_pOwner->PageDown(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
					}
				}
				else {
					if( pt.x < m_rcThumb.left ) {
						if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
					}
					else if ( pt.x > m_rcThumb.right ){
						if( m_pOwner != NULL ) m_pOwner->PageRight(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
					}
				}
			}
			if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->SendNotify(this, UINOTIFY_SCROLL);
			return;
		}
		if( event.dwType == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButton1State |= UISTATE_Hover;
				m_uButton2State |= UISTATE_Hover;
				if( ::PtInRect(&m_rcThumb, event.ptMouse) ) m_uThumbState |= UISTATE_Hover;
				Invalidate();
			}
			return;
		}
		if( event.dwType == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButton1State &= ~UISTATE_Hover;
				m_uButton2State &= ~UISTATE_Hover;
				m_uThumbState &= ~UISTATE_Hover;
				Invalidate();
			}
			return;
		}

		if( m_pOwner != NULL ) m_pOwner->EventHandler(event); else CControlUI::EventHandler(event);
	}

	void CScrollBarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("button1normalimage")) == 0 ) SetButton1NormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button1hotimage")) == 0 ) SetButton1HotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button1pushedimage")) == 0 ) SetButton1PushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button1disabledimage")) == 0 ) SetButton1DisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button2normalimage")) == 0 ) SetButton2NormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button2hotimage")) == 0 ) SetButton2HotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button2pushedimage")) == 0 ) SetButton2PushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("button2disabledimage")) == 0 ) SetButton2DisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbnormalimage")) == 0 ) SetThumbNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbdisabledimage")) == 0 ) SetThumbDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("railnormalimage")) == 0 ) SetRailNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("railhotimage")) == 0 ) SetRailHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("railpushedimage")) == 0 ) SetRailPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("raildisabledimage")) == 0 ) SetRailDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("bknormalimage")) == 0 ) SetBkNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("bkhotimage")) == 0 ) SetBkHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("bkpushedimage")) == 0 ) SetBkPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("bkdisabledimage")) == 0 ) SetBkDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("linesize")) == 0 ) SetLineSize(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("range")) == 0 ) SetScrollRange(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetScrollPos(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("showbutton1")) == 0 ) SetShowButton1(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("showbutton2")) == 0 ) SetShowButton2(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("minthumbsize")) == 0 ) m_minThumbSize = CDuiCodeOperation::StringToInt(pstrValue);
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CScrollBarUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		PaintBk(hDC);
		PaintButton1(hDC);
		PaintButton2(hDC);
		PaintThumb(hDC);
		PaintRail(hDC);
	}

	void CScrollBarUI::PaintBk(HDC hDC)
	{
		if( !IsEnabled() ) m_uThumbState |= UISTATE_Disabled;
		else m_uThumbState &= ~ UISTATE_Disabled;

		if( (m_uThumbState & UISTATE_Disabled) != 0 ) {
			if( !m_sBkDisabledImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sBkDisabledImage.c_str()) ) m_sBkDisabledImage.clear();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_Pushed) != 0 ) {
			if( !m_sBkPushedImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sBkPushedImage.c_str()) )
					m_sBkPushedImage.clear();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_Hover) != 0 ) {
			if( !m_sBkHotImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sBkHotImage.c_str()) )
					m_sBkHotImage.clear();
				else return;
			}
		}

		if( !m_sBkNormalImage.empty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sBkNormalImage.c_str()) ) m_sBkNormalImage.clear();
			else return;
		}
	}

	void CScrollBarUI::PaintButton1(HDC hDC)
	{
		if( !m_bShowButton1 ) return;

		if( !IsEnabled() ) m_uButton1State |= UISTATE_Disabled;
		else m_uButton1State &= ~ UISTATE_Disabled;

		m_sImageModify = CDuiStringOperation::format(_T("dest='%d,%d,%d,%d'"), m_rcButton1.left - m_rcItem.left, \
			m_rcButton1.top - m_rcItem.top, m_rcButton1.right - m_rcItem.left, m_rcButton1.bottom - m_rcItem.top);

		if( (m_uButton1State & UISTATE_Disabled) != 0 ) {
			if( !m_sButton1DisabledImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sButton1DisabledImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) )
					m_sButton1DisabledImage.clear();
				else return;
			}
		}
		else if( (m_uButton1State & UISTATE_Pushed) != 0 ) {
			if( !m_sButton1PushedImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sButton1PushedImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) )
					m_sButton1PushedImage.clear();
				else return;
			}
		}
		else if( (m_uButton1State & UISTATE_Hover) != 0 ) {
			if( !m_sButton1HotImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sButton1HotImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) ) m_sButton1HotImage.clear();
				else return;
			}
		}

		if( !m_sButton1NormalImage.empty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sButton1NormalImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) ) m_sButton1NormalImage.clear();
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintButton2(HDC hDC)
	{
		if( !m_bShowButton2 ) return;

		if( !IsEnabled() ) m_uButton2State |= UISTATE_Disabled;
		else m_uButton2State &= ~ UISTATE_Disabled;

		m_sImageModify.clear();
		m_sImageModify = CDuiStringOperation::format(_T("dest='%d,%d,%d,%d'"), m_rcButton2.left - m_rcItem.left, \
			m_rcButton2.top - m_rcItem.top, m_rcButton2.right - m_rcItem.left, m_rcButton2.bottom - m_rcItem.top);

		if( (m_uButton2State & UISTATE_Disabled) != 0 ) {
			if( !m_sButton2DisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sButton2DisabledImage.c_str(), m_sImageModify.c_str()) )
					m_sButton2DisabledImage.clear();
				else return;
			}
		}
		else if( (m_uButton2State & UISTATE_Pushed) != 0 ) {
			if( !m_sButton2PushedImage.empty() ) {
				if( !DrawImage(hDC, m_sButton2PushedImage.c_str(), m_sImageModify.c_str()) ) 
					m_sButton2PushedImage.clear();
				else return;
			}
		}
		else if( (m_uButton2State & UISTATE_Hover) != 0 ) {
			if( !m_sButton2HotImage.empty() )
			{
				if( !DrawImage(hDC, m_sButton2HotImage.c_str(), m_sImageModify.c_str()) )
					m_sButton2HotImage.clear();
				else return;
			}
		}

		if( !m_sButton2NormalImage.empty() ) {
			if( !DrawImage(hDC, m_sButton2NormalImage.c_str(), m_sImageModify.c_str()) )
				m_sButton2NormalImage.clear();
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect(hDC, m_rcButton2, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintThumb(HDC hDC)
	{
		if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
		if( !IsEnabled() ) m_uThumbState |= UISTATE_Disabled;
		else m_uThumbState &= ~ UISTATE_Disabled;

		m_sImageModify = CDuiStringOperation::format(_T("dest='%d,%d,%d,%d'"), m_rcThumb.left - m_rcItem.left, \
			m_rcThumb.top - m_rcItem.top, m_rcThumb.right - m_rcItem.left, m_rcThumb.bottom - m_rcItem.top);

		if( (m_uThumbState & UISTATE_Disabled) != 0 ) {
			if( !m_sThumbDisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sThumbDisabledImage.c_str(), m_sImageModify.c_str()) )
					m_sThumbDisabledImage.clear();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_Pushed) != 0 ) {
			if( !m_sThumbPushedImage.empty() ) {
				if( !DrawImage(hDC, m_sThumbPushedImage.c_str(), m_sImageModify.c_str()) )
					m_sThumbPushedImage.clear();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_Hover) != 0 ) {
			if( !m_sThumbHotImage.empty() ) {
				if( !DrawImage(hDC, m_sThumbHotImage.c_str(), m_sImageModify.c_str()) )
					m_sThumbHotImage.clear();
				else return;
			}
		}

		if( !m_sThumbNormalImage.empty() ) {
			if( !DrawImage(hDC, m_sThumbNormalImage.c_str(),m_sImageModify.c_str()) )
				m_sThumbNormalImage.clear();
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect(hDC, m_rcThumb, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintRail(HDC hDC)
	{
		if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
		if( !IsEnabled() ) m_uThumbState |= UISTATE_Disabled;
		else m_uThumbState &= ~ UISTATE_Disabled;

		if( !m_bHorizontal )
		{
			m_sImageModify = CDuiStringOperation::format(_T("dest='%d,%d,%d,%d'"), m_rcThumb.left - m_rcItem.left, \
				(m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top - m_cxyFixed.cx / 2, \
				m_rcThumb.right - m_rcItem.left, \
				(m_rcThumb.top + m_rcThumb.bottom) / 2 - m_rcItem.top + m_cxyFixed.cx - m_cxyFixed.cx / 2);
		}
		else {
			m_sImageModify = CDuiStringOperation::format(_T("dest='%d,%d,%d,%d'"), \
				(m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left - m_cxyFixed.cy / 2, \
				m_rcThumb.top - m_rcItem.top, \
				(m_rcThumb.left + m_rcThumb.right) / 2 - m_rcItem.left + m_cxyFixed.cy - m_cxyFixed.cy / 2, \
				m_rcThumb.bottom - m_rcItem.top);
		}

		if( (m_uThumbState & UISTATE_Disabled) != 0 )
		{
			if( !m_sRailDisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sRailDisabledImage.c_str(), m_sImageModify.c_str()) )
					m_sRailDisabledImage.clear();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_Pushed) != 0 ) {
			if( !m_sRailPushedImage.empty() ) {
				if( !DrawImage(hDC, m_sRailPushedImage.c_str(), m_sImageModify.c_str()) )
					m_sRailPushedImage.clear();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_Hover) != 0 ) {
			if( !m_sRailHotImage.empty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sRailHotImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) ) m_sRailHotImage.clear();
				else return;
			}
		}

		if( !m_sRailNormalImage.empty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sRailNormalImage.c_str(), (LPCTSTR)m_sImageModify.c_str()) ) m_sRailNormalImage.clear();
			else return;
		}
	}
}
