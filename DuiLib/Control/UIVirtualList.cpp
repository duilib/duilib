#include "StdAfx.h"
#include "UIVirtualList.h"

namespace DuiLib {

// =============================================================================
// CVirtualListBodyUI
// =============================================================================

CVirtualListBodyUI::CVirtualListBodyUI(CVirtualListUI* pOwner)
    : m_pOwner(pOwner)
    , m_iOldYScrollPos(0)
    , m_bArrangedOnce(false)
{
    ASSERT(m_pOwner);
    // 不在 ctor 中调用 EnableScrollBar：此时 m_pManager 尚未被赋值，
    // 创建出来的 ScrollBar 无法应用 PaintManager 中的默认皮肤属性（VScrollBar/HScrollBar）。
    // 需要滚动条的话，请在 XML 上写 vscrollbar="true" / hscrollbar="true"
    // 或在 Window 创建后调用 CVirtualListUI::EnableScrollBar(true, true)。
}

void CVirtualListBodyUI::SetScrollPos(SIZE szPos)
{
    int cx = 0;
    int cy = 0;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
        m_pVerticalScrollBar->SetScrollPos(szPos.cy);
        cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
        m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
        cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if (cx == 0 && cy == 0) return;

    if (cx != 0) {
        // 水平滚动：仅平移，不改变虚拟可视行
        for (int i = 0; i < m_items.GetSize(); ++i) {
            CControlUI* pControl = static_cast<CControlUI*>(m_items[i]);
            if (!pControl->IsVisible()) continue;
            if (pControl->IsFloat()) continue;
            pControl->Move(CDuiSize(-cx, 0), false);
        }
        // 同步表头水平位置 + 列 rect
        if (m_pOwner) {
            CListHeaderUI* pHeader = m_pOwner->GetHeader();
            if (pHeader != NULL) {
                TListInfoUI* pInfo = m_pOwner->GetListInfo();
                pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);
                for (int i = 0; i < pInfo->nColumns; ++i) {
                    CControlUI* pCol = static_cast<CControlUI*>(pHeader->GetItemAt(i));
                    if (!pCol->IsVisible()) continue;
                    if (pCol->IsFloat()) continue;
                    pCol->Move(CDuiSize(-cx, 0), false);
                    pInfo->rcColumn[i] = pCol->GetPos();
                }
                pHeader->Invalidate();
            }
        }
    }

    if (cy != 0) {
        // 垂直滚动：触发虚表重排（控件位置由 DoArrange 负责）
        DoArrange(false);
    }

    Invalidate();
}

void CVirtualListBodyUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    rc.left   += m_rcInset.left;
    rc.top    += m_rcInset.top;
    rc.right  -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    int cxNeeded = 0;
    int cyNeeded = 0;

    if (m_pOwner) {
        TListInfoUI* pInfo = m_pOwner->GetListInfo();
        if (pInfo != NULL && pInfo->nColumns > 0) {
            // 列总宽 = 表头各列宽之和（最右减最左，水平滚动覆盖的是这一段）
            cxNeeded = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
        } else {
            CListHeaderUI* pHeader = m_pOwner->GetHeader();
            if (pHeader != NULL && pHeader->GetCount() > 0) {
                cxNeeded = pHeader->EstimateSize(CDuiSize(rc.right - rc.left, rc.bottom - rc.top)).cx;
            }
        }
        cyNeeded = m_pOwner->GetItemHeight() * m_pOwner->GetElementCount();
    }

    // 让基类容器的 ProcessScrollBar 处理滚动条的自动显示/隐藏与范围
    ProcessScrollBar(rc, cxNeeded, cyNeeded);

    // 重新摆放池中控件
    DoArrange(true);
}

void CVirtualListBodyUI::DoEvent(TEventUI& event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pOwner != NULL) m_pOwner->DoEvent(event);
        else CControlUI::DoEvent(event);
        return;
    }

    if (m_pOwner != NULL) {
        if (event.Type == UIEVENT_SCROLLWHEEL) {
            // 水平滚动条上滚轮 → 水平滚动
            if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled()) {
                RECT rcHorizontalScrollBar = m_pHorizontalScrollBar->GetPos();
                if (::PtInRect(&rcHorizontalScrollBar, event.ptMouse)) {
                    switch (LOWORD(event.wParam)) {
                    case SB_LINEUP:   m_pOwner->LineLeft(); return;
                    case SB_LINEDOWN: m_pOwner->LineRight(); return;
                    }
                }
            }
        }
        m_pOwner->DoEvent(event);
    } else {
        CControlUI::DoEvent(event);
    }
}

