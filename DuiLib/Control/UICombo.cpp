#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

class CComboBodyUI : public CVerticalLayoutUI
{
public:
    CComboBodyUI::CComboBodyUI(CComboUI* pOwner);
    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

protected:
    CComboUI* m_pOwner;
};


CComboBodyUI::CComboBodyUI(CComboUI* pOwner) : m_pOwner(pOwner)
{
    ASSERT(m_pOwner);
}

bool CComboBodyUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
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

class CComboWnd : public CWindowWnd
{
public:
    void Init(CComboUI* pOwner);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
	virtual UINT GetClassStyle() const;
#endif

public:
    CPaintManagerUI m_pm;
    CComboUI* m_pOwner;
    CVerticalLayoutUI* m_pLayout;
    int m_iOldSel;
    bool m_bScrollbarClicked;
};


void CComboWnd::Init(CComboUI* pOwner)
{
    m_pOwner = pOwner;
    m_pLayout = NULL;
    m_iOldSel = m_pOwner->GetCurSel();
    m_bScrollbarClicked = false;

    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
    rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
    if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;
    for( int it = 0; it < pOwner->GetCount(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }
    cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
    if( rc.bottom > rcWork.bottom ) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    }
    
    Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOW);
    //::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CComboWnd::GetWindowClassName() const
{
    return _T("ComboWnd");
}

void CComboWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pWindow = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT CComboWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if( uMsg == WM_CREATE ) {
		m_pm.SetForceUseSharedRes(true);
        m_pm.Init(m_hWnd);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CComboBodyUI(m_pOwner);
        m_pLayout->SetManager(&m_pm, NULL, true);
        LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayout"));
        if( pDefaultAttributes ) {
            m_pLayout->SetAttributeList(pDefaultAttributes);
        }
        m_pLayout->SetInset(CDuiRect(1, 1, 1, 1));
        m_pLayout->SetBkColor(0xFFFFFFFF);
        m_pLayout->SetBorderColor(0xFFC6C7D2);
        m_pLayout->SetBorderSize(1);
        m_pLayout->SetAutoDestroy(false);
        m_pLayout->EnableScrollBar();
        m_pLayout->SetAttributeList(m_pOwner->GetDropBoxAttributeList());
        for( int i = 0; i < m_pOwner->GetCount(); i++ ) {
            m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
        }
        m_pm.AttachDialog(m_pLayout);
        
        return 0;
    }
    else if( uMsg == WM_CLOSE ) {
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
		if( !m_pOwner->IsFloat() ) m_pOwner->SetPos(m_pOwner->GetPos(), false);
		else m_pOwner->SetPos(m_pOwner->GetRelativePos(), false);
        m_pOwner->SetFocus();
    }
    else if( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
        CControlUI* pControl = m_pm.FindControl(pt);
        if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_SCROLLBAR) == 0 ) {
            m_bScrollbarClicked = true;
        }
    }
    else if( uMsg == WM_LBUTTONUP ) {
        if (m_bScrollbarClicked) {
            m_bScrollbarClicked = false;
        }
        else {
            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
            CControlUI* pControl = m_pm.FindControl(pt);
            if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_SCROLLBAR) != 0 ) PostMessage(WM_KILLFOCUS);
        }
    }
    else if( uMsg == WM_KEYDOWN ) {
        switch( wParam ) {
        case VK_ESCAPE:
            m_pOwner->SelectItem(m_iOldSel, true);
            EnsureVisible(m_iOldSel);
            // FALL THROUGH...
        case VK_RETURN:
            PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEventUI event;
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
            m_pOwner->DoEvent(event);
            EnsureVisible(m_pOwner->GetCurSel());
            return 0;
        }
    }
    else if( uMsg == WM_MOUSEWHEEL ) {
        int zDelta = (int) (short) HIWORD(wParam);
        TEventUI event = { 0 };
        event.Type = UIEVENT_SCROLLWHEEL;
        event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
        event.lParam = lParam;
        event.dwTimestamp = ::GetTickCount();
        m_pOwner->DoEvent(event);
        EnsureVisible(m_pOwner->GetCurSel());
        return 0;
    }
    else if( uMsg == WM_KILLFOCUS ) {
        if( m_hWnd != (HWND) wParam ) {
            HWND hWnd = ::GetFocus();
            HWND hParentWnd = NULL;
            bool bIsChildFocus = false;
            while( hParentWnd = ::GetParent(hWnd) ) {
                if( m_hWnd == hParentWnd ) {
                    bIsChildFocus = true;
                    break;
                }
                hWnd = hParentWnd;
            }
            if (!bIsChildFocus) {
                PostMessage(WM_CLOSE);
                return 0;
            }
        }
    }

    LRESULT lRes = 0;
    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CComboWnd::EnsureVisible(int iIndex)
{
    if( m_pOwner->GetCurSel() < 0 ) return;
    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
    CScrollBarUI* pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();
    if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
    int iPos = m_pLayout->GetScrollPos().cy;
    if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
    int dx = 0;
    if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
    if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
    Scroll(0, dx);
}

