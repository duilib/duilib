#include "StdAfx.h"
#include "Core/WindowUI.h"
#include <WindowsX.h>

namespace DuiLib
{

/////////////////////////////////////////////////////////////////////////////////////
//
//
UI_IMPLEMENT_DYNCREATE(CComboUI);

class CComboWnd : public CWindowUI
{
public:
    void Init(CComboUI *pOwner);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
    virtual UINT GetClassStyle() const;
#endif

public:
    CComboUI *m_pOwner;
    CVerticalLayoutUI *m_pLayout;
    int m_iOldSel;
};


void CComboWnd::Init(CComboUI *pOwner)
{
    m_pOwner = pOwner;
    m_pLayout = NULL;
    m_iOldSel = m_pOwner->GetCurSel();
    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;     // 父窗口left、bottom位置作为弹出窗口起点
    rc.bottom = rc.top + szDrop.cy; // 计算弹出窗口高度

    if(szDrop.cx > 0)
    {
        rc.right = rc.left + szDrop.cx;    // 计算弹出窗口宽度
    }

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;

    for(int it = 0; it < pOwner->GetCount(); it++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(pOwner->GetItemAt(it));

        if(!pControl->IsVisible())
        {
            continue;
        }

        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }

    cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);
    ::MapWindowRect(pOwner->GetManager()->GetHWND(), HWND_DESKTOP, &rc);
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;

    if(rc.bottom > rcWork.bottom)
    {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;

        if(szDrop.cx > 0)
        {
            rc.right = rc.left + szDrop.cx;
        }

        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetHWND(), HWND_DESKTOP, &rc);
    }

    Create(pOwner->GetManager()->GetHWND(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;

    while(::GetParent(hWndParent) != NULL)
    {
        hWndParent = ::GetParent(hWndParent);
    }

    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CComboWnd::GetWindowClassName() const
{
    return _T("ComboWnd");
}

void CComboWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pWindow = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_Pushed;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT CComboWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_CREATE)
    {
        this->ModifyStyle(WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_CAPTION);
        this->ModifyExStyle(0, WS_EX_WINDOWEDGE);
        // 等同于发送WM_NCCALCSIZE消息
        RECT rcClient;
        ::GetClientRect(m_hWnd, &rcClient);
        ::SetWindowPos(m_hWnd, NULL, 0, 0, rcClient.right - rcClient.left, \
                       rcClient.bottom - rcClient.top, SWP_FRAMECHANGED | SWP_NOMOVE);
        m_hPaintDC = ::GetDC(m_hWnd);
        GetUIEngine()->SkinWindow(this);
        m_hInstance = GetUIEngine()->GetInstanceHandle();
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CVerticalLayoutUI;
        m_pLayout->SetManager(this, NULL);
// TODO 这里是针对ComboBox弹出窗口设置默认属性
        GetResEngine()->ApplyResTemplate(m_pLayout, _T("VerticalLayout"));
        m_pLayout->SetInset(CDuiRect(1, 1, 1, 1));
        m_pLayout->SetBkColor(0xFFFFFFFF);
        m_pLayout->SetBorderColor(0xFFC6C7D2);
        m_pLayout->SetBorderSize(1);
        m_pLayout->SetAutoDestroy(false);
        m_pLayout->EnableScrollBar();
        m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());

        for(int i = 0; i < m_pOwner->GetCount(); i++)
        {
            m_pLayout->Add(static_cast<CControlUI *>(m_pOwner->GetItemAt(i)));
        }

        m_pRootControl = m_pLayout;
        return 0;
    }
    else if(uMsg == WM_CLOSE)
    {
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        m_pOwner->SetPos(m_pOwner->GetPos(), false);
        m_pOwner->SetFocus();
    }
    else if(uMsg == WM_LBUTTONUP)
    {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(GetHWND(), &pt);
        CControlUI *pControl = FindControl(pt);

        if(pControl && _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0)
        {
            PostMessage(WM_KILLFOCUS);
        }
    }
    else if(uMsg == WM_KEYDOWN)
    {
        switch(wParam)
        {
            case VK_ESCAPE:
                m_pOwner->SelectItem(m_iOldSel, true);
                EnsureVisible(m_iOldSel);

            // FALL THROUGH...
            case VK_RETURN:
                PostMessage(WM_KILLFOCUS);
                break;

            default:
                TEventUI event;
                event.dwType = UIEVENT_KEYDOWN;
                event.chKey = (TCHAR)wParam;
                m_pOwner->EventHandler(event);
                EnsureVisible(m_pOwner->GetCurSel());
                return 0;
        }
    }
    else if(uMsg == WM_MOUSEWHEEL)
    {
        int zDelta = (int)(short) HIWORD(wParam);
        TEventUI event;
        event.dwType = UIEVENT_SCROLLWHEEL;
        event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
        event.lParam = lParam;
        event.dwTimestamp = ::GetTickCount();
        m_pOwner->EventHandler(event);
        EnsureVisible(m_pOwner->GetCurSel());
        return 0;
    }
    else if(uMsg == WM_KILLFOCUS)
    {
        if(m_hWnd != (HWND) wParam)
        {
            PostMessage(WM_CLOSE);
        }
    }

    bool bHandled = false;
    LRESULT lRes = S_OK;
    lRes = CWindowUI::MessageHandler(uMsg, wParam, lParam, bHandled);

    if(bHandled)
    {
        return lRes;
    }

    return CWindowWnd::WindowProc(uMsg, wParam, lParam);
    (uMsg, wParam, lParam);
}

