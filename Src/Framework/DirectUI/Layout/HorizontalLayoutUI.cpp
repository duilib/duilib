#include "stdafx.h"
#include "HorizontalLayoutUI.h"

UI_IMPLEMENT_DYNCREATE(CHorizontalLayoutUI);

CHorizontalLayoutUI::CHorizontalLayoutUI(void)
	: m_iSepWidth(0)
	, m_bImmMode(false)
{
}


CHorizontalLayoutUI::~CHorizontalLayoutUI(void)
{
}

LPCTSTR CHorizontalLayoutUI::GetClass() const
{
	return _T("HorizontalLayout");
}

LPVOID CHorizontalLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("HorizontalLayout")) == 0 )
		return static_cast<CHorizontalLayoutUI*>(this);
	return
		CContainerUI::GetInterface(pstrName);
}

UINT CHorizontalLayoutUI::GetControlFlags() const
{
	if( IsEnabled() && m_iSepWidth != 0 )
		return UIFLAG_SETCURSOR;
	else
		return 0;
}

void CHorizontalLayoutUI::SetPosition(LPCRECT rc)
{
	CControlUI::SetPosition(rc);
	CDuiRect rcTemp(m_rcControl);

	// Adjust for inset
	rcTemp.left += m_rcInset.left;
	rcTemp.top += m_rcInset.top;
	rcTemp.right -= m_rcInset.right;
	rcTemp.bottom -= m_rcInset.bottom;

	if( m_items.GetSize() == 0)
	{
		ProcessScrollBar(rcTemp, 0, 0);
		return;
	}

	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
		rcTemp.right -= m_pVerticalScrollBar->GetFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		rcTemp.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	// Determine the width of elements that are sizeable
	SIZE szAvailable = { rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top };
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

	int nAdjustables = 0;
	int cxFixed = 0;
	int nEstimateNum = 0;
	for( int it1 = 0; it1 < m_items.GetSize(); it1++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if( !pControl->IsVisible() )
			continue;
		if( pControl->IsFloat() )
			continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if( sz.cx == 0 )
		{
			nAdjustables++;
		}
		else
		{
			if( sz.cx < pControl->GetMinWidth() )
				sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() )
				sz.cx = pControl->GetMaxWidth();
		}
		cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
		nEstimateNum++;
	}
	cxFixed += (nEstimateNum - 1) * m_iChildPadding;

	int cxExpand = 0;
	int cxNeeded = 0;
	if( nAdjustables > 0 )
		cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosX = rcTemp.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
	{
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	}
	int iAdjustable = 0;
	int cxFixedRemaining = cxFixed;
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
		szRemaining.cx -= rcPadding.left;
		SIZE sz = pControl->EstimateSize(szRemaining);
		if( sz.cx == 0 )
		{
			iAdjustable++;
			sz.cx = cxExpand;
			// Distribute remaining to last element (usually round-off left-overs)
			if( iAdjustable == nAdjustables )
			{
				sz.cx = max(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
			}
			if( sz.cx < pControl->GetMinWidth() )
				sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() )
				sz.cx = pControl->GetMaxWidth();
		}
		else {
			if( sz.cx < pControl->GetMinWidth() )
				sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() )
				sz.cx = pControl->GetMaxWidth();

			cxFixedRemaining -= sz.cx;
		}

		sz.cy = pControl->GetFixedHeight();
		if( sz.cy == 0 )
			sz.cy = rcTemp.bottom - rcTemp.top - rcPadding.top - rcPadding.bottom;
		if( sz.cy < 0 )
			sz.cy = 0;
		if( sz.cy < pControl->GetMinHeight() )
			sz.cy = pControl->GetMinHeight();
		if( sz.cy > pControl->GetMaxHeight() )
			sz.cy = pControl->GetMaxHeight();

		RECT rcCtrl = { iPosX + rcPadding.left, rcTemp.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rcTemp.top + rcPadding.top + sz.cy};
		pControl->SetPosition(&rcCtrl);
		iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
		cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
		szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
	}
	cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

	// Process the scrollbar
	ProcessScrollBar(rcTemp, cxNeeded, 0);
}