bool CVirtualListBodyUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    RECT rcTemp = { 0 };
    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return true;

    TListInfoUI* pListInfo = NULL;
    if (m_pOwner) pListInfo = m_pOwner->GetListInfo();

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    CControlUI::DoPaint(hDC, rcPaint, pStopControl);

    if (m_items.GetSize() > 0) {
        RECT rc = m_rcItem;
        rc.left   += m_rcInset.left;
        rc.top    += m_rcInset.top;
        rc.right  -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right  -= m_pVerticalScrollBar->GetFixedWidth();
        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

        if (!::IntersectRect(&rcTemp, &rcPaint, &rc)) {
            for (int it = 0; it < m_items.GetSize(); ++it) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if (pControl == pStopControl) return false;
                if (!pControl->IsVisible()) continue;
                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
                if (pControl->IsFloat()) {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) return false;
                }
            }
        } else {
            int iDrawIndex = 0;
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for (int it = 0; it < m_items.GetSize(); ++it) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                if (pControl == pStopControl) return false;
                if (!pControl->IsVisible()) continue;
                if (!pControl->IsFloat()) {
                    IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
                    if (pListItem != NULL) {
                        // 用控件的"数据下标"作为 drawIndex，让交替背景跟随数据而非池位置
                        int iDataIdx = pListItem->GetIndex();
                        pListItem->SetDrawIndex(iDataIdx >= 0 ? iDataIdx : iDrawIndex);
                        iDrawIndex += 1;
                    }
                    if (pListInfo && pListInfo->iHLineSize > 0) {
                        RECT rcPadding = pControl->GetPadding();
                        const RECT& rcPos = pControl->GetPos();
                        RECT rcBottomLine = { rcPos.left, rcPos.bottom + rcPadding.bottom,
                                              rcPos.right, rcPos.bottom + rcPadding.bottom + pListInfo->iHLineSize };
                        if (::IntersectRect(&rcTemp, &rcPaint, &rcBottomLine)) {
                            rcBottomLine.top    += pListInfo->iHLineSize / 2;
                            rcBottomLine.bottom = rcBottomLine.top;
                            CRenderEngine::DrawLine(hDC, rcBottomLine, pListInfo->iHLineSize, GetAdjustColor(pListInfo->dwHLineColor));
                        }
                    }
                }
                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
                if (pControl->IsFloat()) {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) return false;
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                } else {
                    if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) continue;
                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) return false;
                }
            }
        }
    }

    if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible()) {
        if (m_pVerticalScrollBar == pStopControl) return false;
        if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos())) {
            if (!m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl)) return false;
        }
    }
    if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible()) {
        if (m_pHorizontalScrollBar == pStopControl) return false;
        if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos())) {
            if (!m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl)) return false;
        }
    }
    return true;
}

void CVirtualListBodyUI::DoArrange(bool bForce)
{
    if (m_pOwner == NULL) return;
    int nItemHeight = m_pOwner->GetItemHeight();
    if (nItemHeight <= 0) return;
    int nDataCount = m_pOwner->GetElementCount();

    int nVScrollPos = 0;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
        nVScrollPos = m_pVerticalScrollBar->GetScrollPos();

    if (!bForce && m_bArrangedOnce && nVScrollPos == m_iOldYScrollPos) {
        // 没有滚动也没强制 → 无需重排
        return;
    }
    m_iOldYScrollPos = nVScrollPos;
    m_bArrangedOnce = true;

    // 计算可用区域（去除 inset 和滚动条占位）
    RECT rcView = m_rcItem;
    rcView.left   += m_rcInset.left;
    rcView.top    += m_rcInset.top;
    rcView.right  -= m_rcInset.right;
    rcView.bottom -= m_rcInset.bottom;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
        rcView.right -= m_pVerticalScrollBar->GetFixedWidth();
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
        rcView.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    int viewHeight = rcView.bottom - rcView.top;
    if (viewHeight <= 0 || rcView.right <= rcView.left) return;

    // 顶部第一个数据的下标和它的真实 y 偏移
    int iTopIndex = nVScrollPos / nItemHeight;

    int nPool = m_items.GetSize();
    int iHorzScroll = 0;
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
        iHorzScroll = m_pHorizontalScrollBar->GetScrollPos();

    TListInfoUI* pInfo = m_pOwner->GetListInfo();

    // 池中每个控件按顺序贴到 (iTopIndex + i) 的位置
    m_pOwner->m_bInArrange = true;
    for (int i = 0; i < nPool; ++i) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[i]);
        int iDataIndex = iTopIndex + i;
        IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if (iDataIndex >= nDataCount || iDataIndex < 0) {
            // 池里多余的控件（数据末尾不够填满）→ 隐藏。
            // 注意：CListContainerElementUI::SetVisible(false) 在自身被选中时会调用
            // SelectItem(-1)，会破坏我们维护的数据级选中状态——所以先清掉局部选中标志。
            if (pListItem && pListItem->IsSelected()) pListItem->Select(false, false);
            if (pControl->IsVisible()) pControl->SetVisible(false);
            continue;
        }
        if (!pControl->IsVisible()) pControl->SetVisible(true);

        // 控件的虚拟 y = iDataIndex * itemHeight；屏幕 y = rcView.top + virtY - scrollPos
        int yTop    = rcView.top + iDataIndex * nItemHeight - nVScrollPos;
        int yBottom = yTop + nItemHeight;
        // 水平方向：表头列存在时直接对齐到 column[0].left ~ column[N-1].right（列 rect 已经
        // 在父 SetPos 中根据水平滚动偏移更新好了），保证行背景跨满所有列。
        int xLeft, xRight;
        if (pInfo && pInfo->nColumns > 0) {
            xLeft  = pInfo->rcColumn[0].left;
            xRight = pInfo->rcColumn[pInfo->nColumns - 1].right;
        } else {
            xLeft  = rcView.left  - iHorzScroll;
            xRight = rcView.right - iHorzScroll;
        }

        RECT rcCtrl = { xLeft, yTop, xRight, yBottom };
        pControl->SetPos(rcCtrl, false);

        // 同步选中状态 + IListItemUI 的 m_iIndex（pListItem 已在循环开头取过）
        if (pListItem) {
            pListItem->SetIndex(iDataIndex);
            // 选中状态由数据下标驱动
            bool bWantSel = (iDataIndex == m_pOwner->GetCurSelDataIndex());
            if (pListItem->IsSelected() != bWantSel) {
                pListItem->Select(bWantSel, false);
            }
        }

        // 数据填充
        m_pOwner->FillElement(pControl, iDataIndex);
    }

    m_pOwner->m_bInArrange = false;

    Invalidate();
}