void CComboWnd::EnsureVisible(int iIndex)
{
    if(m_pOwner->GetCurSel() < 0)
    {
        return;
    }

    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
    CScrollBarUI *pHorizontalScrollBar = m_pLayout->GetHorizontalScrollBar();

    if(pHorizontalScrollBar && pHorizontalScrollBar->IsVisible())
    {
        rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();
    }

    int iPos = m_pLayout->GetScrollPos().cy;

    if(rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom)
    {
        return;
    }

    int dx = 0;

    if(rcItem.top < rcList.top)
    {
        dx = rcItem.top - rcList.top;
    }

    if(rcItem.bottom > rcList.bottom)
    {
        dx = rcItem.bottom - rcList.bottom;
    }

    Scroll(0, dx);
}

void CComboWnd::Scroll(int dx, int dy)
{
    if(dx == 0 && dy == 0)
    {
        return;
    }

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


CComboUI::CComboUI()
    : m_pWindow(NULL)
    , m_pEditWnd(NULL)
    , m_iCurSel(-1)
    , m_uButtonState(0)
{
    m_szDropBox = CDuiSize(0, 150);
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
    m_ListInfo.nColumns = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER;
    m_ListInfo.dwTextColor = 0xFF000000;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.dwLineColor = 0;
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

LPCTSTR CComboUI::GetClass() const
{
    return _T("ComboUI");
}

LPVOID CComboUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("Combo")) == 0)
    {
        return static_cast<CComboUI *>(this);
    }

    if(_tcscmp(pstrName, _T("IListOwner")) == 0)
    {
        return static_cast<IListOwnerUI *>(this);
    }

    return CContainerUI::GetInterface(pstrName);
}

bool CComboUI::IsClientArea()
{
    return true;
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

bool CComboUI::SelectItem(int iIndex, bool bTakeFocus)
{
    if(m_pWindow != NULL)
    {
        m_pWindow->CloseWindow();
    }

    if(iIndex == m_iCurSel)
    {
        return true;
    }

    int iOldSel = m_iCurSel;

    if(m_iCurSel >= 0)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[m_iCurSel]);

        if(!pControl)
        {
            return false;
        }

        IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(_T("ListItem")));

        if(pListItem != NULL)
        {
            pListItem->Select(false);
        }

        m_iCurSel = -1;
    }

    if(iIndex < 0)
    {
        return false;
    }

    if(m_items.GetSize() == 0)
    {
        return false;
    }

    if(iIndex >= m_items.GetSize())
    {
        iIndex = m_items.GetSize() - 1;
    }

    CControlUI *pControl = static_cast<CControlUI *>(m_items[iIndex]);

    if(!pControl || !pControl->IsVisible() || !pControl->IsEnabled())
    {
        return false;
    }

    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(_T("ListItem")));

    if(pListItem == NULL)
    {
        return false;
    }

    m_iCurSel = iIndex;

    if(m_pWindow != NULL || bTakeFocus)
    {
        pControl->SetFocus();
    }

    pListItem->Select(true);

    if(m_pManager != NULL)
    {
        m_pManager->SendNotify(this, UINOTIFY_ITEM_SELECTED, m_iCurSel, iOldSel);
    }

    Invalidate();
    return true;
}

