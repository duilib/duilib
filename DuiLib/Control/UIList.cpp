#include "StdAfx.h"

namespace DuiLib {


/////////////////////////////////////////////////////////////////////////////////////
//

class CListBodyUI : public CVerticalLayoutUI
{
public:
    CListBodyUI(CListUI* pOwner);

    void SetScrollPos(SIZE szPos);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoEvent(TEventUI& event);
    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int& iCurSel);

protected:
    static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
    int __cdecl ItemComareFunc(const void *item1, const void *item2);

protected:
    CListUI* m_pOwner;
    PULVCompareFunc m_pCompareFunc;
    UINT_PTR m_compareData;
};

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListUI::CListUI() : m_pCallback(NULL), m_bScrollSelect(false), m_iCurSel(-1), m_iExpandedItem(-1)
{
    m_pList = new CListBodyUI(this);
    m_pHeader = new CListHeaderUI;

    Add(m_pHeader);
    CVerticalLayoutUI::Add(m_pList);

    m_ListInfo.nColumns = 0;
    m_ListInfo.uFixedHeight = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
    m_ListInfo.dwTextColor = 0xFF000000;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.iHLineSize = 0;
    m_ListInfo.dwHLineColor = 0xFF3C3C3C;
    m_ListInfo.iVLineSize = 0;
    m_ListInfo.dwVLineColor = 0xFF3C3C3C;
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

LPCTSTR CListUI::GetClass() const
{
    return DUI_CTR_LIST;
}

UINT CListUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

LPVOID CListUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_LIST) == 0 ) return static_cast<CListUI*>(this);
    if( _tcscmp(pstrName, DUI_CTR_ILIST) == 0 ) return static_cast<IListUI*>(this);
    if( _tcscmp(pstrName, DUI_CTR_ILISTOWNER) == 0 ) return static_cast<IListOwnerUI*>(this);
    return CVerticalLayoutUI::GetInterface(pstrName);
}

CControlUI* CListUI::GetItemAt(int iIndex) const
{
    return m_pList->GetItemAt(iIndex);
}

int CListUI::GetItemIndex(CControlUI* pControl) const
{
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::GetItemIndex(pControl);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->GetItemIndex(pControl);

    return m_pList->GetItemIndex(pControl);
}

bool CListUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::SetItemIndex(pControl, iIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetItemIndex(pControl, iIndex);

    int iOrginIndex = m_pList->GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iIndex ) return true;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !m_pList->SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = m_pList->GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool CListUI::SetMultiItemIndex(CControlUI* pStartControl, int iCount, int iNewStartIndex)
{
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) return false;
    if( pStartControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);
    // We also need to recognize header sub-items
    if( _tcsstr(pStartControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex);

    int iStartIndex = GetItemIndex(pStartControl);
    if (iStartIndex == iNewStartIndex) return true;
    if (iStartIndex + iCount > GetCount()) return false;
    if (iNewStartIndex + iCount > GetCount()) return false;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !m_pList->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex) ) return false;
    int iMinIndex = min(iStartIndex, iNewStartIndex);
    int iMaxIndex = max(iStartIndex + iCount, iNewStartIndex + iCount);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = m_pList->GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

int CListUI::GetCount() const
{
    return m_pList->GetCount();
}

bool CListUI::Add(CControlUI* pControl)
{
    // Override the Add() method so we can add items specifically to
    // the intended widgets. Headers are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
        if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
            CVerticalLayoutUI::Remove(m_pHeader);
            m_pHeader = static_cast<CListHeaderUI*>(pControl);
        }
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return CVerticalLayoutUI::AddAt(pControl, 0);
    }
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) {
        bool ret = m_pHeader->Add(pControl);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    // The list items should know about us
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(GetCount());
    }
    return m_pList->Add(pControl);
}

bool CListUI::AddAt(CControlUI* pControl, int iIndex)
{
    // Override the AddAt() method so we can add items specifically to
    // the intended widgets. Headers and are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) {
        if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
            CVerticalLayoutUI::Remove(m_pHeader);
            m_pHeader = static_cast<CListHeaderUI*>(pControl);
        }
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return CVerticalLayoutUI::AddAt(pControl, 0);
    }
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) {
        bool ret = m_pHeader->AddAt(pControl, iIndex);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    if (!m_pList->AddAt(pControl, iIndex)) return false;

    // The list items should know about us
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
        CControlUI* p = m_pList->GetItemAt(i);
        pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
    return true;
}

bool CListUI::Remove(CControlUI* pControl, bool bDoNotDestroy)
{
    if( pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL ) return CVerticalLayoutUI::Remove(pControl, bDoNotDestroy);
    // We also need to recognize header sub-items
    if( _tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL ) return m_pHeader->Remove(pControl, bDoNotDestroy);

    int iIndex = m_pList->GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!m_pList->RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < m_pList->GetCount(); ++i) {
        CControlUI* p = m_pList->GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

bool CListUI::RemoveAt(int iIndex, bool bDoNotDestroy)
{
    if (!m_pList->RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < m_pList->GetCount(); ++i) {
        CControlUI* p = m_pList->GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) pListItem->SetIndex(i);
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

void CListUI::RemoveAll()
{
    m_iCurSel = -1;
    m_iExpandedItem = -1;
    m_pList->RemoveAll();
}

void CListUI::SetPos(RECT rc, bool bNeedInvalidate)
{
	if( m_pHeader != NULL ) { // 设置header各子元素x坐标,因为有些listitem的setpos需要用到(临时修复)
		int iLeft = rc.left + m_rcInset.left;
		int iRight = rc.right - m_rcInset.right;

		m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

		if( !m_pHeader->IsVisible() ) {
			for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
				static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			}
		}
		m_pHeader->SetPos(CDuiRect(iLeft, 0, iRight, 0), false);
		int iOffset = m_pList->GetScrollPos().cx;
		for( int i = 0; i < m_ListInfo.nColumns; i++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			RECT rcPos = pControl->GetPos();
			if( iOffset > 0 ) {
				rcPos.left -= iOffset;
				rcPos.right -= iOffset;
				pControl->SetPos(rcPos, false);
			}
			m_ListInfo.rcColumn[i] = pControl->GetPos();
		}
		if( !m_pHeader->IsVisible() ) {
			for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
				static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
			}
			m_pHeader->SetInternVisible(false);
		}
	}

	CVerticalLayoutUI::SetPos(rc, bNeedInvalidate);

	if( m_pHeader == NULL ) return;

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

	m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

	if( !m_pHeader->IsVisible() ) {
		for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
			static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
		}
		m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), false);
	}
	int iOffset = m_pList->GetScrollPos().cx;
	for( int i = 0; i < m_ListInfo.nColumns; i++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;

		RECT rcPos = pControl->GetPos();
		if( iOffset > 0 ) {
			rcPos.left -= iOffset;
			rcPos.right -= iOffset;
			pControl->SetPos(rcPos, false);
		}
		m_ListInfo.rcColumn[i] = pControl->GetPos();
	}
	if( !m_pHeader->IsVisible() ) {
		for( int it = m_pHeader->GetCount() - 1; it >= 0; it-- ) {
			static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
		}
		m_pHeader->SetInternVisible(false);
	}
}

void CListUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
	CVerticalLayoutUI::Move(szOffset, bNeedInvalidate);
	if( !m_pHeader->IsVisible() ) m_pHeader->Move(szOffset, false);
}

void CListUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CVerticalLayoutUI::DoEvent(event);
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

    if( event.Type == UIEVENT_KEYDOWN )
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            switch( event.chKey ) {
            case VK_UP:
                SelectItem(FindSelectable(m_iCurSel - 1, false), true);
            case VK_DOWN:
                SelectItem(FindSelectable(m_iCurSel + 1, true), true);
            case VK_PRIOR:
                PageUp();
            case VK_NEXT:
                PageDown();
            case VK_HOME:
                SelectItem(FindSelectable(0, false), true);
            case VK_END:
                SelectItem(FindSelectable(GetCount() - 1, true), true);
            case VK_RETURN:
                if( m_iCurSel != -1 ) GetItemAt(m_iCurSel)->Activate();
            }
            return;
        }
    }

    if( event.Type == UIEVENT_SCROLLWHEEL )
    {
        if (IsEnabled()) {
            switch( LOWORD(event.wParam) ) {
            case SB_LINEUP:
                if( m_bScrollSelect ) SelectItem(FindSelectable(m_iCurSel - 1, false), true);
                else LineUp();
                return;
            case SB_LINEDOWN:
                if( m_bScrollSelect ) SelectItem(FindSelectable(m_iCurSel + 1, true), true);
                else LineDown();
                return;
            }
        }
    }

    CVerticalLayoutUI::DoEvent(event);
}