void CComboWnd::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}

#if(_WIN32_WINNT >= 0x0501)
UINT CComboWnd::GetClassStyle() const
{
	return __super::GetClassStyle() | CS_DROPSHADOW;
}
#endif
////////////////////////////////////////////////////////


CComboUI::CComboUI() : m_pWindow(NULL), m_iCurSel(-1), m_uButtonState(0)
{
    m_szDropBox = CDuiSize(0, 150);
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

    m_ListInfo.nColumns = 0;
    m_ListInfo.uFixedHeight = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE;
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

	m_bShowText = true;
	m_bSelectCloseFlag = true;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

LPCTSTR CComboUI::GetClass() const
{
    return DUI_CTR_COMBO;
}

LPVOID CComboUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_ILISTOWNER) == 0 ) return static_cast<IListOwnerUI*>(this);
	if( _tcscmp(pstrName, DUI_CTR_COMBO) == 0 ) return static_cast<CComboUI*>(this);
    return CContainerUI::GetInterface(pstrName);
}

UINT CComboUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

void CComboUI::DoInit()
{
}

int CComboUI::GetCurSel() const
{
    return m_iCurSel;
}

bool CComboUI::GetSelectCloseFlag()
{
	return m_bSelectCloseFlag;
}

void CComboUI::SetSelectCloseFlag(bool flag)
{
	m_bSelectCloseFlag = flag;
}

bool CComboUI::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if( m_bSelectCloseFlag && m_pWindow != NULL ) m_pWindow->Close();
    if( iIndex == m_iCurSel ) return true;
    int iOldSel = m_iCurSel;
    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        if( !pControl ) return false;
        IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) pListItem->Select(false, bTriggerEvent);
        m_iCurSel = -1;
    }
    if( iIndex < 0 ) return false;
    if( m_items.GetSize() == 0 ) return false;
    if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
    CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
    if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() ) return false;
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem == NULL ) return false;
    m_iCurSel = iIndex;
    if( m_pWindow != NULL || bTakeFocus ) pControl->SetFocus();
    pListItem->Select(true, bTriggerEvent);
    if( m_pManager != NULL && bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
    Invalidate();

    return true;
}

bool CComboUI::ExpandItem(int iIndex, bool bExpand)
{
    return false;
}

int CComboUI::GetExpandedItem() const
{
    return -1;
}

bool CComboUI::SetItemIndex(CControlUI* pControl, int iNewIndex)
{
    int iOrginIndex = GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iNewIndex ) return true;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !CContainerUI::SetItemIndex(pControl, iNewIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iNewIndex);
    int iMaxIndex = max(iOrginIndex, iNewIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool CComboUI::SetMultiItemIndex(CControlUI* pStartControl, int iCount, int iNewStartIndex)
{
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) return false;
    int iStartIndex = GetItemIndex(pStartControl);
    if (iStartIndex == iNewStartIndex) return true;
    if (iStartIndex + iCount > GetCount()) return false;
    if (iNewStartIndex + iCount > GetCount()) return false;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) pSelectedListItem = 
        static_cast<IListItemUI*>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));
    if( !CContainerUI::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex) ) return false;
    int iMinIndex = min(iStartIndex, iNewStartIndex);
    int iMaxIndex = max(iStartIndex + iCount, iNewStartIndex + iCount);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool CComboUI::Add(CControlUI* pControl)
{
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) 
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(m_items.GetSize());
    }
    return CContainerUI::Add(pControl);
}

