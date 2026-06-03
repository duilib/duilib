#ifndef __UIVIRTUALLIST_H__
#define __UIVIRTUALLIST_H__

#pragma once
#include <vector>


namespace DuiLib {

class CVirtualListUI;
class CVirtualListBodyUI;

/// 虚表数据代理接口：开发者实现该接口，把数据填充到子项控件上。
/// CreateElement   返回一个新建的子项控件（一般是 CListHBoxElementUI 等）。
///                 控件由虚表内部管理生命周期，开发者不要自行 delete。
/// FillElement     把 iIndex 对应的真实数据填充到 pControl 上；该 pControl
///                 是由 CreateElement 创建过的控件，会被复用。
/// GetElementCount 返回逻辑子项总数（不是池中控件数）。
class IVirtualListProvider
{
public:
    virtual ~IVirtualListProvider() {}
    virtual CControlUI* CreateElement() = 0;
    virtual void FillElement(CControlUI* pControl, int iIndex) = 0;
    virtual int GetElementCount() = 0;
};

/// 虚表的内容容器（List Body）。
/// 负责按当前滚动位置摆放控件池中的实际子控件，并在水平滚动时同步表头位置。
class DUILIB_API CVirtualListBodyUI : public CVerticalLayoutUI
{
public:
    explicit CVirtualListBodyUI(CVirtualListUI* pOwner);

    CVirtualListUI* GetOwner() const { return m_pOwner; }

    void SetScrollPos(SIZE szPos);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoEvent(TEventUI& event);
    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

    /// 重新摆放/填充池中可见控件。
    /// bForce: 即使没有滚动也强制重排（如表头列宽变化、容器尺寸变化、首次显示）。
    void DoArrange(bool bForce);

    /// 强制全量刷新池中全部子项的数据填充。
    void RefillAll();

protected:
    CVirtualListUI* m_pOwner;
    int  m_iOldYScrollPos;
    bool m_bArrangedOnce;
};

/// 虚表控件。提供与 CListUI 类似的能力（表头、列、滚动条），但只创建少量
/// 子控件循环复用，能流畅承载几十万行数据。
/// 通过 SetDataProvider/SetItemHeight/InitElement 启动；UI 上仍可使用
/// ListHeader / ListHeaderItem / ListHBoxElement 等已有控件描述列。
class DUILIB_API CVirtualListUI : public CVerticalLayoutUI, public IListUI
{
    friend class CVirtualListBodyUI;

public:
    CVirtualListUI();
    virtual ~CVirtualListUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    // ====== 虚表特性接口 ======
    void SetDataProvider(IVirtualListProvider* pProvider);
    IVirtualListProvider* GetDataProvider() const;

    /// 必填：每行高度（必须 > 0），在 InitElement/Refresh 之前设置。
    void SetItemHeight(int nHeight);
    int  GetItemHeight() const;

    /// 初始化控件池。第一次必须显式调用，nMaxPoolItem 是池上限；
    /// 不传则虚表会按当前可见行数自动计算（推荐）。
    void InitElement(int nMaxPoolItem = -1);

    /// 数据集发生变化（增删改）后调用，按需要扩缩池并重排重填。
    void Refresh();

    /// 获取数据集元素总数（来自 Provider）。
    int  GetElementCount() const;

    /// 让指定数据下标的项可见；bToTop=true 时尽量置顶。
    void EnsureVisible(int iIndex, bool bToTop = false);

    /// 当前可见数据下标列表。
    void GetDisplayCollection(std::vector<int>& collection);

    /// 选中某一行（基于数据下标）。
    void SetCurSelDataIndex(int iDataIndex, bool bTriggerEvent = true);
    int  GetCurSelDataIndex() const { return m_iCurSelDataIdx; }

    // ====== IListUI ======
    CListHeaderUI* GetHeader() const;
    CContainerUI*  GetList() const;
    IListCallbackUI* GetTextCallback() const;
    void SetTextCallback(IListCallbackUI* pCallback);

    // ====== IListOwnerUI ======
    TListInfoUI* GetListInfo();
    int  GetCurSel() const;
    bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true);
    void DoEvent(TEventUI& event);
    bool ExpandItem(int /*iIndex*/, bool /*bExpand*/ = true) { return false; }
    int  GetExpandedItem() const { return -1; }

    // ====== 子项样式 ======
    UINT GetItemFixedHeight();
    void SetItemFixedHeight(UINT nHeight);
    UINT GetItemTextStyle();
    void SetItemTextStyle(UINT uStyle);
    RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
    DWORD GetItemTextColor() const;
    void  SetItemTextColor(DWORD dwTextColor);
    DWORD GetItemBkColor() const;
    void  SetItemBkColor(DWORD dwBkColor);
    LPCTSTR GetItemBkImage() const;
    void    SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
    DWORD GetSelectedItemTextColor() const;
    void  SetSelectedItemTextColor(DWORD dwTextColor);
    DWORD GetSelectedItemBkColor() const;
    void  SetSelectedItemBkColor(DWORD dwBkColor);
    LPCTSTR GetSelectedItemImage() const;
    void    SetSelectedItemImage(LPCTSTR pStrImage);
    DWORD GetHotItemTextColor() const;
    void  SetHotItemTextColor(DWORD dwTextColor);
    DWORD GetHotItemBkColor() const;
    void  SetHotItemBkColor(DWORD dwBkColor);
    LPCTSTR GetHotItemImage() const;
    void    SetHotItemImage(LPCTSTR pStrImage);
    int  GetItemHLineSize() const;
    void SetItemHLineSize(int iSize);
    DWORD GetItemHLineColor() const;
    void  SetItemHLineColor(DWORD dwLineColor);
    int  GetItemVLineSize() const;
    void SetItemVLineSize(int iSize);
    DWORD GetItemVLineColor() const;
    void  SetItemVLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    // ====== 容器/布局 ======
    // 重写 Add/AddAt/Remove 让 XML 中的 ListHeader/ListHeaderItem 能替换/挂到 m_pHeader 上。
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl, bool bDoNotDestroy = false);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    SIZE GetScrollPos() const;
    SIZE GetScrollRange() const;
    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();
    void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = true);
    virtual CScrollBarUI* GetVerticalScrollBar() const;
    virtual CScrollBarUI* GetHorizontalScrollBar() const;

protected:
    /// 根据当前可见区域和 ItemHeight 计算池建议大小（可见行数 + 1 缓冲）。
    int CalcPoolSize() const;
    /// 实际建池：按需 Add/Remove 池中控件。
    void EnsurePoolSize(int nWanted);
    /// 内部填充：把 iDataIndex 数据填到 pControl 上，并维护选中状态。
    void FillElement(CControlUI* pControl, int iDataIndex);
    /// 创建一个池控件。
    CControlUI* CreateElement();

protected:
    CVirtualListBodyUI* m_pList;
    CListHeaderUI*      m_pHeader;
    TListInfoUI         m_ListInfo;

    IListCallbackUI*    m_pCallback;
    IVirtualListProvider* m_pProvider;

    int  m_nItemHeight;       // 每个子项的固定高度
    int  m_nMaxPoolSize;      // 控件池上限（<=0 表示由控件自动按可见区域决定）
    int  m_iCurSelDataIdx;    // 当前选中的数据下标（基于数据，跨滚动保留）
    bool m_bInArrange;    // true while DoArrange is running, prevents EnsureVisible from fighting scroll
};

} // namespace DuiLib

#endif // __UIVIRTUALLIST_H__