bool CHorizontalLayoutUI::EventHandler(TEventUI& event)
{
	if( m_iSepWidth != 0 )
	{
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
				LONG cx = event.ptMouse.x - m_ptLastMouse.x;
				m_ptLastMouse = event.ptMouse;
				RECT rc = m_rcNewPos;
				if( m_iSepWidth >= 0 )
				{
					if( cx > 0 && event.ptMouse.x < m_rcNewPos.right - m_iSepWidth )
						return true;
					if( cx < 0 && event.ptMouse.x > m_rcNewPos.right )
						return true;
					rc.right += cx;
					if( rc.right - rc.left <= GetMinWidth() )
					{
						if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() )
							return true;
						rc.right = rc.left + GetMinWidth();
					}
					if( rc.right - rc.left >= GetMaxWidth() )
					{
						if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() )
							return true;
						rc.right = rc.left + GetMaxWidth();
					}
				}
				else
				{
					if( cx > 0 && event.ptMouse.x < m_rcNewPos.left )
						return true;
					if( cx < 0 && event.ptMouse.x > m_rcNewPos.left - m_iSepWidth )
						return true;
					rc.left += cx;
					if( rc.right - rc.left <= GetMinWidth() )
					{
						if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() )
							return true;
						rc.left = rc.right - GetMinWidth();
					}
					if( rc.right - rc.left >= GetMaxWidth() )
					{
						if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() )
							return true;
						rc.left = rc.right - GetMaxWidth();
					}
				}

				CDuiRect rcInvalidate = GetThumbRect(true);
				m_rcNewPos = rc;
				m_cxyFixed.cx = m_rcNewPos.right - m_rcNewPos.left;

				if( m_bImmMode )
				{
					m_rcControl = m_rcNewPos;
					NeedParentUpdate();
				}
				else
				{
					rcInvalidate.Join(GetThumbRect(true));
					rcInvalidate.Join(GetThumbRect(false));
					if( m_pManager )
						m_pManager->Invalidate(rcInvalidate);
				}
				return true;
			}
		}
		if( event.dwType == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = GetThumbRect(false);
			if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) )
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return true;
			}
		}
	}
	return CContainerUI::EventHandler(event);
}

void CHorizontalLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcscmp(lpszName, _T("sepwidth")) == 0 )
		SetSepWidth(_ttoi(lpszValue));
	else
		if( _tcscmp(lpszName, _T("sepimm")) == 0 )
			SetSepImmMode(_tcscmp(lpszValue, _T("true")) == 0);
	else
		CContainerUI::SetAttribute(lpszName, lpszValue);
}

RECT CHorizontalLayoutUI::GetThumbRect(bool bUseNew /*= false*/) const
{
	if( ( m_dwState & UISTATE_Captured) != 0 && bUseNew)
	{
		if( m_iSepWidth >= 0 )
			return CDuiRect(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom);
		else
			return CDuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom);
	}
	else
	{
		if( m_iSepWidth >= 0 )
			return CDuiRect(m_rcControl.right - m_iSepWidth, m_rcControl.top, m_rcControl.right, m_rcControl.bottom);
		else return CDuiRect(m_rcControl.left, m_rcControl.top, m_rcControl.left - m_iSepWidth, m_rcControl.bottom);
	}
}

void CHorizontalLayoutUI::PostRender(IUIRender* pRender,LPCRECT pRcPaint)
{
	if( (m_dwState & UISTATE_Captured) != 0 && !m_bImmMode )
	{
		RECT rcSeparator = GetThumbRect(true);
		pRender->DrawColor(&rcSeparator, 0xAA000000);
	}
}

void CHorizontalLayoutUI::SetSepWidth(int iWidth)
{
	m_iSepWidth = iWidth;
}

int CHorizontalLayoutUI::GetSepWidth() const
{
	return m_iSepWidth;
}

void CHorizontalLayoutUI::SetSepImmMode(bool bImmediately)
{
	if( m_bImmMode == bImmediately )
		return;
	if( (m_dwState & UISTATE_Captured) != 0 && !m_bImmMode && m_pManager != NULL )
	{
		m_pManager->RemovePostPaint(this);
	}

	m_bImmMode = bImmediately;
}

bool CHorizontalLayoutUI::IsSepImmMode() const
{
	return m_bImmMode;
}