bool CComboUI::AddAt(CControlUI* pControl, int iIndex)
{
    if (!CContainerUI::AddAt(pControl, iIndex)) return false;

    // The list items should know about us
    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
    if( pListItem != NULL ) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        pListItem = static_cast<IListItemUI*>(p->GetInterface(DUI_CTR_ILISTITEM));
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
    return true;
}

bool CComboUI::Remove(CControlUI* pControl, bool bDoNotDestroy)
{
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!CContainerUI::RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
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

bool CComboUI::RemoveAt(int iIndex, bool bDoNotDestroy)
{
    if (!CContainerUI::RemoveAt(iIndex, bDoNotDestroy)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
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

void CComboUI::RemoveAll()
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll();
}

void CComboUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CContainerUI::DoEvent(event);
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
    if( event.Type == UIEVENT_BUTTONDOWN )
    {
        if( IsEnabled() ) {
            Activate();
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~ UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        if (IsKeyboardEnabled() && IsEnabled()) {
            switch( event.chKey ) {
            case VK_F4:
                Activate();
            case VK_UP:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel - 1, false));
                SetSelectCloseFlag(true);
            case VK_DOWN:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel + 1, true));
                SetSelectCloseFlag(true);
            case VK_PRIOR:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel - 1, false));
                SetSelectCloseFlag(true);
            case VK_NEXT:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(m_iCurSel + 1, true));
                SetSelectCloseFlag(true);
            case VK_HOME:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(0, false));
                SetSelectCloseFlag(true);
            case VK_END:
                SetSelectCloseFlag(false);
                SelectItem(FindSelectable(GetCount() - 1, true));
                SetSelectCloseFlag(true);
            }
            return;
        }
    }
    if( event.Type == UIEVENT_SCROLLWHEEL )
    {
        if (IsEnabled()) {
            bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
            SetSelectCloseFlag(false);
            SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
            SetSelectCloseFlag(true);
            return;
        }
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
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
    CControlUI::DoEvent(event);
}

SIZE CComboUI::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8);
    return CControlUI::EstimateSize(szAvailable);
}

bool CComboUI::Activate()
{
    if( !CControlUI::Activate() ) return false;
    if( m_pWindow ) return true;
    m_pWindow = new CComboWnd();
    ASSERT(m_pWindow);
    m_pWindow->Init(this);
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_DROPDOWN);
    Invalidate();
    return true;
}

CDuiString CComboUI::GetText() const
{
    if( m_iCurSel < 0 ) return _T("");
    CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
    return pControl->GetText();
}

void CComboUI::SetEnabled(bool bEnable)
{
    CContainerUI::SetEnabled(bEnable);
    if( !IsEnabled() ) m_uButtonState = 0;
}

CDuiString CComboUI::GetDropBoxAttributeList()
{
    return m_sDropBoxAttributes;
}

void CComboUI::SetDropBoxAttributeList(LPCTSTR pstrList)
{
    m_sDropBoxAttributes = pstrList;
}

SIZE CComboUI::GetDropBoxSize() const
{
    return m_szDropBox;
}

void CComboUI::SetDropBoxSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
}

bool CComboUI::GetShowText() const
{
	return m_bShowText;
}

void CComboUI::SetShowText(bool flag)
{
	m_bShowText = flag;
	Invalidate();
}

RECT CComboUI::GetTextPadding() const
{
    return m_rcTextPadding;
}

void CComboUI::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
}

LPCTSTR CComboUI::GetNormalImage() const
{
    return m_diNormal.sDrawString;
}