void CVirtualListBodyUI::RefillAll()
{
    if (m_pOwner == NULL) return;
    int nItemHeight = m_pOwner->GetItemHeight();
    int nDataCount = m_pOwner->GetElementCount();
    int nVScrollPos = 0;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
        nVScrollPos = m_pVerticalScrollBar->GetScrollPos();
    int iTopIndex = (nItemHeight > 0) ? (nVScrollPos / nItemHeight) : 0;

    RECT rcView = m_rcItem;
    rcView.left   += m_rcInset.left;
    rcView.top    += m_rcInset.top;
    rcView.right  -= m_rcInset.right;
    rcView.bottom -= m_rcInset.bottom;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
        rcView.right -= m_pVerticalScrollBar->GetFixedWidth();
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
        rcView.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

    int iHorzScroll = 0;
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
        iHorzScroll = m_pHorizontalScrollBar->GetScrollPos();

    TListInfoUI* pInfo = m_pOwner->GetListInfo();

    m_pOwner->m_bInArrange = true;

    for (int i = 0; i < m_items.GetSize(); ++i) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[i]);
        int iDataIndex = iTopIndex + i;

        // Handle out-of-range items: hide and deselect
        if (iDataIndex >= nDataCount || iDataIndex < 0) {
            IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
            if (pListItem && pListItem->IsSelected()) pListItem->Select(false, false);
            if (pControl->IsVisible()) pControl->SetVisible(false);
            continue;
        }

        // Make sure in-range items are visible
        if (!pControl->IsVisible()) pControl->SetVisible(true);

        // Update position
        int yTop    = rcView.top + iDataIndex * nItemHeight - nVScrollPos;
        int yBottom = yTop + nItemHeight;
        int xLeft, xRight;
        if (pInfo && pInfo->nColumns > 0) {
            xLeft  = pInfo->rcColumn[0].left;
            xRight = pInfo->rcColumn[pInfo->nColumns - 1].right;
        } else {
            xLeft  = rcView.left  - iHorzScroll;
            xRight = rcView.right - iHorzScroll;
        }
        RECT rcCtrl = { xLeft, yTop, xRight, yBottom };
        pControl->SetPos(rcCtrl, false);

        // Update data index and selection
        IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
        if (pListItem) {
            pListItem->SetIndex(iDataIndex);
            bool bWantSel = (iDataIndex == m_pOwner->GetCurSelDataIndex());
            if (pListItem->IsSelected() != bWantSel) {
                pListItem->Select(bWantSel, false);
            }
        }

        m_pOwner->FillElement(pControl, iDataIndex);
    }

    m_pOwner->m_bInArrange = false;

    Invalidate();
}

// =============================================================================
// CVirtualListUI
// =============================================================================

CVirtualListUI::CVirtualListUI()
    : m_pList(NULL)
    , m_pHeader(NULL)
    , m_pCallback(NULL)
    , m_pProvider(NULL)
    , m_nItemHeight(0)
    , m_nMaxPoolSize(-1)
    , m_iCurSelDataIdx(-1)
    , m_bInArrange(false)
{
    m_pList   = new CVirtualListBodyUI(this);
    m_pHeader = new CListHeaderUI;

    CVerticalLayoutUI::Add(m_pHeader);
    CVerticalLayoutUI::Add(m_pList);

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
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

CVirtualListUI::~CVirtualListUI()
{
    // m_pList / m_pHeader 已由 CContainerUI 基类的 m_items 负责销毁
}

LPCTSTR CVirtualListUI::GetClass() const
{
    return _T("VirtualListUI");
}

UINT CVirtualListUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

LPVOID CVirtualListUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_VIRTUALLIST) == 0) return static_cast<CVirtualListUI*>(this);
    if (_tcscmp(pstrName, DUI_CTR_LIST)         == 0) return static_cast<IListUI*>(this);
    if (_tcscmp(pstrName, DUI_CTR_ILIST)        == 0) return static_cast<IListUI*>(this);
    if (_tcscmp(pstrName, DUI_CTR_ILISTOWNER)   == 0) return static_cast<IListOwnerUI*>(this);
    return CVerticalLayoutUI::GetInterface(pstrName);
}

// ===== Provider / 池 =====

void CVirtualListUI::SetDataProvider(IVirtualListProvider* pProvider)
{
    m_pProvider = pProvider;
}

IVirtualListProvider* CVirtualListUI::GetDataProvider() const
{
    return m_pProvider;
}

void CVirtualListUI::SetItemHeight(int nHeight)
{
    if (nHeight <= 0) return;
    if (m_nItemHeight == nHeight) return;
    m_nItemHeight = nHeight;
    NeedUpdate();
}

int CVirtualListUI::GetItemHeight() const
{
    return m_nItemHeight;
}