CListHeaderUI* CListUI::GetHeader() const
{
    return m_pHeader;
}

CContainerUI* CListUI::GetList() const
{
    return m_pList;
}

bool CListUI::GetScrollSelect()
{
    return m_bScrollSelect;
}

void CListUI::SetScrollSelect(bool bScrollSelect)
{
    m_bScrollSelect = bScrollSelect;
}

int CListUI::GetCurSel() const
{
    return m_iCurSel;
}

bool CListUI::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if( iIndex == m_iCurSel ) return true;

    int iOldSel = m_iCurSel;
    // We should first unselect the currently selected item
    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = GetItemAt(m_iCurSel);
        if( pControl != NULL) {
            IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
            if( pListItem != NULL ) pListItem->Select(false, bTriggerEvent);
        }

        m_iCurSel = -1;
    }
    if( iIndex < 0 ) return false;

    CControlUI* pControl = GetItemAt(iIndex);
    if( pControl == NULL ) return false;
    if( !pControl->IsVisible() ) return false;
    if( !pControl->IsEnabled() ) return false;

    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem == NULL ) return false;
    m_iCurSel = iIndex;
    if( !pListItem->Select(true, bTriggerEvent) ) {
        m_iCurSel = -1;
        return false;
    }
    EnsureVisible(m_iCurSel);
    if( bTakeFocus ) pControl->SetFocus();
    if( m_pManager != NULL && bTriggerEvent ) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
    }

    return true;
}

TListInfoUI* CListUI::GetListInfo()
{
    return &m_ListInfo;
}

int CListUI::GetChildPadding() const
{
    return m_pList->GetChildPadding();
}

void CListUI::SetChildPadding(int iPadding)
{
    m_pList->SetChildPadding(iPadding);
}

UINT CListUI::GetItemFixedHeight()
{
    return m_ListInfo.uFixedHeight;
}

void CListUI::SetItemFixedHeight(UINT nHeight)
{
    m_ListInfo.uFixedHeight = nHeight;
    NeedUpdate();
}

int CListUI::GetItemFont(int index)
{
    return m_ListInfo.nFont;
}

void CListUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    NeedUpdate();
}

UINT CListUI::GetItemTextStyle()
{
    return m_ListInfo.uTextStyle;
}

void CListUI::SetItemTextStyle(UINT uStyle)
{
    m_ListInfo.uTextStyle = uStyle;
    NeedUpdate();
}

void CListUI::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    NeedUpdate();
}

RECT CListUI::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void CListUI::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetItemBkImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diBk.sDrawString == pStrImage && m_ListInfo.diBk.pImageInfo != NULL ) return;
	m_ListInfo.diBk.Clear();
	m_ListInfo.diBk.sDrawString = pStrImage;
	Invalidate();
}

bool CListUI::IsAlternateBk() const
{
	return m_ListInfo.bAlternateBk;
}

void CListUI::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
    Invalidate();
}

DWORD CListUI::GetItemTextColor() const
{
	return m_ListInfo.dwTextColor;
}

DWORD CListUI::GetItemBkColor() const
{
	return m_ListInfo.dwBkColor;
}

LPCTSTR CListUI::GetItemBkImage() const
{
    return m_ListInfo.diBk.sDrawString;
}

void CListUI::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetSelectedItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diSelected.sDrawString == pStrImage && m_ListInfo.diSelected.pImageInfo != NULL ) return;
	m_ListInfo.diSelected.Clear();
	m_ListInfo.diSelected.sDrawString = pStrImage;
	Invalidate();
}

DWORD CListUI::GetSelectedItemTextColor() const
{
	return m_ListInfo.dwSelectedTextColor;
}

DWORD CListUI::GetSelectedItemBkColor() const
{
	return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR CListUI::GetSelectedItemImage() const
{
	return m_ListInfo.diSelected.sDrawString;
}

void CListUI::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetHotItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diHot.sDrawString == pStrImage && m_ListInfo.diHot.pImageInfo != NULL ) return;
	m_ListInfo.diHot.Clear();
	m_ListInfo.diHot.sDrawString = pStrImage;
	Invalidate();
}

DWORD CListUI::GetHotItemTextColor() const
{
	return m_ListInfo.dwHotTextColor;
}
DWORD CListUI::GetHotItemBkColor() const
{
	return m_ListInfo.dwHotBkColor;
}

LPCTSTR CListUI::GetHotItemImage() const
{
	return m_ListInfo.diHot.sDrawString;
}

void CListUI::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetDisabledItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diDisabled.sDrawString == pStrImage && m_ListInfo.diDisabled.pImageInfo != NULL ) return;
	m_ListInfo.diDisabled.Clear();
	m_ListInfo.diDisabled.sDrawString = pStrImage;
	Invalidate();
}

DWORD CListUI::GetDisabledItemTextColor() const
{
	return m_ListInfo.dwDisabledTextColor;
}

DWORD CListUI::GetDisabledItemBkColor() const
{
	return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR CListUI::GetDisabledItemImage() const
{
	return m_ListInfo.diDisabled.sDrawString;
}

int CListUI::GetItemHLineSize() const
{
    return m_ListInfo.iHLineSize;
}

void CListUI::SetItemHLineSize(int iSize)
{
    m_ListInfo.iHLineSize = iSize;
    Invalidate();
}

DWORD CListUI::GetItemHLineColor() const
{
    return m_ListInfo.dwHLineColor;
}

void CListUI::SetItemHLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwHLineColor = dwLineColor;
    Invalidate();
}

int CListUI::GetItemVLineSize() const
{
    return m_ListInfo.iVLineSize;
}

void CListUI::SetItemVLineSize(int iSize)
{
    m_ListInfo.iVLineSize = iSize;
    Invalidate();
}

DWORD CListUI::GetItemVLineColor() const
{
    return m_ListInfo.dwVLineColor;
}

void CListUI::SetItemVLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwVLineColor = dwLineColor;
    Invalidate();
}

bool CListUI::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void CListUI::SetItemShowHtml(bool bShowHtml)
{
    if( m_ListInfo.bShowHtml == bShowHtml ) return;

    m_ListInfo.bShowHtml = bShowHtml;
    NeedUpdate();
}

void CListUI::SetMultiExpanding(bool bMultiExpandable)
{
    m_ListInfo.bMultiExpandable = bMultiExpandable;
}

bool CListUI::ExpandItem(int iIndex, bool bExpand /*= true*/)
{
    if( m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable) {
        CControlUI* pControl = GetItemAt(m_iExpandedItem);
        if( pControl != NULL ) {
            IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
            if( pItem != NULL ) pItem->Expand(false);
        }
        m_iExpandedItem = -1;
    }
    if( bExpand ) {
        CControlUI* pControl = GetItemAt(iIndex);
        if( pControl == NULL ) return false;
        if( !pControl->IsVisible() ) return false;
        IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if( pItem == NULL ) return false;
        m_iExpandedItem = iIndex;
        if( !pItem->Expand(true) ) {
            m_iExpandedItem = -1;
            return false;
        }
    }
    NeedUpdate();
    return true;
}

int CListUI::GetExpandedItem() const
{
    return m_iExpandedItem;
}

void CListUI::EnsureVisible(int iIndex)
{
    if( m_iCurSel < 0 ) return;
    RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pList->GetPos();
    RECT rcListInset = m_pList->GetInset();

    rcList.left += rcListInset.left;
    rcList.top += rcListInset.top;
    rcList.right -= rcListInset.right;
    rcList.bottom -= rcListInset.bottom;

    CScrollBarUI* pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

    int iPos = m_pList->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
    Scroll(0, dx);
}

void CListUI::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pList->GetScrollPos();
    m_pList->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}

void CListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("hidden")) != 0);
    else if( _tcscmp(pstrName, _T("headerbkimage")) == 0 ) GetHeader()->SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("multiexpanding")) == 0 ) SetMultiExpanding(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("itemheight")) == 0 ) m_ListInfo.uFixedHeight = _ttoi(pstrValue);
    else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
    else if( _tcscmp(pstrName, _T("itemalign")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("itemvalign")) == 0)
    {
        if (_tcsstr(pstrValue, _T("top")) != NULL) {
            m_ListInfo.uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_ListInfo.uTextStyle |= DT_TOP;
        }
        if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
            m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_ListInfo.uTextStyle |= DT_VCENTER;
        }
        if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
            m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_ListInfo.uTextStyle |= DT_BOTTOM;
        }
    }
    else if( _tcscmp(pstrName, _T("itemendellipsis")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
        else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
    }
    else if( _tcscmp(pstrName, _T("itemmultiline")) == 0 ) {
        if (_tcscmp(pstrValue, _T("true")) == 0) {
            m_ListInfo.uTextStyle &= ~DT_SINGLELINE;
            m_ListInfo.uTextStyle |= DT_WORDBREAK;
        }
        else m_ListInfo.uTextStyle |= DT_SINGLELINE;
    }
    else if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetItemTextPadding(rcTextPadding);
    }
    else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemTextColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
    else if( _tcscmp(pstrName, _T("itemvlinesize")) == 0 ) {
        SetItemVLineSize(_ttoi(pstrValue));
    }
    else if( _tcscmp(pstrName, _T("itemvlinecolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemVLineColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemhlinesize")) == 0 ) {
        SetItemHLineSize(_ttoi(pstrValue));
    }
    else if( _tcscmp(pstrName, _T("itemhlinecolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemHLineColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    else CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
}

IListCallbackUI* CListUI::GetTextCallback() const
{
    return m_pCallback;
}

void CListUI::SetTextCallback(IListCallbackUI* pCallback)
{
    m_pCallback = pCallback;
}

SIZE CListUI::GetScrollPos() const
{
    return m_pList->GetScrollPos();
}

SIZE CListUI::GetScrollRange() const
{
    return m_pList->GetScrollRange();
}

void CListUI::SetScrollPos(SIZE szPos)
{
    m_pList->SetScrollPos(szPos);
}

void CListUI::LineUp()
{
    m_pList->LineUp();
}

void CListUI::LineDown()
{
    m_pList->LineDown();
}

void CListUI::PageUp()
{
    m_pList->PageUp();
}

void CListUI::PageDown()
{
    m_pList->PageDown();
}

void CListUI::HomeUp()
{
    m_pList->HomeUp();
}

void CListUI::EndDown()
{
    m_pList->EndDown();
}

void CListUI::LineLeft()
{
    m_pList->LineLeft();
}

void CListUI::LineRight()
{
    m_pList->LineRight();
}

void CListUI::PageLeft()
{
    m_pList->PageLeft();
}

void CListUI::PageRight()
{
    m_pList->PageRight();
}

void CListUI::HomeLeft()
{
    m_pList->HomeLeft();
}

void CListUI::EndRight()
{
    m_pList->EndRight();
}

void CListUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
    m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
}

CScrollBarUI* CListUI::GetVerticalScrollBar() const
{
    return m_pList->GetVerticalScrollBar();
}

CScrollBarUI* CListUI::GetHorizontalScrollBar() const
{
    return m_pList->GetHorizontalScrollBar();
}

bool CListUI::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
{
	if (!m_pList) return false;
	int iCurSel = m_iCurSel;
	bool bResult = m_pList->SortItems(pfnCompare, dwData, iCurSel);
	if (bResult) {
		m_iCurSel = iCurSel;
		EnsureVisible(m_iCurSel);
		NeedUpdate();
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListBodyUI::CListBodyUI(CListUI* pOwner) : m_pOwner(pOwner)
{
    ASSERT(m_pOwner);
}

bool CListBodyUI::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int& iCurSel)
{
	if (!pfnCompare) return false;
	m_pCompareFunc = pfnCompare;
	m_compareData = dwData;
	CControlUI *pCurSelControl = GetItemAt(iCurSel);
	CControlUI **pData = (CControlUI **)m_items.GetData();
	qsort_s(m_items.GetData(), m_items.GetSize(), sizeof(CControlUI*), CListBodyUI::ItemComareFunc, this);
	if (pCurSelControl) iCurSel = GetItemIndex(pCurSelControl);
	IListItemUI *pItem = NULL;
	for (int i = 0; i < m_items.GetSize(); ++i)
	{
		pItem = (IListItemUI*)(static_cast<CControlUI*>(m_items[i])->GetInterface(TEXT("ListItem")));
		if (pItem)
		{
			pItem->SetIndex(i);
		}
	}

	return true;
}

int __cdecl CListBodyUI::ItemComareFunc(void *pvlocale, const void *item1, const void *item2)
{
	CListBodyUI *pThis = (CListBodyUI*)pvlocale;
	if (!pThis || !item1 || !item2)
		return 0;
	return pThis->ItemComareFunc(item1, item2);
}

int __cdecl CListBodyUI::ItemComareFunc(const void *item1, const void *item2)
{
	CControlUI *pControl1 = *(CControlUI**)item1;
	CControlUI *pControl2 = *(CControlUI**)item2;
	return m_pCompareFunc((UINT_PTR)pControl1, (UINT_PTR)pControl2, m_compareData);
}

void CListBodyUI::SetScrollPos(SIZE szPos)
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

    if( cx != 0 && m_pOwner ) {
        CListHeaderUI* pHeader = m_pOwner->GetHeader();
        if( pHeader == NULL ) return;
        TListInfoUI* pInfo = m_pOwner->GetListInfo();
        pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);
        for( int i = 0; i < pInfo->nColumns; i++ ) {
            CControlUI* pControl = static_cast<CControlUI*>(pHeader->GetItemAt(i));
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
			pControl->Move(CDuiSize(-cx, -cy), false);
			pInfo->rcColumn[i] = pControl->GetPos();
        }
		pHeader->Invalidate();
    }
}

void CListBodyUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    // Adjust for inset
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    // Determine the minimum size
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
        szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

    int iChildPadding = m_iChildPadding;
    TListInfoUI* pInfo = NULL;
    if( m_pOwner ) {
        pInfo = m_pOwner->GetListInfo();
        if( pInfo != NULL ) {
            iChildPadding += pInfo->iHLineSize;
            if (pInfo->nColumns > 0) {
                szAvailable.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
            }
        }
    }

    int cxNeeded = 0;
    int cyFixed = 0;
    int nEstimateNum = 0;
    SIZE szControlAvailable;
    int iControlMaxWidth = 0;
    int iControlMaxHeight = 0;
    for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) continue;
        szControlAvailable = szAvailable;
        RECT rcPadding = pControl->GetPadding();
        szControlAvailable.cx -= rcPadding.left + rcPadding.right;
        iControlMaxWidth = pControl->GetFixedWidth();
        iControlMaxHeight = pControl->GetFixedHeight();
        if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
        if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
        if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
        if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
        SIZE sz = pControl->EstimateSize(szAvailable);
        if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
        if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
        cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

        sz.cx = MAX(sz.cx, 0);
        if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
        if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
        cxNeeded = MAX(cxNeeded, sz.cx);
        nEstimateNum++;
    }
    cyFixed += (nEstimateNum - 1) * iChildPadding;

    if( m_pOwner ) {
        CListHeaderUI* pHeader = m_pOwner->GetHeader();
        if( pHeader != NULL && pHeader->GetCount() > 0 ) {
            cxNeeded = MAX(0, pHeader->EstimateSize(CDuiSize(rc.right - rc.left, rc.bottom - rc.top)).cx);
        }
    }

    // Place elements
    int cyNeeded = 0;
    // Position the elements
    SIZE szRemaining = szAvailable;
    int iPosY = rc.top;
    if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
        iPosY -= m_pVerticalScrollBar->GetScrollPos();
    }
    int iPosX = rc.left;
    if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
        iPosX -= m_pHorizontalScrollBar->GetScrollPos();
    }
    
    int iAdjustable = 0;
    for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
        if( !pControl->IsVisible() ) continue;
        if( pControl->IsFloat() ) {
            SetFloatPos(it2);
            continue;
        }

        RECT rcPadding = pControl->GetPadding();
        szRemaining.cy -= rcPadding.top;
        szControlAvailable = szRemaining;
        szControlAvailable.cx -= rcPadding.left + rcPadding.right;
        iControlMaxWidth = pControl->GetFixedWidth();
        iControlMaxHeight = pControl->GetFixedHeight();
        if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
        if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
        if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
        if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
        SIZE sz = pControl->EstimateSize(szControlAvailable);
        if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
        if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
        sz.cx = pControl->GetMaxWidth();
        if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
        if( sz.cx < 0 ) sz.cx = 0;
        if( sz.cx > szControlAvailable.cx ) sz.cx = szControlAvailable.cx;
        if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();

        RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
        pControl->SetPos(rcCtrl, false);

        iPosY += sz.cy + iChildPadding + rcPadding.top + rcPadding.bottom;
        cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
        szRemaining.cy -= sz.cy + iChildPadding + rcPadding.bottom;
    }
    cyNeeded += (nEstimateNum - 1) * iChildPadding;

    // Process the scrollbar
    ProcessScrollBar(rc, cxNeeded, cyNeeded);
}

void CListBodyUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CControlUI::DoEvent(event);
        return;
    }

    if( m_pOwner != NULL ) {
		if (event.Type == UIEVENT_SCROLLWHEEL) {
			if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled()) {
				RECT rcHorizontalScrollBar = m_pHorizontalScrollBar->GetPos();
				if( ::PtInRect(&rcHorizontalScrollBar, event.ptMouse) ) 
				{
					switch( LOWORD(event.wParam) ) {
					case SB_LINEUP:
						m_pOwner->LineLeft();
						return;
					case SB_LINEDOWN:
						m_pOwner->LineRight();
						return;
					}
				}
			}
		}
		m_pOwner->DoEvent(event); }
	else {
		CControlUI::DoEvent(event);
	}
}

bool CListBodyUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    RECT rcTemp = { 0 };
    if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return true;

    TListInfoUI* pListInfo = NULL;
    if( m_pOwner ) pListInfo = m_pOwner->GetListInfo();

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    CControlUI::DoPaint(hDC, rcPaint, pStopControl);

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
                if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                }
            }
        }
        else {
            int iDrawIndex = 0;
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if( pControl == pStopControl ) return false;
                if( !pControl->IsVisible() ) continue;
                if( !pControl->IsFloat() ) {
                    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
                    if( pListItem != NULL ) {
                        pListItem->SetDrawIndex(iDrawIndex);
                        iDrawIndex += 1;
                    }
                    if (pListInfo && pListInfo->iHLineSize > 0) {
                        // 因为没有为最后一个预留分割条长度，如果list铺满，最后一条不会显示
                        RECT rcPadding = pControl->GetPadding();
                        const RECT& rcPos = pControl->GetPos();
                        RECT rcBottomLine = { rcPos.left, rcPos.bottom + rcPadding.bottom, rcPos.right, rcPos.bottom + rcPadding.bottom + pListInfo->iHLineSize };
                        if( ::IntersectRect(&rcTemp, &rcPaint, &rcBottomLine) ) {
                            rcBottomLine.top += pListInfo->iHLineSize / 2;
                            rcBottomLine.bottom = rcBottomLine.top;
                            CRenderEngine::DrawLine(hDC, rcBottomLine, pListInfo->iHLineSize, GetAdjustColor(pListInfo->dwHLineColor));
                        }
                    }
                }
                if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                if( pControl->IsFloat() ) {
                    if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else {
                    if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                    if( !pControl->Paint(hDC, rcPaint, pStopControl) ) return false;
                }
            }
        }
    }

    if( m_pVerticalScrollBar != NULL ) {
        if( m_pVerticalScrollBar == pStopControl ) return false;
        if (m_pVerticalScrollBar->IsVisible()) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
                if( !m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
            }
        }
    }

    if( m_pHorizontalScrollBar != NULL ) {
        if( m_pHorizontalScrollBar == pStopControl ) return false;
        if (m_pHorizontalScrollBar->IsVisible()) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
                if( !m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl) ) return false;
            }
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListHeaderUI::CListHeaderUI()
{
}

LPCTSTR CListHeaderUI::GetClass() const
{
    return DUI_CTR_LISTHEADER;
}

LPVOID CListHeaderUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0 ) return this;
    return CHorizontalLayoutUI::GetInterface(pstrName);
}

SIZE CListHeaderUI::EstimateSize(SIZE szAvailable)
{
    SIZE cXY = {0, m_cxyFixed.cy};
	if( cXY.cy == 0 && m_pManager != NULL ) {
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			cXY.cy = MAX(cXY.cy,static_cast<CControlUI*>(m_items[it])->EstimateSize(szAvailable).cy);
		}
		int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
		cXY.cy = MAX(cXY.cy,nMin);
	}

    for( int it = 0; it < m_items.GetSize(); it++ ) {
        cXY.cx +=  static_cast<CControlUI*>(m_items[it])->EstimateSize(szAvailable).cx;
    }

    return cXY;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListHeaderItemUI::CListHeaderItemUI() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
m_uTextStyle(DT_CENTER | DT_VCENTER | DT_SINGLELINE), m_dwTextColor(0), m_dwSepColor(0), 
m_iFont(-1), m_bShowHtml(false)
{
	SetTextPadding(CDuiRect(2, 0, 2, 0));
    ptLastMouse.x = ptLastMouse.y = 0;
    SetMinWidth(16);
}

LPCTSTR CListHeaderItemUI::GetClass() const
{
    return DUI_CTR_LISTHEADERITEM;
}

LPVOID CListHeaderItemUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTHEADERITEM) == 0 ) return this;
    return CControlUI::GetInterface(pstrName);
}

UINT CListHeaderItemUI::GetControlFlags() const
{
    if( IsEnabled() && m_iSepWidth != 0 ) return UIFLAG_SETCURSOR;
    else return 0;
}

void CListHeaderItemUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        m_uButtonState = 0;
    }
}

bool CListHeaderItemUI::IsDragable() const
{
	return m_bDragable;
}

void CListHeaderItemUI::SetDragable(bool bDragable)
{
    m_bDragable = bDragable;
    if ( !m_bDragable ) m_uButtonState &= ~UISTATE_CAPTURED;
}

DWORD CListHeaderItemUI::GetSepWidth() const
{
	return m_iSepWidth;
}

void CListHeaderItemUI::SetSepWidth(int iWidth)
{
    m_iSepWidth = iWidth;
}

DWORD CListHeaderItemUI::GetTextStyle() const
{
	return m_uTextStyle;
}

void CListHeaderItemUI::SetTextStyle(UINT uStyle)
{
    m_uTextStyle = uStyle;
    Invalidate();
}

DWORD CListHeaderItemUI::GetTextColor() const
{
	return m_dwTextColor;
}


void CListHeaderItemUI::SetTextColor(DWORD dwTextColor)
{
    m_dwTextColor = dwTextColor;
    Invalidate();
}

DWORD CListHeaderItemUI::GetSepColor() const
{
    return m_dwSepColor;
}

void CListHeaderItemUI::SetSepColor(DWORD dwSepColor)
{
    m_dwSepColor = dwSepColor;
    Invalidate();
}

RECT CListHeaderItemUI::GetTextPadding() const
{
	return m_rcTextPadding;
}

void CListHeaderItemUI::SetTextPadding(RECT rc)
{
	m_rcTextPadding = rc;
	Invalidate();
}

void CListHeaderItemUI::SetFont(int index)
{
    m_iFont = index;
}

bool CListHeaderItemUI::IsShowHtml()
{
    return m_bShowHtml;
}

void CListHeaderItemUI::SetShowHtml(bool bShowHtml)
{
    if( m_bShowHtml == bShowHtml ) return;

    m_bShowHtml = bShowHtml;
    Invalidate();
}

LPCTSTR CListHeaderItemUI::GetNormalImage() const
{
	return m_diNormal.sDrawString;
}