void CComboUI::SetNormalImage(LPCTSTR pStrImage)
{
	if( m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL ) return;
	m_diNormal.Clear();
	m_diNormal.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CComboUI::GetHotImage() const
{
    return m_diHot.sDrawString;
}

void CComboUI::SetHotImage(LPCTSTR pStrImage)
{
	if( m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL ) return;
	m_diHot.Clear();
	m_diHot.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CComboUI::GetPushedImage() const
{
    return m_diPushed.sDrawString;
}

void CComboUI::SetPushedImage(LPCTSTR pStrImage)
{
	if( m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL ) return;
	m_diPushed.Clear();
	m_diPushed.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CComboUI::GetFocusedImage() const
{
	return m_diFocused.sDrawString;
}

void CComboUI::SetFocusedImage(LPCTSTR pStrImage)
{
	if( m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL ) return;
	m_diFocused.Clear();
	m_diFocused.sDrawString = pStrImage;
	Invalidate();
}

LPCTSTR CComboUI::GetDisabledImage() const
{
	return m_diDisabled.sDrawString;
}

void CComboUI::SetDisabledImage(LPCTSTR pStrImage)
{
	if( m_diDisabled.sDrawString == pStrImage && m_diDisabled.pImageInfo != NULL ) return;
	m_diDisabled.Clear();
	m_diDisabled.sDrawString = pStrImage;
	Invalidate();
}

TListInfoUI* CComboUI::GetListInfo()
{
    return &m_ListInfo;
}

UINT CComboUI::GetItemFixedHeight()
{
    return m_ListInfo.uFixedHeight;
}

void CComboUI::SetItemFixedHeight(UINT nHeight)
{
    m_ListInfo.uFixedHeight = nHeight;
    Invalidate();
}

int CComboUI::GetItemFont(int index)
{
    return m_ListInfo.nFont;
}

void CComboUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    Invalidate();
}

UINT CComboUI::GetItemTextStyle()
{
    return m_ListInfo.uTextStyle;
}

void CComboUI::SetItemTextStyle(UINT uStyle)
{
	m_ListInfo.uTextStyle = uStyle;
	Invalidate();
}

RECT CComboUI::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void CComboUI::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    Invalidate();
}

void CComboUI::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void CComboUI::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
}

void CComboUI::SetItemBkImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diBk.sDrawString == pStrImage && m_ListInfo.diBk.pImageInfo != NULL ) return;
	m_ListInfo.diBk.Clear();
	m_ListInfo.diBk.sDrawString = pStrImage;
}

DWORD CComboUI::GetItemTextColor() const
{
	return m_ListInfo.dwTextColor;
}

DWORD CComboUI::GetItemBkColor() const
{
	return m_ListInfo.dwBkColor;
}

LPCTSTR CComboUI::GetItemBkImage() const
{
	return m_ListInfo.diBk.sDrawString;
}

bool CComboUI::IsAlternateBk() const
{
    return m_ListInfo.bAlternateBk;
}

void CComboUI::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
}

void CComboUI::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
}

void CComboUI::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
}

void CComboUI::SetSelectedItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diSelected.sDrawString == pStrImage && m_ListInfo.diSelected.pImageInfo != NULL ) return;
	m_ListInfo.diSelected.Clear();
	m_ListInfo.diSelected.sDrawString = pStrImage;
}

DWORD CComboUI::GetSelectedItemTextColor() const
{
	return m_ListInfo.dwSelectedTextColor;
}

DWORD CComboUI::GetSelectedItemBkColor() const
{
	return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR CComboUI::GetSelectedItemImage() const
{
	return m_ListInfo.diSelected.sDrawString;
}

void CComboUI::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
}

void CComboUI::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
}

void CComboUI::SetHotItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diHot.sDrawString == pStrImage && m_ListInfo.diHot.pImageInfo != NULL ) return;
	m_ListInfo.diHot.Clear();
	m_ListInfo.diHot.sDrawString = pStrImage;
}