int CVirtualListUI::GetElementCount() const
{
    if (m_pProvider) return m_pProvider->GetElementCount();
    return 0;
}

int CVirtualListUI::CalcPoolSize() const
{
    if (m_nItemHeight <= 0) return 0;
    if (m_pList == NULL) return 0;
    RECT rcClient = m_pList->GetClientPos();
    int viewHeight = rcClient.bottom - rcClient.top;
    if (viewHeight <= 0) return 0;
    // 上取整 + 1 缓冲行（确保 ListHeight 不是 ItemHeight 整数倍时，最后一行
    // 部分可见 + 滚动过渡时仍能从控件池里取到一行用于绘制）
    int n = (viewHeight + m_nItemHeight - 1) / m_nItemHeight + 1;
    if (n < 1) n = 1;
    return n;
}

void CVirtualListUI::EnsurePoolSize(int nWanted)
{
    if (m_pList == NULL || m_pProvider == NULL) return;
    if (nWanted < 0) nWanted = 0;
    int nDataCount = GetElementCount();
    // 池上限：用户指定优先；否则按数据量与可见区裁剪
    int nLimit = (m_nMaxPoolSize > 0) ? m_nMaxPoolSize : nWanted;
    int nFinal = nWanted;
    if (nFinal > nLimit) nFinal = nLimit;
    if (nDataCount >= 0 && nFinal > nDataCount && m_nMaxPoolSize <= 0) {
        // 数据量比可见行还少时不必创建多余控件
        nFinal = nDataCount;
    }
    int nCur = m_pList->GetCount();
    while (nCur < nFinal) {
        CControlUI* pControl = CreateElement();
        if (pControl == NULL) break;
        m_pList->Add(pControl);
        ++nCur;
    }
    while (nCur > nFinal) {
        m_pList->RemoveAt(nCur - 1);
        --nCur;
    }
}

void CVirtualListUI::InitElement(int nMaxPoolItem)
{
    ASSERT(m_pProvider);
    ASSERT(m_nItemHeight > 0);
    if (m_pProvider == NULL || m_nItemHeight <= 0) return;
    m_nMaxPoolSize = nMaxPoolItem;

    int nWant = (nMaxPoolItem > 0) ? nMaxPoolItem : CalcPoolSize();
    if (nWant <= 0) nWant = 1;
    EnsurePoolSize(nWant);

    NeedUpdate();
}

void CVirtualListUI::Refresh()
{
    if (m_pProvider == NULL) return;
    int nWant = CalcPoolSize();
    if (m_nMaxPoolSize > 0 && nWant > m_nMaxPoolSize) nWant = m_nMaxPoolSize;
    if (nWant < 1) nWant = 1;
    EnsurePoolSize(nWant);

    // 选中索引越界保护
    int nDataCount = GetElementCount();
    if (m_iCurSelDataIdx >= nDataCount) m_iCurSelDataIdx = -1;

    // 滚动位置可能因 range 缩小而被 ProcessScrollBar 自动夹紧
    if (m_pList) m_pList->NeedUpdate();
    NeedUpdate();
}

CControlUI* CVirtualListUI::CreateElement()
{
    if (m_pProvider == NULL) return NULL;
    CControlUI* pCtrl = m_pProvider->CreateElement();
    if (pCtrl == NULL) return NULL;
    IListItemUI* pListItem = static_cast<IListItemUI*>(pCtrl->GetInterface(DUI_CTR_ILISTITEM));
    if (pListItem != NULL) {
        pListItem->SetOwner(this);
        pListItem->SetIndex(-1);
    }
    return pCtrl;
}

void CVirtualListUI::FillElement(CControlUI* pControl, int iDataIndex)
{
    if (m_pProvider == NULL || pControl == NULL) return;
    if (iDataIndex < 0 || iDataIndex >= GetElementCount()) return;
    m_pProvider->FillElement(pControl, iDataIndex);
}

// ===== 选择 / 滚动 / 可见性 =====

void CVirtualListUI::SetCurSelDataIndex(int iDataIndex, bool bTriggerEvent)
{
    int nDataCount = GetElementCount();
    if (iDataIndex < -1) iDataIndex = -1;
    if (iDataIndex >= nDataCount) iDataIndex = nDataCount - 1;
    if (iDataIndex == m_iCurSelDataIdx) return;
    int iOld = m_iCurSelDataIdx;
    m_iCurSelDataIdx = iDataIndex;
    // DoArrange 会按 GetCurSelDataIndex() 同步每个池控件的选中态。
    if (m_pList) m_pList->DoArrange(true);
    Invalidate();
    if (bTriggerEvent && m_pManager) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSelDataIdx, iOld);
    }
}

int CVirtualListUI::GetCurSel() const
{
    return m_iCurSelDataIdx;
}

bool CVirtualListUI::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if (iIndex < 0 || iIndex >= GetElementCount()) {
        SetCurSelDataIndex(-1, bTriggerEvent);
        return iIndex == -1;
    }
    SetCurSelDataIndex(iIndex, bTriggerEvent);
    if (!m_bInArrange) EnsureVisible(iIndex, false);
    if (bTakeFocus && m_pList) m_pList->SetFocus();
    return true;
}