void CListHeaderItemUI::SetNormalImage(LPCTSTR pStrImage)
{
	if( m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL ) return;
	m_diNormal.Clear();
	m_diNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CListHeaderItemUI::GetHotImage() const
{
	return m_diHot.sDrawString;
}

void CListHeaderItemUI::SetHotImage(LPCTSTR pStrImage)
{
	if( m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL ) return;
	m_diHot.Clear();
	m_diHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CListHeaderItemUI::GetPushedImage() const
{
	return m_diPushed.sDrawString;
}

void CListHeaderItemUI::SetPushedImage(LPCTSTR pStrImage)
{
	if( m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL ) return;
	m_diPushed.Clear();
	m_diPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CListHeaderItemUI::GetFocusedImage() const
{
	return m_diFocused.sDrawString;
}

void CListHeaderItemUI::SetFocusedImage(LPCTSTR pStrImage)
{
	if( m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL ) return;
	m_diFocused.Clear();
	m_diFocused.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CListHeaderItemUI::GetSepImage() const
{
	return m_diSep.sDrawString;
}

void CListHeaderItemUI::SetSepImage(LPCTSTR pStrImage)
{
	if( m_diSep.sDrawString == pStrImage && m_diSep.pImageInfo != NULL ) return;
	m_diSep.Clear();
	m_diSep.sDrawString = pStrImage;
	Invalidate();
}

void CListHeaderItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("dragable")) == 0 ) SetDragable(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("align")) == 0 ) {
        if( _tcsstr(pstrValue, _T("left")) != NULL ) {
            m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_uTextStyle |= DT_LEFT;
        }
        if( _tcsstr(pstrValue, _T("center")) != NULL ) {
            m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_uTextStyle |= DT_CENTER;
        }
        if( _tcsstr(pstrValue, _T("right")) != NULL ) {
            m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("valign")) == 0)
    {
        if (_tcsstr(pstrValue, _T("top")) != NULL) {
            m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_uTextStyle |= DT_TOP;
        }
        if (_tcsstr(pstrValue, _T("vcenter")) != NULL) {
            m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_uTextStyle |= DT_VCENTER;
        }
        if (_tcsstr(pstrValue, _T("bottom")) != NULL) {
            m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_uTextStyle |= DT_BOTTOM;
        }
    }
    else if( _tcscmp(pstrName, _T("endellipsis")) == 0 ) {
        if( _tcscmp(pstrValue, _T("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
        else m_uTextStyle &= ~DT_END_ELLIPSIS;
    }  
    else if( _tcscmp(pstrName, _T("multiline")) == 0 ) {
        if (_tcscmp(pstrValue, _T("true")) == 0) {
            m_uTextStyle  &= ~DT_SINGLELINE;
            m_uTextStyle |= DT_WORDBREAK;
        }
        else m_uTextStyle |= DT_SINGLELINE;
    }
    else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetTextColor(clrColor);
    }
	else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetTextPadding(rcTextPadding);
	}
    else if( _tcscmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
    else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
    else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("sepcolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSepColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("sepimage")) == 0 ) SetSepImage(pstrValue);
    else CControlUI::SetAttribute(pstrName, pstrValue);
}

void CListHeaderItemUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CControlUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
    {
        if( !IsEnabled() ) return;
        RECT rcSeparator = GetThumbRect();
        if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
            if( m_bDragable ) {
                m_uButtonState |= UISTATE_CAPTURED;
                ptLastMouse = event.ptMouse;
            }
        }
        else {
            m_uButtonState |= UISTATE_PUSHED;
            m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~UISTATE_CAPTURED;
            if( GetParent() ) 
                GetParent()->NeedParentUpdate();
        }
        else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
            m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            RECT rc = m_rcItem;
            if( m_iSepWidth >= 0 ) {
                rc.right -= ptLastMouse.x - event.ptMouse.x;
            }
            else {
                rc.left -= ptLastMouse.x - event.ptMouse.x;
            }
            
            if( rc.right - rc.left > GetMinWidth() ) {
                m_cxyFixed.cx = rc.right - rc.left;
                ptLastMouse = event.ptMouse;
                if( GetParent() ) 
                    GetParent()->NeedParentUpdate();
            }
        }
        return;
    }
    if( event.Type == UIEVENT_SETCURSOR )
    {
        RECT rcSeparator = GetThumbRect();
        if( IsEnabled() && m_bDragable && ::PtInRect(&rcSeparator, event.ptMouse) ) {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
            return;
        }
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) == 0  ) {
                    m_uButtonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( !::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) != 0  ) {
                    m_uButtonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }
            if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
        }
        else {
            if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
            return;
        }
    }
    CControlUI::DoEvent(event);
}

SIZE CListHeaderItemUI::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8);
    return CControlUI::EstimateSize(szAvailable);
}

RECT CListHeaderItemUI::GetThumbRect() const
{
    if( m_iSepWidth >= 0 ) return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
    else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
}

void CListHeaderItemUI::PaintStatusImage(HDC hDC)
{
	if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
	else m_uButtonState &= ~ UISTATE_FOCUSED;

	if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
		if( !DrawImage(hDC, m_diPushed) )  DrawImage(hDC, m_diNormal);
	}
	else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !DrawImage(hDC, m_diHot) )  DrawImage(hDC, m_diNormal);
	}
	else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
		if( !DrawImage(hDC, m_diFocused) )  DrawImage(hDC, m_diNormal);
	}
	else {
		DrawImage(hDC, m_diNormal);
	}

    if (m_iSepWidth > 0) {
        RECT rcThumb = GetThumbRect();
        m_diSep.rcDestOffset.left = rcThumb.left - m_rcItem.left;
        m_diSep.rcDestOffset.top = rcThumb.top - m_rcItem.top;
        m_diSep.rcDestOffset.right = rcThumb.right - m_rcItem.left;
        m_diSep.rcDestOffset.bottom = rcThumb.bottom - m_rcItem.top;
        if( !DrawImage(hDC, m_diSep) ) {
            if (m_dwSepColor != 0) {
                RECT rcSepLine = { rcThumb.left + m_iSepWidth/2, rcThumb.top, rcThumb.left + m_iSepWidth/2, rcThumb.bottom};
                CRenderEngine::DrawLine(hDC, rcSepLine, m_iSepWidth, GetAdjustColor(m_dwSepColor));
            }
        }
    }
}

void CListHeaderItemUI::PaintText(HDC hDC)
{
    if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();

	RECT rcText = m_rcItem;
	rcText.left += m_rcTextPadding.left;
	rcText.top += m_rcTextPadding.top;
	rcText.right -= m_rcTextPadding.right;
	rcText.bottom -= m_rcTextPadding.bottom;

    if( m_sText.IsEmpty() ) return;
    int nLinks = 0;
    if( m_bShowHtml )
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
        NULL, NULL, nLinks, m_iFont, m_uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, m_dwTextColor, \
        m_iFont, m_uTextStyle);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListElementUI::CListElementUI() : 
m_iIndex(-1),
m_iDrawIndex(0),
m_pOwner(NULL), 
m_bSelected(false),
m_uButtonState(0)
{
}

LPCTSTR CListElementUI::GetClass() const
{
    return DUI_CTR_LISTELEMENT;
}

UINT CListElementUI::GetControlFlags() const
{
    return UIFLAG_WANTRETURN;
}

LPVOID CListElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0 ) return static_cast<IListItemUI*>(this);
	if( _tcscmp(pstrName, DUI_CTR_LISTELEMENT) == 0 ) return static_cast<CListElementUI*>(this);
    return CControlUI::GetInterface(pstrName);
}

IListOwnerUI* CListElementUI::GetOwner()
{
    return m_pOwner;
}

void CListElementUI::SetOwner(CControlUI* pOwner)
{
    if (pOwner != NULL) m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(DUI_CTR_ILISTOWNER));
}

void CListElementUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);
    if( !IsVisible() && m_bSelected)
    {
        m_bSelected = false;
        if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
    }
}

void CListElementUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        m_uButtonState = 0;
    }
}

int CListElementUI::GetIndex() const
{
    return m_iIndex;
}

void CListElementUI::SetIndex(int iIndex)
{
    m_iIndex = iIndex;
}

