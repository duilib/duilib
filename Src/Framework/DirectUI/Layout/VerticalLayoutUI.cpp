#include "stdafx.h"
#include "VerticalLayoutUI.h"

UI_IMPLEMENT_DYNCREATE(CVerticalLayoutUI);

CVerticalLayoutUI::CVerticalLayoutUI(void)
	: m_iSepHeight(0)
	, m_bImmMode(false)
{
}


CVerticalLayoutUI::~CVerticalLayoutUI(void)
{
}

LPCTSTR CVerticalLayoutUI::GetClass() const
{
	return _T("VerticalLayout");
}

LPVOID CVerticalLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("VerticalLayout")) == 0 )
		return static_cast<CVerticalLayoutUI*>(this);
	return
		CContainerUI::GetInterface(pstrName);
}

UINT CVerticalLayoutUI::GetControlFlags() const
{
	if( IsEnabled() && m_iSepHeight != 0 )
		return UIFLAG_SETCURSOR;
	else
		return 0;
}

void CVerticalLayoutUI::SetSepHeight(int iHeight)
{
	m_iSepHeight = iHeight;
}

int CVerticalLayoutUI::GetSepHeight() const
{
	return m_iSepHeight;
}

void CVerticalLayoutUI::SetSepImmMode(bool bImmediately)
{
	if( m_bImmMode == bImmediately ) return;
	if( (m_dwState & UISTATE_Captured) != 0 && !m_bImmMode && m_pManager != NULL )
	{
		m_pManager->RemovePostPaint(this);
	}

	m_bImmMode = bImmediately;
}

bool CVerticalLayoutUI::IsSepImmMode() const
{
	return m_bImmMode;
}

void CVerticalLayoutUI::PostRender(IUIRender* pRender,LPCRECT pRcPaint)
{
	if( (m_dwState & UISTATE_Captured) != 0 && !m_bImmMode )
	{
		RECT rcSeparator = GetThumbRect(true);
		pRender->DrawColor(&rcSeparator, 0xAA000000);
	}
}

void CVerticalLayoutUI::SetPosition(LPCRECT rc)
{
	CControlUI::SetPosition(rc);
	CDuiRect rcTemp(m_rcControl);

	// Adjust for inset
	rcTemp.left += m_rcInset.left;
	rcTemp.top += m_rcInset.top;
	rcTemp.right -= m_rcInset.right;
	rcTemp.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
		rcTemp.right -= m_pVerticalScrollBar->GetFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		rcTemp.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	if( m_items.GetSize() == 0)
	{
		ProcessScrollBar(rcTemp, 0, 0);
		return;
	}

	// Determine the minimum size
	SIZE szAvailable = { rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top };
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

	int nAdjustables = 0;
	int cyFixed = 0;
	int nEstimateNum = 0;
	for( int it1 = 0; it1 < m_items.GetSize(); it1++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if( !pControl->IsVisible() )
			continue;
		if( pControl->IsFloat() )
			continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if( sz.cy == 0 )
		{
			nAdjustables++;
		}
		else
		{
			if( sz.cy < pControl->GetMinHeight() )
				sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() )
				sz.cy = pControl->GetMaxHeight();
		}
		cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
		nEstimateNum++;
	}
	cyFixed += (nEstimateNum - 1) * m_iChildPadding;

	// Place elements
	int cyNeeded = 0;
	int cyExpand = 0;
	if( nAdjustables > 0 )
		cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = rcTemp.top;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
	{
		iPosY -= m_pVerticalScrollBar->GetScrollPos();
	}
	int iPosX = rcTemp.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
	{
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	}
	int iAdjustable = 0;
	int cyFixedRemaining = cyFixed;
	for( int it2 = 0; it2 < m_items.GetSize(); it2++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if( !pControl->IsVisible() )
			continue;
		if( pControl->IsFloat() )
		{
			SetFloatPos(it2);
			continue;
		}

		RECT rcPadding = pControl->GetPadding();
		szRemaining.cy -= rcPadding.top;
		SIZE sz = pControl->EstimateSize(szRemaining);
		if( sz.cy == 0 )
		{
			iAdjustable++;
			sz.cy = cyExpand;
			// Distribute remaining to last element (usually round-off left-overs)
			if( iAdjustable == nAdjustables )
			{
				sz.cy = max(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
			} 
			if( sz.cy < pControl->GetMinHeight() )
				sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() )
				sz.cy = pControl->GetMaxHeight();
		}
		else
		{
			if( sz.cy < pControl->GetMinHeight() )
				sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() )
				sz.cy = pControl->GetMaxHeight();
			cyFixedRemaining -= sz.cy;
		}

		sz.cx = pControl->GetFixedWidth();
		if( sz.cx == 0 )
			sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
		if( sz.cx < 0 )
			sz.cx = 0;
		if( sz.cx < pControl->GetMinWidth() )
			sz.cx = pControl->GetMinWidth();
		if( sz.cx > pControl->GetMaxWidth() )
			sz.cx = pControl->GetMaxWidth();

		RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
		pControl->SetPosition(&rcCtrl);

		iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
		cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
		szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
	}
	cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

	// Process the scrollbar
	ProcessScrollBar(rcTemp, 0, cyNeeded);
}