void CVirtualListUI::EnsureVisible(int iIndex, bool bToTop)
{
    if (iIndex < 0 || iIndex >= GetElementCount()) return;
    if (m_nItemHeight <= 0 || m_pList == NULL) return;

    CScrollBarUI* pV = m_pList->GetVerticalScrollBar();
    if (pV == NULL) return;

    RECT rcClient = m_pList->GetClientPos();
    int viewH = rcClient.bottom - rcClient.top;
    if (viewH <= 0) return;

    int yTop    = iIndex * m_nItemHeight;
    int yBottom = yTop + m_nItemHeight;

    int curPos = pV->GetScrollPos();
    int newPos = curPos;
    if (bToTop) {
        newPos = yTop;
    } else {
        if (yTop < curPos) newPos = yTop;
        else if (yBottom > curPos + viewH) newPos = yBottom - viewH;
    }
    if (newPos < 0) newPos = 0;
    int range = pV->GetScrollRange();
    if (newPos > range) newPos = range;
    if (newPos != curPos) {
        m_pList->SetScrollPos(CDuiSize(m_pList->GetScrollPos().cx, newPos));
    }
}

void CVirtualListUI::GetDisplayCollection(std::vector<int>& collection)
{
    collection.clear();
    if (m_nItemHeight <= 0 || m_pList == NULL) return;
    CScrollBarUI* pV = m_pList->GetVerticalScrollBar();
    int pos = (pV && pV->IsVisible()) ? pV->GetScrollPos() : 0;
    RECT rcClient = m_pList->GetClientPos();
    int viewH = rcClient.bottom - rcClient.top;
    if (viewH <= 0) return;

    int firstIdx = pos / m_nItemHeight;
    // 末尾要按"下沿落入视图"的方式取整，避免漏算最后部分可见行
    int lastBottom = pos + viewH;
    int lastIdx = (lastBottom + m_nItemHeight - 1) / m_nItemHeight - 1;
    int total = GetElementCount();
    if (lastIdx >= total) lastIdx = total - 1;
    if (firstIdx < 0) firstIdx = 0;
    for (int i = firstIdx; i <= lastIdx; ++i) collection.push_back(i);
}

// ===== IListUI =====

CListHeaderUI* CVirtualListUI::GetHeader() const { return m_pHeader; }
CContainerUI*  CVirtualListUI::GetList()   const { return m_pList; }
IListCallbackUI* CVirtualListUI::GetTextCallback() const { return m_pCallback; }
void CVirtualListUI::SetTextCallback(IListCallbackUI* pCallback) { m_pCallback = pCallback; }
TListInfoUI* CVirtualListUI::GetListInfo() { return &m_ListInfo; }

// ===== 事件 =====

void CVirtualListUI::DoEvent(TEventUI& event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pParent != NULL) m_pParent->DoEvent(event);
        else CVerticalLayoutUI::DoEvent(event);
        return;
    }

    if (event.Type == UIEVENT_SETFOCUS)  { m_bFocused = true; return; }
    if (event.Type == UIEVENT_KILLFOCUS) { m_bFocused = false; return; }

    if (event.Type == UIEVENT_KEYDOWN) {
        if (IsKeyboardEnabled() && IsEnabled()) {
            int total = GetElementCount();
            switch (event.chKey) {
            case VK_UP:
                if (total > 0) SelectItem(m_iCurSelDataIdx > 0 ? m_iCurSelDataIdx - 1 : 0, true);
                return;
            case VK_DOWN:
                if (total > 0) SelectItem(m_iCurSelDataIdx + 1 < total ? m_iCurSelDataIdx + 1 : total - 1, true);
                return;
            case VK_HOME:
                if (total > 0) SelectItem(0, true);
                return;
            case VK_END:
                if (total > 0) SelectItem(total - 1, true);
                return;
            case VK_PRIOR: PageUp(); return;
            case VK_NEXT:  PageDown(); return;
            }
        }
    }

    if (event.Type == UIEVENT_SCROLLWHEEL) {
        if (IsEnabled()) {
            switch (LOWORD(event.wParam)) {
            case SB_LINEUP:   LineUp();   return;
            case SB_LINEDOWN: LineDown(); return;
            }
        }
    }

    CVerticalLayoutUI::DoEvent(event);
}

// ===== 容器 Add/AddAt/Remove：拦截 ListHeader / ListHeaderItem，挂到 m_pHeader 上 =====

// 替换占位 header 的内部辅助：以 bDoNotDestroy=true 从 m_items 摘除后显式 delete，
// 避免 CContainerUI::Remove 在 m_pManager 还未注入时不会自动回收旧对象造成的内存泄漏。
static void ReplacePlaceholderHeader(CVerticalLayoutUI* self, CListHeaderUI*& pHeaderRef, CControlUI* pNewHeader)
{
    if (pHeaderRef != pNewHeader && pHeaderRef != NULL && pHeaderRef->GetCount() == 0) {
        CListHeaderUI* pOld = pHeaderRef;
        self->CVerticalLayoutUI::Remove(pOld, true);
        delete pOld;
        pHeaderRef = static_cast<CListHeaderUI*>(pNewHeader);
    }
}