int CListElementUI::GetDrawIndex() const
{
    return m_iDrawIndex;
}

void CListElementUI::SetDrawIndex(int iIndex)
{
    m_iDrawIndex = iIndex;
}

void CListElementUI::Invalidate()
{
    if( !IsVisible() ) return;

    if( GetParent() ) {
        CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(DUI_CTR_CONTAINER));
        if( pParentContainer ) {
            RECT rc = pParentContainer->GetPos();
            RECT rcInset = pParentContainer->GetInset();
            rc.left += rcInset.left;
            rc.top += rcInset.top;
            rc.right -= rcInset.right;
            rc.bottom -= rcInset.bottom;
            CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
            if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
            CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
            if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

            RECT invalidateRc = m_rcItem;
            if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
            {
                return;
            }

            CControlUI* pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;
            while( pParent = pParent->GetParent() )
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();
                if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
                {
                    return;
                }
            }

            if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
        }
        else {
            CControlUI::Invalidate();
        }
    }
    else {
        CControlUI::Invalidate();
    }
}

bool CListElementUI::Activate()
{
    if( !CControlUI::Activate() ) return false;
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
    return true;
}

bool CListElementUI::IsSelected() const
{
    return m_bSelected;
}

bool CListElementUI::Select(bool bSelect, bool bTriggerEvent)
{
    if( !IsEnabled() ) return false;
    if( bSelect == m_bSelected ) return true;
    m_bSelected = bSelect;
    if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex, bTriggerEvent);
    Invalidate();

    return true;
}

bool CListElementUI::IsExpanded() const
{
    return false;
}

bool CListElementUI::Expand(bool /*bExpand = true*/)
{
    return false;
}

void CListElementUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CControlUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_DBLCLICK )
    {
        if( IsEnabled() ) {
            Activate();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            if( event.chKey == VK_RETURN ) {
                Activate();
                Invalidate();
                return;
            }
        }
    }
    // An important twist: The list-item will send the event not to its immediate
    // parent but to the "attached" list. A list may actually embed several components
    // in its path to the item, but key-presses etc. needs to go to the actual list.
    if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
}

void CListElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
    else CControlUI::SetAttribute(pstrName, pstrValue);
}

void CListElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if( pInfo == NULL ) return;
    DWORD iBackColor = 0;
    if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iBackColor = pInfo->dwHotBkColor;
    }
    if( IsSelected() ) {
        iBackColor = pInfo->dwSelectedBkColor;
    }
    if( !IsEnabled() ) {
        iBackColor = pInfo->dwDisabledBkColor;
    }

    if ( iBackColor != 0 ) {
        CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(iBackColor));
    }

    if( !IsEnabled() ) {
        if( DrawImage(hDC, pInfo->diDisabled) ) return;
    }
    if( IsSelected() ) {
        if( DrawImage(hDC, pInfo->diSelected) ) return;
    }
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if( DrawImage(hDC, pInfo->diHot) ) return;
    }

    if( !DrawImage(hDC, m_diBk) ) {
        if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) {
            if( DrawImage(hDC, pInfo->diBk) ) return;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListLabelElementUI::CListLabelElementUI() : m_bNeedEstimateSize(true), m_uFixedHeightLast(0),
    m_nFontLast(-1), m_uTextStyleLast(0)
{
    m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
    m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
    ::ZeroMemory(&m_rcTextPaddingLast, sizeof(m_rcTextPaddingLast));
}

LPCTSTR CListLabelElementUI::GetClass() const
{
    return DUI_CTR_LISTLABELELEMENT;
}

LPVOID CListLabelElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTLABELELEMENT) == 0 ) return static_cast<CListLabelElementUI*>(this);
    return CListElementUI::GetInterface(pstrName);
}

void CListLabelElementUI::SetOwner(CControlUI* pOwner)
{
    m_bNeedEstimateSize = true;
    CListElementUI::SetOwner(pOwner);
}

void CListLabelElementUI::SetFixedWidth(int cx)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetFixedWidth(cx);
}

void CListLabelElementUI::SetFixedHeight(int cy)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetFixedHeight(cy);
}

void CListLabelElementUI::SetText(LPCTSTR pstrText)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetText(pstrText);
}

void CListLabelElementUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CListElementUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
            Select();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE ) 
    {
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) == 0  ) {
                    m_uButtonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( !::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) != 0  ) {
                    m_uButtonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }
            if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
        }
        else {
            if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
            return;
        }
    }
    CListElementUI::DoEvent(event);
}

SIZE CListLabelElementUI::EstimateSize(SIZE szAvailable)
{
    if( m_pOwner == NULL ) return CDuiSize(0, 0);
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return CDuiSize(0, 0);
    if (m_cxyFixed.cx > 0) {
        if (m_cxyFixed.cy > 0) return m_cxyFixed;
        else if (pInfo->uFixedHeight > 0) return CDuiSize(m_cxyFixed.cx, pInfo->uFixedHeight);
    }

    if ((pInfo->uTextStyle & DT_SINGLELINE) == 0 && 
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) {
            m_bNeedEstimateSize = true;
    }
    if (m_uFixedHeightLast != pInfo->uFixedHeight || m_nFontLast != pInfo->nFont ||
        m_uTextStyleLast != pInfo->uTextStyle ||
        m_rcTextPaddingLast.left != pInfo->rcTextPadding.left || m_rcTextPaddingLast.right != pInfo->rcTextPadding.right ||
        m_rcTextPaddingLast.top != pInfo->rcTextPadding.top || m_rcTextPaddingLast.bottom != pInfo->rcTextPadding.bottom) {
            m_bNeedEstimateSize = true;
    }

    if (m_bNeedEstimateSize) {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_uFixedHeightLast = pInfo->uFixedHeight;
        m_nFontLast = pInfo->nFont;
        m_uTextStyleLast = pInfo->uTextStyle;
        m_rcTextPaddingLast = pInfo->rcTextPadding;
        
        m_cxyFixedLast = m_cxyFixed;
        if (m_cxyFixedLast.cy == 0) {
            m_cxyFixedLast.cy = pInfo->uFixedHeight;
        }

        if ((pInfo->uTextStyle & DT_SINGLELINE) != 0) {
            if( m_cxyFixedLast.cy == 0 ) {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
            }
            if (m_cxyFixedLast.cx == 0) {
                RECT rcText = { 0, 0, 9999, m_cxyFixedLast.cy };
                if( pInfo->bShowHtml ) {
                    int nLinks = 0;
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                m_cxyFixedLast.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;
            }
        }
        else {
            if( m_cxyFixedLast.cx == 0 ) {
                m_cxyFixedLast.cx = szAvailable.cx;
            }
            RECT rcText = { 0, 0, m_cxyFixedLast.cx, 9999 };
            rcText.left += pInfo->rcTextPadding.left;
            rcText.right -= pInfo->rcTextPadding.right;
            if( pInfo->bShowHtml ) {
                int nLinks = 0;
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            m_cxyFixedLast.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
        }
    }
    return m_cxyFixedLast;
}

bool CListLabelElementUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    DrawItemBk(hDC, m_rcItem);
    DrawItemText(hDC, m_rcItem);
    return true;
}

void CListLabelElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
{
    if( m_sText.IsEmpty() ) return;

    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if( pInfo == NULL ) return;
    DWORD iTextColor = pInfo->dwTextColor;
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if( !IsEnabled() ) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    int nLinks = 0;
    RECT rcText = rcItem;
    rcText.left += pInfo->rcTextPadding.left;
    rcText.right -= pInfo->rcTextPadding.right;
    rcText.top += pInfo->rcTextPadding.top;
    rcText.bottom -= pInfo->rcTextPadding.bottom;

    if( pInfo->bShowHtml )
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
        NULL, NULL, nLinks, pInfo->nFont, pInfo->uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
        pInfo->nFont, pInfo->uTextStyle);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CListTextElementUI::CListTextElementUI() : m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL)
{
    ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
}

CListTextElementUI::~CListTextElementUI()
{
    CDuiString* pText;
    for( int it = 0; it < m_aTexts.GetSize(); it++ ) {
        pText = static_cast<CDuiString*>(m_aTexts[it]);
        if( pText ) delete pText;
    }
    m_aTexts.Empty();
}