bool CVerticalLayoutUI::EventHandler(TEventUI& event)
{
	if( m_iSepHeight != 0 ) {
		if( event.dwType == UIEVENT_LBUTTONDOWN && IsEnabled() )
		{
			RECT rcSeparator = GetThumbRect(false);
			if( ::PtInRect(&rcSeparator, event.ptMouse) )
			{
				m_dwState |= UISTATE_Captured;
				m_ptLastMouse = event.ptMouse;
				m_rcNewPos = m_rcControl;
				if( !m_bImmMode && m_pManager )
					m_pManager->AddPostPaint(this);
				return true;
			}
		}
		if( event.dwType == UIEVENT_LBUTTONUP )
		{
			if( (m_dwState & UISTATE_Captured) != 0 )
			{
				m_dwState &= ~UISTATE_Captured;
				m_rcControl = m_rcNewPos;
				if( !m_bImmMode && m_pManager )
					m_pManager->RemovePostPaint(this);
				NeedParentUpdate();
				return true;
			}
		}
		if( event.dwType == UIEVENT_MOUSEMOVE )
		{
			if( (m_dwState & UISTATE_Captured) != 0 )
			{
				LONG cy = event.ptMouse.y - m_ptLastMouse.y;
				m_ptLastMouse = event.ptMouse;
				RECT rc = m_rcNewPos;
				if( m_iSepHeight >= 0 )
				{
					if( cy > 0 && event.ptMouse.y < m_rcNewPos.bottom + m_iSepHeight )
						return true;
					if( cy < 0 && event.ptMouse.y > m_rcNewPos.bottom )
						return true;
					rc.bottom += cy;
					if( rc.bottom - rc.top <= GetMinHeight() )
					{
						if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() )
							return true;
						rc.bottom = rc.top + GetMinHeight();
					}
					if( rc.bottom - rc.top >= GetMaxHeight() )
					{
						if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() )
							return true;
						rc.bottom = rc.top + GetMaxHeight();
					}
				}
				else
				{
					if( cy > 0 && event.ptMouse.y < m_rcNewPos.top )
						return true;
					if( cy < 0 && event.ptMouse.y > m_rcNewPos.top + m_iSepHeight )
						return true;
					rc.top += cy;
					if( rc.bottom - rc.top <= GetMinHeight() )
					{
						if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() )
							return true;
						rc.top = rc.bottom - GetMinHeight();
					}
					if( rc.bottom - rc.top >= GetMaxHeight() )
					{
						if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() )
							return true;
						rc.top = rc.bottom - GetMaxHeight();
					}
				}

				CDuiRect rcInvalidate = GetThumbRect(true);
				m_rcNewPos = rc;
				m_cxyFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

				if( m_bImmMode )
				{
					m_rcControl = m_rcNewPos;
					NeedParentUpdate();
				}
				else
				{
					rcInvalidate.Join(GetThumbRect(true));
					rcInvalidate.Join(GetThumbRect(false));
					if( m_pManager ) m_pManager->Invalidate(rcInvalidate);
				}
				return true;
			}
		}
		if( event.dwType == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = GetThumbRect(false);
			if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) )
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
				return true;
			}
		}
	}
	return CContainerUI::EventHandler(event);
}

void CVerticalLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcscmp(lpszName, _T("sepheight")) == 0 )
		SetSepHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("sepimm")) == 0 )
		SetSepImmMode(_tcscmp(lpszValue, _T("true")) == 0);
	else
		CContainerUI::SetAttribute(lpszName, lpszValue);
}

RECT CVerticalLayoutUI::GetThumbRect(bool bUseNew /*= false*/) const
{
	if( (m_dwState & UISTATE_Captured) != 0 && bUseNew)
	{
		if( m_iSepHeight >= 0 ) 
			return CDuiRect(m_rcNewPos.left,
			max(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
			m_rcNewPos.right,
			m_rcNewPos.bottom);
		else 
			return CDuiRect(m_rcNewPos.left,
			m_rcNewPos.top,
			m_rcNewPos.right, 
			min(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
	}
	else
	{
		if( m_iSepHeight >= 0 ) 
			return CDuiRect(m_rcControl.left,
			max(m_rcControl.bottom - m_iSepHeight, m_rcControl.top),
			m_rcControl.right, 
			m_rcControl.bottom);
		else 
			return CDuiRect(m_rcControl.left,
			m_rcControl.top,
			m_rcControl.right, 
			min(m_rcControl.top - m_iSepHeight, m_rcControl.bottom));

	}
}