bool CVirtualListUI::Add(CControlUI* pControl)
{
    if (pControl == NULL) return false;
    // XML 提供的 <ListHeader> —— 替换掉构造时创建的空 header
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) {
        ReplacePlaceholderHeader(this, m_pHeader, pControl);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return CVerticalLayoutUI::AddAt(pControl, 0);
    }
    // 直接添加 ListHeaderItem —— 挂到当前 m_pHeader 下
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) {
        bool ret = m_pHeader->Add(pControl);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    // 其他控件（罕见，例如 XML 中给 VirtualList 加装饰元素）走默认路径
    return CVerticalLayoutUI::Add(pControl);
}

bool CVirtualListUI::AddAt(CControlUI* pControl, int iIndex)
{
    if (pControl == NULL) return false;
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) {
        ReplacePlaceholderHeader(this, m_pHeader, pControl);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return CVerticalLayoutUI::AddAt(pControl, 0);
    }
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) {
        bool ret = m_pHeader->AddAt(pControl, iIndex);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    return CVerticalLayoutUI::AddAt(pControl, iIndex);
}

bool CVirtualListUI::Remove(CControlUI* pControl, bool bDoNotDestroy)
{
    if (pControl == NULL) return false;
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) {
        return CVerticalLayoutUI::Remove(pControl, bDoNotDestroy);
    }
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) {
        bool ret = m_pHeader->Remove(pControl, bDoNotDestroy);
        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }
    return CVerticalLayoutUI::Remove(pControl, bDoNotDestroy);
}

// ===== 布局 =====

void CVirtualListUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    // 1) 先让表头摆正，并更新 ListInfo 的列 rect（参考 CListUI::SetPos 同样的逻辑）
    if (m_pHeader != NULL) {
        int iLeft  = rc.left  + m_rcInset.left;
        int iRight = rc.right - m_rcInset.right;

        m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

        if (!m_pHeader->IsVisible()) {
            for (int it = m_pHeader->GetCount() - 1; it >= 0; --it)
                static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
        }
        // 让表头容器先铺一遍，以便其各列的 cxyFixed 经布局后正确
        m_pHeader->SetPos(CDuiRect(iLeft, 0, iRight, 0), false);
        int iOffset = (m_pList && m_pList->GetScrollPos().cx > 0) ? m_pList->GetScrollPos().cx : 0;
        for (int i = 0; i < m_ListInfo.nColumns; ++i) {
            CControlUI* pCol = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
            if (!pCol->IsVisible()) continue;
            if (pCol->IsFloat()) continue;
            RECT rcPos = pCol->GetPos();
            if (iOffset > 0) {
                rcPos.left  -= iOffset;
                rcPos.right -= iOffset;
                pCol->SetPos(rcPos, false);
            }
            m_ListInfo.rcColumn[i] = pCol->GetPos();
        }
        if (!m_pHeader->IsVisible()) {
            for (int it = m_pHeader->GetCount() - 1; it >= 0; --it)
                static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
            m_pHeader->SetInternVisible(false);
        }
    }

    // 2) 走基类布局：把 header / list 撑开放好
    CVerticalLayoutUI::SetPos(rc, bNeedInvalidate);

    if (m_pHeader == NULL) return;

    // 3) 用真实可视区域再次更新列 rect（去除滚动条占位、考虑表头隐藏的特殊情况）
    rc = m_rcItem;
    rc.left   += m_rcInset.left;
    rc.top    += m_rcInset.top;
    rc.right  -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;
    if (m_pList) {
        if (m_pList->GetVerticalScrollBar()   && m_pList->GetVerticalScrollBar()->IsVisible())
            rc.right -= m_pList->GetVerticalScrollBar()->GetFixedWidth();
    }
    m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
    if (!m_pHeader->IsVisible()) {
        for (int it = m_pHeader->GetCount() - 1; it >= 0; --it)
            static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
        m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), false);
    }
    int iOffset = (m_pList && m_pList->GetScrollPos().cx > 0) ? m_pList->GetScrollPos().cx : 0;
    for (int i = 0; i < m_ListInfo.nColumns; ++i) {
        CControlUI* pCol = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
        if (!pCol->IsVisible()) continue;
        if (pCol->IsFloat()) continue;
        RECT rcPos = pCol->GetPos();
        if (iOffset > 0) {
            rcPos.left  -= iOffset;
            rcPos.right -= iOffset;
            pCol->SetPos(rcPos, false);
        }
        m_ListInfo.rcColumn[i] = pCol->GetPos();
    }
    if (!m_pHeader->IsVisible()) {
        for (int it = m_pHeader->GetCount() - 1; it >= 0; --it)
            static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
        m_pHeader->SetInternVisible(false);
    }

    // 4) 容器尺寸变化后，按需扩缩控件池并强制重排。
    if (m_pProvider != NULL && m_nItemHeight > 0) {
        int nWant = CalcPoolSize();
        if (m_nMaxPoolSize > 0 && nWant > m_nMaxPoolSize) nWant = m_nMaxPoolSize;
        if (nWant > GetElementCount() && m_nMaxPoolSize <= 0) nWant = GetElementCount();
        if (nWant < 1) nWant = 1;
        if (m_pList && m_pList->GetCount() != nWant) {
            EnsurePoolSize(nWant);
            // 池变化后需要重新计算列宽并应用到子项 → 走一次 list 重新布局
            if (m_pList) m_pList->NeedUpdate();
        }
        if (m_pList) m_pList->DoArrange(true);
    }
}

void CVirtualListUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CVerticalLayoutUI::Move(szOffset, bNeedInvalidate);
    if (m_pHeader != NULL && !m_pHeader->IsVisible()) m_pHeader->Move(szOffset, false);
}

void CVirtualListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("header")) == 0) {
        if (m_pHeader) m_pHeader->SetVisible(_tcscmp(pstrValue, _T("hidden")) != 0);
    }
    else if (_tcscmp(pstrName, _T("headerbkimage")) == 0) { if (m_pHeader) m_pHeader->SetBkImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("itemheight")) == 0) {
        int n = _ttoi(pstrValue);
        SetItemHeight(n);
        m_ListInfo.uFixedHeight = n;
    }
    else if (_tcscmp(pstrName, _T("itemfont")) == 0) m_ListInfo.nFont = _ttoi(pstrValue);
    else if (_tcscmp(pstrName, _T("itemalign")) == 0) {
        if (_tcsstr(pstrValue, _T("left")) != NULL)   { m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);  m_ListInfo.uTextStyle |= DT_LEFT; }
        if (_tcsstr(pstrValue, _T("center")) != NULL) { m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);    m_ListInfo.uTextStyle |= DT_CENTER; }
        if (_tcsstr(pstrValue, _T("right")) != NULL)  { m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);   m_ListInfo.uTextStyle |= DT_RIGHT; }
    }
    else if (_tcscmp(pstrName, _T("itemvalign")) == 0) {
        if (_tcsstr(pstrValue, _T("top")) != NULL)     { m_ListInfo.uTextStyle &= ~(DT_BOTTOM | DT_VCENTER); m_ListInfo.uTextStyle |= DT_TOP; }
        if (_tcsstr(pstrValue, _T("vcenter")) != NULL) { m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM);     m_ListInfo.uTextStyle |= DT_VCENTER; }
        if (_tcsstr(pstrValue, _T("bottom")) != NULL)  { m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);    m_ListInfo.uTextStyle |= DT_BOTTOM; }
    }
    else if (_tcscmp(pstrName, _T("itemtextcolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetItemTextColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itembkcolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetItemBkColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itembkimage")) == 0) SetItemBkImage(pstrValue);
    else if (_tcscmp(pstrName, _T("itemaltbk")) == 0) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("itemselectedtextcolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetSelectedItemTextColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itemselectedbkcolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetSelectedItemBkColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itemselectedimage")) == 0) SetSelectedItemImage(pstrValue);
    else if (_tcscmp(pstrName, _T("itemhottextcolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetHotItemTextColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itemhotbkcolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetHotItemBkColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itemhotimage")) == 0) SetHotItemImage(pstrValue);
    else if (_tcscmp(pstrName, _T("itemhlinesize")) == 0) SetItemHLineSize(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("itemhlinecolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetItemHLineColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itemvlinesize")) == 0) SetItemVLineSize(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("itemvlinecolor")) == 0) {
        if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL; SetItemVLineColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("itemshowhtml")) == 0) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
    else if (_tcscmp(pstrName, _T("vscrollbar")) == 0) {
        EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0,
                        GetHorizontalScrollBar() != NULL && GetHorizontalScrollBar()->IsVisible());
    }
    else if (_tcscmp(pstrName, _T("hscrollbar")) == 0) {
        EnableScrollBar(GetVerticalScrollBar() != NULL && GetVerticalScrollBar()->IsVisible(),
                        _tcscmp(pstrValue, _T("true")) == 0);
    }
    else CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
}

// ===== 滚动条直通 =====

SIZE CVirtualListUI::GetScrollPos()   const { return m_pList ? m_pList->GetScrollPos()   : CDuiSize(0, 0); }
SIZE CVirtualListUI::GetScrollRange() const { return m_pList ? m_pList->GetScrollRange() : CDuiSize(0, 0); }
void CVirtualListUI::SetScrollPos(SIZE szPos) { if (m_pList) m_pList->SetScrollPos(szPos); }
void CVirtualListUI::LineUp()    { if (m_pList) m_pList->LineUp(); }
void CVirtualListUI::LineDown()  { if (m_pList) m_pList->LineDown(); }
void CVirtualListUI::PageUp()    { if (m_pList) m_pList->PageUp(); }
void CVirtualListUI::PageDown()  { if (m_pList) m_pList->PageDown(); }
void CVirtualListUI::HomeUp()    { if (m_pList) m_pList->HomeUp(); }
void CVirtualListUI::EndDown()   { if (m_pList) m_pList->EndDown(); }
void CVirtualListUI::LineLeft()  { if (m_pList) m_pList->LineLeft(); }
void CVirtualListUI::LineRight() { if (m_pList) m_pList->LineRight(); }
void CVirtualListUI::PageLeft()  { if (m_pList) m_pList->PageLeft(); }
void CVirtualListUI::PageRight() { if (m_pList) m_pList->PageRight(); }
void CVirtualListUI::HomeLeft()  { if (m_pList) m_pList->HomeLeft(); }
void CVirtualListUI::EndRight()  { if (m_pList) m_pList->EndRight(); }
void CVirtualListUI::EnableScrollBar(bool bV, bool bH) { if (m_pList) m_pList->EnableScrollBar(bV, bH); }
CScrollBarUI* CVirtualListUI::GetVerticalScrollBar()   const { return m_pList ? m_pList->GetVerticalScrollBar() : NULL; }
CScrollBarUI* CVirtualListUI::GetHorizontalScrollBar() const { return m_pList ? m_pList->GetHorizontalScrollBar() : NULL; }