bool CComboUI::SetItemIndex(CControlUI *pControl, int iIndex)
{
    int iOrginIndex = GetItemIndex(pControl);

    if(iOrginIndex == -1)
    {
        return false;
    }

    if(iOrginIndex == iIndex)
    {
        return true;
    }

    IListItemUI *pSelectedListItem = NULL;

    if(m_iCurSel >= 0) pSelectedListItem =
            static_cast<IListItemUI *>(GetItemAt(m_iCurSel)->GetInterface(_T("ListItem")));

    if(!CContainerUI::SetItemIndex(pControl, iIndex))
    {
        return false;
    }

    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);

    for(int i = iMinIndex; i < iMaxIndex + 1; ++i)
    {
        CControlUI *p = GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(_T("ListItem")));

        if(pListItem != NULL)
        {
            pListItem->SetIndex(i);
        }
    }

    if(m_iCurSel >= 0 && pSelectedListItem != NULL)
    {
        m_iCurSel = pSelectedListItem->GetIndex();
    }

    return true;
}

bool CComboUI::Add(CControlUI *pControl)
{
    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(_T("ListItem")));

    if(pListItem != NULL)
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(m_items.GetSize());
    }

    return CContainerUI::Add(pControl);
}

bool CComboUI::AddAt(CControlUI *pControl, int iIndex)
{
    if(!CContainerUI::AddAt(pControl, iIndex))
    {
        return false;
    }

    // The list items should know about us
    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(_T("ListItem")));

    if(pListItem != NULL)
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < GetCount(); ++i)
    {
        CControlUI *p = GetItemAt(i);
        pListItem = static_cast<IListItemUI *>(p->GetInterface(_T("ListItem")));

        if(pListItem != NULL)
        {
            pListItem->SetIndex(i);
        }
    }

    if(m_iCurSel >= iIndex)
    {
        m_iCurSel += 1;
    }

    return true;
}

bool CComboUI::Remove(CControlUI *pControl)
{
    int iIndex = GetItemIndex(pControl);

    if(iIndex == -1)
    {
        return false;
    }

    if(!CContainerUI::RemoveAt(iIndex))
    {
        return false;
    }

    for(int i = iIndex; i < GetCount(); ++i)
    {
        CControlUI *p = GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(_T("ListItem")));

        if(pListItem != NULL)
        {
            pListItem->SetIndex(i);
        }
    }

    if(iIndex == m_iCurSel && m_iCurSel >= 0)
    {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if(iIndex < m_iCurSel)
    {
        m_iCurSel -= 1;
    }

    return true;
}

bool CComboUI::RemoveAt(int iIndex)
{
    if(!CContainerUI::RemoveAt(iIndex))
    {
        return false;
    }

    for(int i = iIndex; i < GetCount(); ++i)
    {
        CControlUI *p = GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(_T("ListItem")));

        if(pListItem != NULL)
        {
            pListItem->SetIndex(i);
        }
    }

    if(iIndex == m_iCurSel && m_iCurSel >= 0)
    {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if(iIndex < m_iCurSel)
    {
        m_iCurSel -= 1;
    }

    return true;
}

void CComboUI::RemoveAll()
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll();
}

void CComboUI::EventHandler(TEventUI & event)
{
    if(!IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND)
    {
        if(m_pParent != NULL)
        {
            m_pParent->EventHandler(event);
        }
        else
        {
            CContainerUI::EventHandler(event);
        }

        return;
    }

    if(event.dwType == UIEVENT_SETFOCUS)
    {
        Invalidate();
    }

    if(event.dwType == UIEVENT_KILLFOCUS)
    {
        Invalidate();
    }

    if(event.dwType == UIEVENT_LBUTTONDOWN)
    {
        if(IsEnabled())
        {
            Activate();
            m_uButtonState |= UISTATE_Pushed | UISTATE_Captured;
        }

        return;
    }

    if(event.dwType == UIEVENT_LBUTTONUP)
    {
        if((m_uButtonState & UISTATE_Captured) != 0)
        {
            m_uButtonState &= ~ UISTATE_Captured;
            Invalidate();
        }

        return;
    }

    if(event.dwType == UIEVENT_MOUSEMOVE)
    {
        return;
    }

    if(event.dwType == UIEVENT_KEYDOWN)
    {
        switch(event.chKey)
        {
            case VK_F4:
                Activate();
                return;

            case VK_UP:
                SelectItem(FindSelectable(m_iCurSel - 1, false));
                return;

            case VK_DOWN:
                SelectItem(FindSelectable(m_iCurSel + 1, true));
                return;

            case VK_PRIOR:
                SelectItem(FindSelectable(m_iCurSel - 1, false));
                return;

            case VK_NEXT:
                SelectItem(FindSelectable(m_iCurSel + 1, true));
                return;

            case VK_HOME:
                SelectItem(FindSelectable(0, false));
                return;

            case VK_END:
                SelectItem(FindSelectable(GetCount() - 1, true));
                return;
        }
    }

    if(event.dwType == UIEVENT_SCROLLWHEEL)
    {
        bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
        SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
        return;
    }

    if(event.dwType == UIEVENT_CONTEXTMENU)
    {
        return;
    }

    if(event.dwType == UIEVENT_MOUSEENTER)
    {
        if(::PtInRect(&m_rcItem, event.ptMouse))
        {
            if((m_uButtonState & UISTATE_Hover) == 0)
            {
                m_uButtonState |= UISTATE_Hover;
            }

            Invalidate();
        }

        return;
    }

    if(event.dwType == UIEVENT_MOUSELEAVE)
    {
        if((m_uButtonState & UISTATE_Hover) != 0)
        {
            m_uButtonState &= ~UISTATE_Hover;
            Invalidate();
        }

        return;
    }

    CControlUI::EventHandler(event);
}