LPCTSTR CListTextElementUI::GetClass() const
{
    return DUI_CTR_LISTTEXTELEMENT;
}

LPVOID CListTextElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTTEXTELEMENT) == 0 ) return static_cast<CListTextElementUI*>(this);
    return CListLabelElementUI::GetInterface(pstrName);
}

UINT CListTextElementUI::GetControlFlags() const
{
    return UIFLAG_WANTRETURN | ( (IsEnabled() && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
}

LPCTSTR CListTextElementUI::GetText(int iIndex) const
{
    CDuiString* pText = static_cast<CDuiString*>(m_aTexts.GetAt(iIndex));
    if( pText ) return pText->GetData();
    return NULL;
}

void CListTextElementUI::SetText(int iIndex, LPCTSTR pstrText)
{
    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
    m_bNeedEstimateSize = true;
    
    while( m_aTexts.GetSize() < pInfo->nColumns ) { m_aTexts.Add(NULL); }

    CDuiString* pText = static_cast<CDuiString*>(m_aTexts[iIndex]);
    if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

	if ( pText ) //by cddjr 2011/10/20
		pText->Assign(pstrText);
	else
		m_aTexts.SetAt(iIndex, new CDuiString(pstrText));
    Invalidate();
}

void CListTextElementUI::SetOwner(CControlUI* pOwner)
{
    if (pOwner != NULL) {
        m_bNeedEstimateSize = true;
        CListElementUI::SetOwner(pOwner);
        m_pOwner = static_cast<IListUI*>(pOwner->GetInterface(DUI_CTR_ILIST));
    }
}

CDuiString* CListTextElementUI::GetLinkContent(int iIndex)
{
    if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
    return NULL;
}

void CListTextElementUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CListLabelElementUI::DoEvent(event);
        return;
    }

    // When you hover over a link
    if( event.Type == UIEVENT_SETCURSOR ) {
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
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
    if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) {
        int nHoverLink = -1;
        for( int i = 0; i < m_nLinks; i++ ) {
            if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
                nHoverLink = i;
                break;
            }
        }

        if(m_nHoverLink != nHoverLink) {
            Invalidate();
            m_nHoverLink = nHoverLink;
        }
    }
    if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) {
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
    CListLabelElementUI::DoEvent(event);
}

SIZE CListTextElementUI::EstimateSize(SIZE szAvailable)
{
    if( m_pOwner == NULL ) return CDuiSize(0, 0);
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return CDuiSize(0, 0);
    SIZE cxyFixed = m_cxyFixed;
    if (cxyFixed.cx == 0 && pInfo->nColumns > 0) {
        cxyFixed.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
        if (m_cxyFixedLast.cx != cxyFixed.cx) m_bNeedEstimateSize = true;
    }
    if (cxyFixed.cx > 0) {
        if (cxyFixed.cy > 0) return cxyFixed;
        else if (pInfo->uFixedHeight > 0) return CDuiSize(cxyFixed.cx, pInfo->uFixedHeight);
    }
    
    if ((pInfo->uTextStyle & DT_SINGLELINE) == 0 && 
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy)) {
            m_bNeedEstimateSize = true;
    }
    if (m_uFixedHeightLast != pInfo->uFixedHeight || m_nFontLast != pInfo->nFont ||
        m_uTextStyleLast != pInfo->uTextStyle ||
        m_rcTextPaddingLast.left != pInfo->rcTextPadding.left || m_rcTextPaddingLast.right != pInfo->rcTextPadding.right ||
        m_rcTextPaddingLast.top != pInfo->rcTextPadding.top || m_rcTextPaddingLast.bottom != pInfo->rcTextPadding.bottom) {
            m_bNeedEstimateSize = true;
    }

    CDuiString strText;
    IListCallbackUI* pCallback = m_pOwner->GetTextCallback();
    if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, 0);
    else if (m_aTexts.GetSize() > 0) strText.Assign(GetText(0));
    else strText = m_sText;
    if (m_sTextLast != strText) m_bNeedEstimateSize = true;

    if (m_bNeedEstimateSize) {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_uFixedHeightLast = pInfo->uFixedHeight;
        m_nFontLast = pInfo->nFont;
        m_uTextStyleLast = pInfo->uTextStyle;
        m_rcTextPaddingLast = pInfo->rcTextPadding;
        m_sTextLast = strText;

        m_cxyFixedLast = m_cxyFixed;
        if (m_cxyFixedLast.cx == 0 && pInfo->nColumns > 0) {
            m_cxyFixedLast.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
        }
        if (m_cxyFixedLast.cy == 0) {
            m_cxyFixedLast.cy = pInfo->uFixedHeight;
        }

        if ((pInfo->uTextStyle & DT_SINGLELINE) != 0) {
            if( m_cxyFixedLast.cy == 0 ) {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
            }
            if (m_cxyFixedLast.cx == 0) {
                RECT rcText = { 0, 0, 9999, m_cxyFixedLast.cy };
                if( pInfo->bShowHtml ) {
                    int nLinks = 0;
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                m_cxyFixedLast.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;
            }
        }
        else {
            if( m_cxyFixedLast.cx == 0 ) {
                m_cxyFixedLast.cx = szAvailable.cx;
            }
            RECT rcText = { 0, 0, m_cxyFixedLast.cx, 9999 };
            rcText.left += pInfo->rcTextPadding.left;
            rcText.right -= pInfo->rcTextPadding.right;
            if( pInfo->bShowHtml ) {
                int nLinks = 0;
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            m_cxyFixedLast.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
        }
    }
    return m_cxyFixedLast;
}

void CListTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
{
    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return;
    DWORD iTextColor = pInfo->dwTextColor;

    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if( !IsEnabled() ) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    IListCallbackUI* pCallback = m_pOwner->GetTextCallback();

    m_nLinks = 0;
    int nLinks = lengthof(m_rcLinks);
    if (pInfo->nColumns > 0) {
        for( int i = 0; i < pInfo->nColumns; i++ )
        {
            RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
            if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1) {
                RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
                CRenderEngine::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
                rcItem.right -= pInfo->iVLineSize;
            }

            rcItem.left += pInfo->rcTextPadding.left;
            rcItem.right -= pInfo->rcTextPadding.right;
            rcItem.top += pInfo->rcTextPadding.top;
            rcItem.bottom -= pInfo->rcTextPadding.bottom;

            CDuiString strText;//不使用LPCTSTR，否则限制太多 by cddjr 2011/10/20
            if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
            else strText.Assign(GetText(i));
            if( pInfo->bShowHtml )
                CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
                &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, pInfo->uTextStyle);
            else
                CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
                pInfo->nFont, pInfo->uTextStyle);

            m_nLinks += nLinks;
            nLinks = lengthof(m_rcLinks) - m_nLinks; 
        }
    }
    else {
        RECT rcItem = m_rcItem;
        rcItem.left += pInfo->rcTextPadding.left;
        rcItem.right -= pInfo->rcTextPadding.right;
        rcItem.top += pInfo->rcTextPadding.top;
        rcItem.bottom -= pInfo->rcTextPadding.bottom;

        CDuiString strText;
        if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, 0);
        else if (m_aTexts.GetSize() > 0) strText.Assign(GetText(0));
        else strText = m_sText;
        if( pInfo->bShowHtml )
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
            &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, pInfo->nFont, pInfo->uTextStyle);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
            pInfo->nFont, pInfo->uTextStyle);

        m_nLinks += nLinks;
        nLinks = lengthof(m_rcLinks) - m_nLinks; 
    }

    for( int i = m_nLinks; i < lengthof(m_rcLinks); i++ ) {
        ::ZeroMemory(m_rcLinks + i, sizeof(RECT));
        ((CDuiString*)(m_sLinks + i))->Empty();
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListContainerElementUI::CListContainerElementUI() : 
m_iIndex(-1),
m_iDrawIndex(0),
m_pOwner(NULL), 
m_bSelected(false),
m_bExpandable(false),
m_bExpand(false),
m_uButtonState(0)
{
}

LPCTSTR CListContainerElementUI::GetClass() const
{
    return DUI_CTR_LISTCONTAINERELEMENT;
}

UINT CListContainerElementUI::GetControlFlags() const
{
    return UIFLAG_WANTRETURN;
}

LPVOID CListContainerElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0 ) return static_cast<IListItemUI*>(this);
	if( _tcscmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0 ) return static_cast<CListContainerElementUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