// ===== 子项样式 setter/getter =====

UINT CVirtualListUI::GetItemFixedHeight() { return m_ListInfo.uFixedHeight; }
void CVirtualListUI::SetItemFixedHeight(UINT nHeight) { m_ListInfo.uFixedHeight = nHeight; SetItemHeight((int)nHeight); }
UINT CVirtualListUI::GetItemTextStyle() { return m_ListInfo.uTextStyle; }
void CVirtualListUI::SetItemTextStyle(UINT u) { m_ListInfo.uTextStyle = u; NeedUpdate(); }
RECT CVirtualListUI::GetItemTextPadding() const { return m_ListInfo.rcTextPadding; }
void CVirtualListUI::SetItemTextPadding(RECT rc) { m_ListInfo.rcTextPadding = rc; NeedUpdate(); }
DWORD CVirtualListUI::GetItemTextColor() const { return m_ListInfo.dwTextColor; }
void CVirtualListUI::SetItemTextColor(DWORD c) { m_ListInfo.dwTextColor = c; Invalidate(); }
DWORD CVirtualListUI::GetItemBkColor()   const { return m_ListInfo.dwBkColor; }
void CVirtualListUI::SetItemBkColor(DWORD c)   { m_ListInfo.dwBkColor = c; Invalidate(); }
LPCTSTR CVirtualListUI::GetItemBkImage() const { return m_ListInfo.diBk.sDrawString; }
void CVirtualListUI::SetItemBkImage(LPCTSTR p) {
    if (m_ListInfo.diBk.sDrawString == p && m_ListInfo.diBk.pImageInfo != NULL) return;
    m_ListInfo.diBk.Clear();
    m_ListInfo.diBk.sDrawString = p;
    Invalidate();
}
bool CVirtualListUI::IsAlternateBk() const { return m_ListInfo.bAlternateBk; }
void CVirtualListUI::SetAlternateBk(bool b) { m_ListInfo.bAlternateBk = b; Invalidate(); }

DWORD CVirtualListUI::GetSelectedItemTextColor() const { return m_ListInfo.dwSelectedTextColor; }
void CVirtualListUI::SetSelectedItemTextColor(DWORD c) { m_ListInfo.dwSelectedTextColor = c; Invalidate(); }
DWORD CVirtualListUI::GetSelectedItemBkColor() const { return m_ListInfo.dwSelectedBkColor; }
void CVirtualListUI::SetSelectedItemBkColor(DWORD c) { m_ListInfo.dwSelectedBkColor = c; Invalidate(); }
LPCTSTR CVirtualListUI::GetSelectedItemImage() const { return m_ListInfo.diSelected.sDrawString; }
void CVirtualListUI::SetSelectedItemImage(LPCTSTR p) {
    if (m_ListInfo.diSelected.sDrawString == p && m_ListInfo.diSelected.pImageInfo != NULL) return;
    m_ListInfo.diSelected.Clear();
    m_ListInfo.diSelected.sDrawString = p;
    Invalidate();
}
DWORD CVirtualListUI::GetHotItemTextColor() const { return m_ListInfo.dwHotTextColor; }
void CVirtualListUI::SetHotItemTextColor(DWORD c) { m_ListInfo.dwHotTextColor = c; Invalidate(); }
DWORD CVirtualListUI::GetHotItemBkColor() const { return m_ListInfo.dwHotBkColor; }
void CVirtualListUI::SetHotItemBkColor(DWORD c) { m_ListInfo.dwHotBkColor = c; Invalidate(); }
LPCTSTR CVirtualListUI::GetHotItemImage() const { return m_ListInfo.diHot.sDrawString; }
void CVirtualListUI::SetHotItemImage(LPCTSTR p) {
    if (m_ListInfo.diHot.sDrawString == p && m_ListInfo.diHot.pImageInfo != NULL) return;
    m_ListInfo.diHot.Clear();
    m_ListInfo.diHot.sDrawString = p;
    Invalidate();
}
int CVirtualListUI::GetItemHLineSize() const { return m_ListInfo.iHLineSize; }
void CVirtualListUI::SetItemHLineSize(int s) { m_ListInfo.iHLineSize = s; Invalidate(); }
DWORD CVirtualListUI::GetItemHLineColor() const { return m_ListInfo.dwHLineColor; }
void CVirtualListUI::SetItemHLineColor(DWORD c) { m_ListInfo.dwHLineColor = c; Invalidate(); }
int CVirtualListUI::GetItemVLineSize() const { return m_ListInfo.iVLineSize; }
void CVirtualListUI::SetItemVLineSize(int s) { m_ListInfo.iVLineSize = s; Invalidate(); }
DWORD CVirtualListUI::GetItemVLineColor() const { return m_ListInfo.dwVLineColor; }
void CVirtualListUI::SetItemVLineColor(DWORD c) { m_ListInfo.dwVLineColor = c; Invalidate(); }
bool CVirtualListUI::IsItemShowHtml() { return m_ListInfo.bShowHtml; }
void CVirtualListUI::SetItemShowHtml(bool b) {
    if (m_ListInfo.bShowHtml == b) return;
    m_ListInfo.bShowHtml = b;
    NeedUpdate();
}

} // namespace DuiLib