SIZE CComboUI::EstimateSize(SIZE szAvailable)
{
    if(m_cxyFixed.cy == 0)
    {
        return CDuiSize(m_cxyFixed.cx, GetResEngine()->GetDefaultFont()->GetTextMetric().tmHeight + 12);
    }

    return CControlUI::EstimateSize(szAvailable);
}

bool CComboUI::Activate()
{
    if(!CControlUI::Activate())
    {
        return false;
    }

    if(m_pWindow)
    {
        return true;
    }

    m_pWindow = new CComboWnd();
    ASSERT(m_pWindow);
    m_pWindow->Init(this);

    if(m_pManager != NULL)
    {
        m_pManager->SendNotify(this, UINOTIFY_DROPDOWN);
    }

    Invalidate();
    return true;
}

LPCTSTR CComboUI::GetText() const
{
    if(m_iCurSel < 0)
    {
        return NULL;
    }

    CControlUI *pControl = static_cast<CControlUI *>(m_items[m_iCurSel]);
    return pControl->GetText();
}

void CComboUI::SetEnabled(bool bEnable)
{
    CContainerUI::SetEnabled(bEnable);

    if(!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

LPCTSTR CComboUI::GetDropBoxAttributeList()
{
    return m_sDropBoxAttributes.c_str();
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
    return m_sNormalImage.c_str();
}

void CComboUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboUI::GetHotImage() const
{
    return m_sHotImage.c_str();
}

void CComboUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboUI::GetPushedImage() const
{
    return m_sPushedImage.c_str();
}

void CComboUI::SetPushedImage(LPCTSTR pStrImage)
{
    m_sPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboUI::GetFocusedImage() const
{
    return m_sFocusedImage.c_str();
}

void CComboUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CComboUI::GetDisabledImage() const
{
    return m_sDisabledImage.c_str();
}

void CComboUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

TListInfoUI *CComboUI::GetListInfo()
{
    return &m_ListInfo;
}

void CComboUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    Invalidate();
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
    m_ListInfo.sBkImage = pStrImage;
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
    return m_ListInfo.sBkImage.c_str();
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
    m_ListInfo.sSelectedImage = pStrImage;
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
    return m_ListInfo.sSelectedImage.c_str();
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
    m_ListInfo.sHotImage = pStrImage;
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
    return m_ListInfo.sHotImage.c_str();
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
    m_ListInfo.sDisabledImage = pStrImage;
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
    return m_ListInfo.sDisabledImage.c_str();
}

DWORD CComboUI::GetItemLineColor() const
{
    return m_ListInfo.dwLineColor;
}

void CComboUI::SetItemLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwLineColor = dwLineColor;
}

bool CComboUI::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void CComboUI::SetItemShowHtml(bool bShowHtml)
{
    if(m_ListInfo.bShowHtml == bShowHtml)
    {
        return;
    }

    m_ListInfo.bShowHtml = bShowHtml;
    Invalidate();
}

void CComboUI::SetPos(RECT rc, bool bNeedInvalidate /*= true*/)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };

    for(int i = 0; i < m_items.GetSize(); i++)
    {
        static_cast<CControlUI *>(m_items[i])->SetPos(rcNull, false);
    }

    // Position this control
    CControlUI::SetPos(rc, bNeedInvalidate);
}

void CComboUI::Move(SIZE szOffset, bool bNeedInvalidate /*= true*/)
{
    CControlUI::Move(szOffset, bNeedInvalidate);
}

void CComboUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if(_tcscmp(pstrName, _T("textpadding")) == 0)
    {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);
        ASSERT(pstr);
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        SetTextPadding(rcTextPadding);
    }
    else if(_tcscmp(pstrName, _T("normalimage")) == 0)
    {
        SetNormalImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("hotimage")) == 0)
    {
        SetHotImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("pushedimage")) == 0)
    {
        SetPushedImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("focusedimage")) == 0)
    {
        SetFocusedImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("disabledimage")) == 0)
    {
        SetDisabledImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("dropbox")) == 0)
    {
        SetDropBoxAttributeList(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("dropboxsize")) == 0)
    {
        SIZE szDropBoxSize = { 0 };
        LPTSTR pstr = NULL;
        szDropBoxSize.cx = _tcstol(pstrValue, &pstr, 10);
        ASSERT(pstr);
        szDropBoxSize.cy = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        SetDropBoxSize(szDropBoxSize);
    }
    else if(_tcscmp(pstrName, _T("itemfont")) == 0)
    {
        m_ListInfo.nFont = _ttoi(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("itemalign")) == 0)
    {
        if(_tcsstr(pstrValue, _T("left")) != NULL)
        {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }

        if(_tcsstr(pstrValue, _T("center")) != NULL)
        {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }

        if(_tcsstr(pstrValue, _T("right")) != NULL)
        {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }

    if(_tcscmp(pstrName, _T("itemtextpadding")) == 0)
    {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = NULL;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);
        ASSERT(pstr);
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10);
        ASSERT(pstr);
        SetItemTextPadding(rcTextPadding);
    }
    else if(_tcscmp(pstrName, _T("itemtextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemTextColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itembkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itembkimage")) == 0)
    {
        SetItemBkImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("itemaltbk")) == 0)
    {
        SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if(_tcscmp(pstrName, _T("itemselectedtextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemTextColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemselectedbkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedItemBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemselectedimage")) == 0)
    {
        SetSelectedItemImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("itemhottextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemTextColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemhotbkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotItemBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemhotimage")) == 0)
    {
        SetHotItemImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemTextColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDisabledItemBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemdisabledimage")) == 0)
    {
        SetDisabledItemImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("itemlinecolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetItemLineColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("itemshowhtml")) == 0)
    {
        SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else
    {
        CContainerUI::SetAttribute(pstrName, pstrValue);
    }
}

void CComboUI::DoPaint(HDC hDC, const RECT & rcPaint)
{
    CControlUI::DoPaint(hDC, rcPaint);
}

void CComboUI::PaintStatusImage(HDC hDC)
{
    if(IsFocused())
    {
        m_uButtonState |= UISTATE_Focused;
    }
    else
    {
        m_uButtonState &= ~ UISTATE_Focused;
    }

    if(!IsEnabled())
    {
        m_uButtonState |= UISTATE_Disabled;
    }
    else
    {
        m_uButtonState &= ~ UISTATE_Disabled;
    }

    if((m_uButtonState & UISTATE_Disabled) != 0)
    {
        if(!m_sDisabledImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sDisabledImage.c_str()))
            {
                m_sDisabledImage.clear();
            }
            else
            {
                return;
            }
        }
    }
    else if((m_uButtonState & UISTATE_Pushed) != 0)
    {
        if(!m_sPushedImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sPushedImage.c_str()))
            {
                m_sPushedImage.clear();
            }
            else
            {
                return;
            }
        }
    }
    else if((m_uButtonState & UISTATE_Hover) != 0)
    {
        if(!m_sHotImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sHotImage.c_str()))
            {
                m_sHotImage.clear();
            }
            else
            {
                return;
            }
        }
    }
    else if((m_uButtonState & UISTATE_Focused) != 0)
    {
        if(!m_sFocusedImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sFocusedImage.c_str()))
            {
                m_sFocusedImage.clear();
            }
            else
            {
                return;
            }
        }
    }

    if(!m_sNormalImage.empty())
    {
        if(!DrawImage(hDC, (LPCTSTR)m_sNormalImage.c_str()))
        {
            m_sNormalImage.clear();
        }
        else
        {
            return;
        }
    }
}

void CComboUI::PaintText(HDC hDC)
{
    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;
    rcText.top += m_rcTextPadding.top;
    rcText.bottom -= m_rcTextPadding.bottom;

    if(m_iCurSel >= 0)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[m_iCurSel]);
        IListItemUI *pElement = static_cast<IListItemUI *>(pControl->GetInterface(_T("ListItem")));

        if(pElement != NULL)
        {
            pElement->DrawItemText(hDC, rcText);
        }
        else
        {
            RECT rcOldPos = pControl->GetPos();
            pControl->SetPos(rcText,false);
            pControl->DoPaint(hDC, rcText);
            pControl->SetPos(rcOldPos,false);
        }
    }
}

} // namespace DuiLib
