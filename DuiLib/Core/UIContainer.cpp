#include "StdAfx.h"

namespace DuiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CContainerUI::CContainerUI()
		: m_iChildPadding(0),
		m_bAutoDestroy(true),
		m_bDelayedDestroy(true),
		m_bMouseChildEnabled(true),
		m_pVerticalScrollBar(NULL),
		m_pHorizontalScrollBar(NULL),
		m_bScrollProcess(false)
	{
		::ZeroMemory(&m_rcInset, sizeof(m_rcInset));
	}

	CContainerUI::~CContainerUI()
	{
		m_bDelayedDestroy = false;
		RemoveAll();
		if( m_pVerticalScrollBar ) delete m_pVerticalScrollBar;
		if( m_pHorizontalScrollBar ) delete m_pHorizontalScrollBar;
	}

	LPCTSTR CContainerUI::GetClass() const
	{
		return _T("ContainerUI");
	}

	LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("IContainer")) == 0 ) return static_cast<IContainerUI*>(this);
		else if( _tcscmp(pstrName, DUI_CTR_CONTAINER) == 0 ) return static_cast<CContainerUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	CControlUI* CContainerUI::GetItemAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
		return static_cast<CControlUI*>(m_items[iIndex]);
	}

	int CContainerUI::GetItemIndex(CControlUI* pControl) const
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				return it;
			}
		}

		return -1;
	}

	bool CContainerUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();            
				m_items.Remove(it);
				return m_items.InsertAt(iIndex, pControl);
			}
		}

		return false;
	}

	int CContainerUI::GetCount() const
	{
		return m_items.GetSize();
	}

	bool CContainerUI::Add(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.Add(pControl);   
	}

	bool CContainerUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if( pControl == NULL) return false;

		if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.InsertAt(iIndex, pControl);
	}

	bool CContainerUI::Remove(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();
				if( m_bAutoDestroy ) {
					if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(pControl);             
					else delete pControl;
				}
				return m_items.Remove(it);
			}
		}
		return false;
	}

	bool CContainerUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (pControl != NULL) {
			return CContainerUI::Remove(pControl);
		}

		return false;
	}

	void CContainerUI::RemoveAll()
	{
		for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
			if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(static_cast<CControlUI*>(m_items[it]));             
			else delete static_cast<CControlUI*>(m_items[it]);
		}
		m_items.Empty();
		NeedUpdate();
	}

	bool CContainerUI::IsAutoDestroy() const
	{
		return m_bAutoDestroy;
	}

	void CContainerUI::SetAutoDestroy(bool bAuto)
	{
		m_bAutoDestroy = bAuto;
	}

	bool CContainerUI::IsDelayedDestroy() const
	{
		return m_bDelayedDestroy;
	}

	void CContainerUI::SetDelayedDestroy(bool bDelayed)
	{
		m_bDelayedDestroy = bDelayed;
	}

	RECT CContainerUI::GetInset() const
	{
		return m_rcInset;
	}

	void CContainerUI::SetInset(RECT rcInset)
	{
		m_rcInset = rcInset;
		NeedUpdate();
	}

	int CContainerUI::GetChildPadding() const
	{
		return m_iChildPadding;
	}

	void CContainerUI::SetChildPadding(int iPadding)
	{
		m_iChildPadding = iPadding;
		NeedUpdate();
	}

	bool CContainerUI::IsMouseChildEnabled() const
	{
		return m_bMouseChildEnabled;
	}

	void CContainerUI::SetMouseChildEnabled(bool bEnable)
	{
		m_bMouseChildEnabled = bEnable;
	}

	void CContainerUI::SetVisible(bool bVisible)
	{
		if( m_bVisible == bVisible ) return;
		CControlUI::SetVisible(bVisible);
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	// �߼��ϣ�����Container�ؼ��������˷���
	// ���ô˷����Ľ���ǣ��ڲ��ӿؼ����أ��ؼ�������Ȼ��ʾ��������Ч������
	void CContainerUI::SetInternVisible(bool bVisible)
	{
		CControlUI::SetInternVisible(bVisible);
		if( m_items.IsEmpty() ) return;
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			// �����ӿؼ���ʾ״̬
			// InternVisible״̬Ӧ���ӿؼ��Լ�����
			static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	void CContainerUI::SetMouseEnabled(bool bEnabled)
	{
		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetMouseEnabled(bEnabled);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetMouseEnabled(bEnabled);
		CControlUI::SetMouseEnabled(bEnabled);
	}

	void CContainerUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused = true;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused = false;
			return;
		}
		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled() )
		{
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
			case VK_DOWN:
				LineDown();
				return;
			case VK_UP:
				LineUp();
				return;
			case VK_NEXT:
				PageDown();
				return;
			case VK_PRIOR:
				PageUp();
				return;
			case VK_HOME:
				HomeUp();
				return;
			case VK_END:
				EndDown();
				return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				LineUp();
				return;
			case SB_LINEDOWN:
				LineDown();
				return;
				}
			}
		}
		else if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled() ) {
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
			case VK_DOWN:
				LineRight();
				return;
			case VK_UP:
				LineLeft();
				return;
			case VK_NEXT:
				PageRight();
				return;
			case VK_PRIOR:
				PageLeft();
				return;
			case VK_HOME:
				HomeLeft();
				return;
			case VK_END:
				EndRight();
				return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				LineLeft();
				return;
			case SB_LINEDOWN:
				LineRight();
				return;
				}
			}
		}
		CControlUI::DoEvent(event);
	}

	SIZE CContainerUI::GetScrollPos() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollPos();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
		return sz;
	}

	SIZE CContainerUI::GetScrollRange() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) sz.cy = m_pVerticalScrollBar->GetScrollRange();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
		return sz;
	}

	void CContainerUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( cx == 0 && cy == 0 ) return;

		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			pControl->Move(CDuiSize(-cx, -cy), false);
		}

		Invalidate();
	}

	void CContainerUI::LineUp()
	{
		int cyLine = 8;
		if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

		SIZE sz = GetScrollPos();
		sz.cy -= cyLine;
		SetScrollPos(sz);
	}

	void CContainerUI::LineDown()
	{
		int cyLine = 8;
		if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

		SIZE sz = GetScrollPos();
		sz.cy += cyLine;
		SetScrollPos(sz);
	}

	void CContainerUI::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndDown()
	{
		SIZE sz = GetScrollPos();
		sz.cy = GetScrollRange().cy;
		SetScrollPos(sz);
	}

	void CContainerUI::LineLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx -= 8;
		SetScrollPos(sz);
	}

	void CContainerUI::LineRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx += 8;
		SetScrollPos(sz);
	}

	void CContainerUI::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx = GetScrollRange().cx;
		SetScrollPos(sz);
	}

	void CContainerUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if( bEnableVertical && !m_pVerticalScrollBar ) {
			m_pVerticalScrollBar = new CScrollBarUI;
			m_pVerticalScrollBar->SetOwner(this);
			m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);
			if ( m_pManager ) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
				if( pDefaultAttributes ) {
					m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableVertical && m_pVerticalScrollBar ) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = NULL;
		}

		if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
			m_pHorizontalScrollBar = new CScrollBarUI;
			m_pHorizontalScrollBar->SetHorizontal(true);
			m_pHorizontalScrollBar->SetOwner(this);
			m_pHorizontalScrollBar->SetManager(m_pManager, NULL, false);
			if ( m_pManager ) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
				if( pDefaultAttributes ) {
					m_pHorizontalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = NULL;
		}

		NeedUpdate();
	}

	CScrollBarUI* CContainerUI::GetVerticalScrollBar() const
	{
		return m_pVerticalScrollBar;
	}

	CScrollBarUI* CContainerUI::GetHorizontalScrollBar() const
	{
		return m_pHorizontalScrollBar;
	}

	int CContainerUI::FindSelectable(int iIndex, bool bForward /*= true*/) const
	{
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if( GetCount() == 0 ) return -1;
		iIndex = CLAMP(iIndex, 0, GetCount() - 1);
		if( bForward ) {
			for( int i = iIndex; i < GetCount(); i++ ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return -1;
		}
		else {
			for( int i = iIndex; i >= 0; --i ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return FindSelectable(0, true);
		}
	}

	void CContainerUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		if( m_items.IsEmpty() ) return;

		rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
			rc.top -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
			rc.bottom += m_pVerticalScrollBar->GetScrollRange();
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		}
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
			rc.left -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right -= m_pHorizontalScrollBar->GetScrollPos();
			rc.right += m_pHorizontalScrollBar->GetScrollRange();
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		}

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
			}
			else { 
				SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
				pControl->SetPos(rcCtrl, false);
			}
		}
	}

	void CContainerUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) m_pVerticalScrollBar->Move(szOffset, false);
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) m_pHorizontalScrollBar->Move(szOffset, false);
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( pControl != NULL && pControl->IsVisible() ) pControl->Move(szOffset, false);
		}
	}

	void CContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("inset")) == 0 ) {
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetInset(rcInset);
		}
		else if( _tcscmp(pstrName, _T("mousechild")) == 0 ) SetMouseChildEnabled(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) {
			EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
		}
		else if( _tcscmp(pstrName, _T("vscrollbarstyle")) == 0 ) {
			EnableScrollBar(true, GetHorizontalScrollBar() != NULL);
			if( GetVerticalScrollBar() ) GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
		}
		else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, _tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("hscrollbarstyle")) == 0 ) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, true);
			if( GetHorizontalScrollBar() ) GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
		}
		else if( _tcscmp(pstrName, _T("childpadding")) == 0 ) SetChildPadding(_ttoi(pstrValue));
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CContainerUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			static_cast<CControlUI*>(m_items[it])->SetManager(pManager, this, bInit);
		}

		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);
		CControlUI::SetManager(pManager, pParent, bInit);
	}

	CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		// Check if this guy is valid
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( (uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL ) return NULL;

		CControlUI* pResult = NULL;
		if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		if( pResult == NULL && m_pVerticalScrollBar != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
		}
		if( pResult == NULL && m_pHorizontalScrollBar != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
		}
		if( pResult != NULL ) return pResult;

		if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled() ) {
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
				for( int it = m_items.GetSize() - 1; it >= 0; it-- ) {
					pResult = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					}          
				}
			}
			else {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					pResult = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					} 
				}
			}
		}

		pResult = NULL;
		if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		return pResult;
	}

	void CContainerUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

		CRenderClip clip;
		CRenderClip::GenerateClip(hDC, rcTemp, clip);
		CControlUI::DoPaint(hDC, rcPaint);

		if( m_items.GetSize() > 0 ) {
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

			if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
			else {
				CRenderClip childClip;
				CRenderClip::GenerateClip(hDC, rcTemp, childClip);
				for( int it = 0; it < m_items.GetSize(); it++ ) {
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						CRenderClip::UseOldClipBegin(hDC, childClip);
						pControl->DoPaint(hDC, rcPaint);
						CRenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
				m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
			}
		}

		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
				m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
			}
		}
	}

	void CContainerUI::SetFloatPos(int iIndex)
	{
		// ��ΪCControlUI::SetPos��float�Ĳ���Ӱ�죬���ﲻ�ܶ�float�����ӹ�������Ӱ��
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);

		if( !pControl->IsVisible() ) return;
		if( !pControl->IsFloat() ) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
		TPercentInfo rcPercent = pControl->GetFloatPercent();
		LONG width = m_rcItem.right - m_rcItem.left;
		LONG height = m_rcItem.bottom - m_rcItem.top;
		RECT rcCtrl = { 0 };
		rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx;
		rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy;
		rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx;
		rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy;
		pControl->SetPos(rcCtrl, false);
	}

	void CContainerUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
			RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
			m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
		}

		if( m_pVerticalScrollBar == NULL ) return;

		if( cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible() ) {
			m_pVerticalScrollBar->SetVisible(true);
			m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
			m_pVerticalScrollBar->SetScrollPos(0);
			m_bScrollProcess = true;
			if( !IsFloat() ) SetPos(m_rcItem);
			else SetPos(GetRelativePos());
			m_bScrollProcess = false;
			return;
		}
		// No scrollbar required
		if( !m_pVerticalScrollBar->IsVisible() ) return;

		// Scroll not needed anymore?
		int cyScroll = cyRequired - (rc.bottom - rc.top);
		if( cyScroll <= 0 && !m_bScrollProcess) {
			m_pVerticalScrollBar->SetVisible(false);
			m_pVerticalScrollBar->SetScrollPos(0);
			m_pVerticalScrollBar->SetScrollRange(0);
			if( !IsFloat() ) SetPos(m_rcItem);
			else SetPos(GetRelativePos());
		}
		else
		{
			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
			m_pVerticalScrollBar->SetPos(rcScrollBarPos, false);

			if( m_pVerticalScrollBar->GetScrollRange() != cyScroll ) {
				int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
				m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));
				if( m_pVerticalScrollBar->GetScrollRange() == 0 ) {
					m_pVerticalScrollBar->SetVisible(false);
					m_pVerticalScrollBar->SetScrollPos(0);
				}
				if( iScrollPos > m_pVerticalScrollBar->GetScrollPos() ) {
					if( !IsFloat() ) SetPos(m_rcItem, false);
					else SetPos(GetRelativePos(), false);
				}
			}
		}
	}

	bool CContainerUI::SetSubControlText( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetText(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedHeight( LPCTSTR pstrSubControlName,int cy )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedHeight(cy);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedWdith( LPCTSTR pstrSubControlName,int cx )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedWidth(cx);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlUserData( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetUserData(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	CDuiString CContainerUI::GetSubControlText( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetText();
	}

	int CContainerUI::GetSubControlFixedHeight( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedHeight();
	}

	int CContainerUI::GetSubControlFixedWdith( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedWidth();
	}

	const CDuiString CContainerUI::GetSubControlUserData( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetUserData();
	}

	CControlUI* CContainerUI::FindSubControl( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=static_cast<CControlUI*>(GetManager()->FindSubControlByName(this,pstrSubControlName));
		return pSubControl;
	}

} // namespace DuiLib