IListOwnerUI* CListContainerElementUI::GetOwner()
{
    return m_pOwner;
}

void CListContainerElementUI::SetOwner(CControlUI* pOwner)
{
    if (pOwner != NULL) m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(DUI_CTR_ILISTOWNER));
}

void CListContainerElementUI::SetVisible(bool bVisible)
{
    CContainerUI::SetVisible(bVisible);
    if( !IsVisible() && m_bSelected)
    {
        m_bSelected = false;
        if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
    }
}

void CListContainerElementUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);
    if( !IsEnabled() ) {
        m_uButtonState = 0;
    }
}

int CListContainerElementUI::GetIndex() const
{
    return m_iIndex;
}

void CListContainerElementUI::SetIndex(int iIndex)
{
    m_iIndex = iIndex;
}

int CListContainerElementUI::GetDrawIndex() const
{
    return m_iDrawIndex;
}

void CListContainerElementUI::SetDrawIndex(int iIndex)
{
    m_iDrawIndex = iIndex;
}

void CListContainerElementUI::Invalidate()
{
    if( !IsVisible() ) return;

    if( GetParent() ) {
        CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(DUI_CTR_CONTAINER));
        if( pParentContainer ) {
            RECT rc = pParentContainer->GetPos();
            RECT rcInset = pParentContainer->GetInset();
            rc.left += rcInset.left;
            rc.top += rcInset.top;
            rc.right -= rcInset.right;
            rc.bottom -= rcInset.bottom;
            CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
            if( pVerticalScrollBar && pVerticalScrollBar->IsVisible() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
            CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
            if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

            RECT invalidateRc = m_rcItem;
            if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
            {
                return;
            }

            CControlUI* pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;
            while( pParent = pParent->GetParent() )
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();
                if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
                {
                    return;
                }
            }

            if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
        }
        else {
            CContainerUI::Invalidate();
        }
    }
    else {
        CContainerUI::Invalidate();
    }
}

bool CListContainerElementUI::Activate()
{
    if( !CContainerUI::Activate() ) return false;
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE);
    return true;
}

bool CListContainerElementUI::IsSelected() const
{
    return m_bSelected;
}

bool CListContainerElementUI::Select(bool bSelect, bool bTriggerEvent)
{
    if( !IsEnabled() ) return false;
    if( bSelect == m_bSelected ) return true;
    m_bSelected = bSelect;
    if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex, bTriggerEvent);
    Invalidate();

    return true;
}

bool CListContainerElementUI::IsExpandable() const
{
    return m_bExpandable;
}

void CListContainerElementUI::SetExpandable(bool bExpandable)
{
    m_bExpandable = bExpandable;
}

bool CListContainerElementUI::IsExpanded() const
{
    return m_bExpand;
}

bool CListContainerElementUI::Expand(bool bExpand)
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return false;  
    if( bExpand == m_bExpand ) return true;
    m_bExpand = bExpand;
    if( m_bExpandable ) {
        if( !m_pOwner->ExpandItem(m_iIndex, bExpand) ) return false;
        if( m_pManager != NULL ) {
            if( bExpand ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMEXPAND, false);
            else m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCOLLAPSE, false);
        }
    }

    return true;
}

void CListContainerElementUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
        else CContainerUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_DBLCLICK )
    {
        if( IsEnabled() ) {
            Activate();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            if( event.chKey == VK_RETURN ) {
                Activate();
                Invalidate();
                return;
            }
        }
    }
    if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
    {
        if( IsEnabled() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);
            Select();
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP ) 
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) == 0  ) {
                    m_uButtonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( !::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( IsEnabled() ) {
                if( (m_uButtonState & UISTATE_HOT) != 0  ) {
                    m_uButtonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }
            if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
        }
        else {
            if (m_pManager) m_pManager->AddMouseLeaveNeeded(this);
            return;
        }
    }

    // An important twist: The list-item will send the event not to its immediate
    // parent but to the "attached" list. A list may actually embed several components
    // in its path to the item, but key-presses etc. needs to go to the actual list.
    if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
}

void CListContainerElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
    else if( _tcscmp(pstrName, _T("expandable")) == 0 ) SetExpandable(_tcscmp(pstrValue, _T("true")) == 0);
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

bool CListContainerElementUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    DrawItemBk(hDC, m_rcItem);
    return CContainerUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CListContainerElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
{
    return;
}

void CListContainerElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
{
	ASSERT(m_pOwner);
	if( m_pOwner == NULL ) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();
	if( pInfo == NULL ) return;
	DWORD iBackColor = 0;
	if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;

	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		iBackColor = pInfo->dwHotBkColor;
	}
	if( IsSelected() ) {
		iBackColor = pInfo->dwSelectedBkColor;
	}
	if( !IsEnabled() ) {
		iBackColor = pInfo->dwDisabledBkColor;
	}
	if ( iBackColor != 0 ) {
		CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
	}

	if( !IsEnabled() ) {
		if( DrawImage(hDC, pInfo->diDisabled) ) return;
	}
	if( IsSelected() ) {
		if( DrawImage(hDC, pInfo->diSelected) ) return;
	}
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( DrawImage(hDC, pInfo->diHot) ) return;
	}
	if( !DrawImage(hDC, m_diBk) ) {
		if( !pInfo->bAlternateBk || m_iDrawIndex % 2 == 0 ) {
			if( DrawImage(hDC, pInfo->diBk) ) return;
		}
	}
}

SIZE CListContainerElementUI::EstimateSize(SIZE szAvailable)
{
    TListInfoUI* pInfo = NULL;
    if( m_pOwner ) pInfo = m_pOwner->GetListInfo();

    SIZE cXY = m_cxyFixed;

    if( cXY.cy == 0 ) {
        cXY.cy = pInfo->uFixedHeight;
    }

    return cXY;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListHBoxElementUI::CListHBoxElementUI()
{
    
}

LPCTSTR CListHBoxElementUI::GetClass() const
{
    return DUI_CTR_LISTHBOXELEMENT;
}

LPVOID CListHBoxElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_LISTHBOXELEMENT) == 0 ) return static_cast<CListHBoxElementUI*>(this);
    return CListContainerElementUI::GetInterface(pstrName);
}

void CListHBoxElementUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    if( m_pOwner == NULL ) return CListContainerElementUI::SetPos(rc, bNeedInvalidate);

    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if (pInfo == NULL) return;
    if (pInfo->nColumns > 0) {
        int iColumnIndex = 0;
        for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
            CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(it2);
                continue;
            }
            if( iColumnIndex >= pInfo->nColumns ) continue;

            RECT rcPadding = pControl->GetPadding();
            RECT rcItem = { pInfo->rcColumn[iColumnIndex].left + rcPadding.left, m_rcItem.top + rcPadding.top, 
                pInfo->rcColumn[iColumnIndex].right - rcPadding.right, m_rcItem.bottom - rcPadding.bottom };
            if (pInfo->iVLineSize > 0 && iColumnIndex < pInfo->nColumns - 1) {
                rcItem.right -= pInfo->iVLineSize;
            }
            pControl->SetPos(rcItem, false);
            iColumnIndex += 1;
        }
    }
    else {
        for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
            CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(it2);
                continue;
            }

            RECT rcPadding = pControl->GetPadding();
            RECT rcItem = { m_rcItem.left + rcPadding.left, m_rcItem.top + rcPadding.top, 
                m_rcItem.right - rcPadding.right, m_rcItem.bottom - rcPadding.bottom };
            pControl->SetPos(rcItem, false);
        }
    }
}

bool CListHBoxElementUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    ASSERT(m_pOwner);
    if( m_pOwner == NULL ) return true;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
    if( pInfo == NULL ) return true;

    DrawItemBk(hDC, m_rcItem);
    for( int i = 0; i < pInfo->nColumns; i++ ) {
        RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
        if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1) {
            RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
            CRenderEngine::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
        }
    }
    return CContainerUI::DoPaint(hDC, rcPaint, pStopControl);
}

} // namespace DuiLib