DWORD CComboUI::GetHotItemTextColor() const
{
	return m_ListInfo.dwHotTextColor;
}
DWORD CComboUI::GetHotItemBkColor() const
{
	return m_ListInfo.dwHotBkColor;
}

LPCTSTR CComboUI::GetHotItemImage() const
{
	return m_ListInfo.diHot.sDrawString;
}

void CComboUI::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
}

void CComboUI::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
}

void CComboUI::SetDisabledItemImage(LPCTSTR pStrImage)
{
	if( m_ListInfo.diDisabled.sDrawString == pStrImage && m_ListInfo.diDisabled.pImageInfo != NULL ) return;
	m_ListInfo.diDisabled.Clear();
	m_ListInfo.diDisabled.sDrawString = pStrImage;
}

DWORD CComboUI::GetDisabledItemTextColor() const
{
	return m_ListInfo.dwDisabledTextColor;
}

DWORD CComboUI::GetDisabledItemBkColor() const
{
	return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR CComboUI::GetDisabledItemImage() const
{
	return m_ListInfo.diDisabled.sDrawString;
}

int CComboUI::GetItemHLineSize() const
{
    return m_ListInfo.iHLineSize;
}

void CComboUI::SetItemHLineSize(int iSize)
{
    m_ListInfo.iHLineSize = iSize;
}

DWORD CComboUI::GetItemHLineColor() const
{
    return m_ListInfo.dwHLineColor;
}

void CComboUI::SetItemHLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwHLineColor = dwLineColor;
}

int CComboUI::GetItemVLineSize() const
{
    return m_ListInfo.iVLineSize;
}

void CComboUI::SetItemVLineSize(int iSize)
{
    m_ListInfo.iVLineSize = iSize;
}

DWORD CComboUI::GetItemVLineColor() const
{
    return m_ListInfo.dwVLineColor;
}

void CComboUI::SetItemVLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwVLineColor = dwLineColor;
}

bool CComboUI::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void CComboUI::SetItemShowHtml(bool bShowHtml)
{
    if( m_ListInfo.bShowHtml == bShowHtml ) return;

    m_ListInfo.bShowHtml = bShowHtml;
    Invalidate();
}

void CComboUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };
    for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CControlUI*>(m_items[i])->SetPos(rcNull, false);
    // Position this control
    CControlUI::SetPos(rc, bNeedInvalidate);
}

void CComboUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
	CControlUI::Move(szOffset, bNeedInvalidate);
}

void CComboUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetTextPadding(rcTextPadding);
    }
	else if( _tcscmp(pstrName, _T("showtext")) == 0 ) SetShowText(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
    else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
    else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
    else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
    else if( _tcscmp(pstrName, _T("dropbox")) == 0 ) SetDropBoxAttributeList(pstrValue);
	else if( _tcscmp(pstrName, _T("dropboxsize")) == 0)
	{
		SIZE szDropBoxSize = { 0 };
		LPTSTR pstr = NULL;
		szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		SetDropBoxSize(szDropBoxSize);
	}
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
    if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
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
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

bool CComboUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CComboUI::PaintStatusImage(HDC hDC)
{
    if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
    else m_uButtonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
    else m_uButtonState &= ~ UISTATE_DISABLED;

    if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
        if (DrawImage(hDC, m_diDisabled)) return;
    }
    else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
        if (DrawImage(hDC, m_diPushed)) return;
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if (DrawImage(hDC, m_diHot)) return;
    }
    else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
        if (DrawImage(hDC, m_diFocused)) return;
    }

    DrawImage(hDC, m_diNormal);
}

void CComboUI::PaintText(HDC hDC)
{
	if (!m_bShowText) return;

    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;
    rcText.top += m_rcTextPadding.top;
    rcText.bottom -= m_rcTextPadding.bottom;

    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if( pElement != NULL ) {
            pElement->DrawItemText(hDC, rcText);
        }
        else {
            RECT rcOldPos = pControl->GetPos();
            pControl->SetPos(rcText, false);
            pControl->Paint(hDC, rcText, NULL);
            pControl->SetPos(rcOldPos, false);
        }
    }
}

} // namespace DuiLib
