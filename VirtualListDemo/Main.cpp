// VirtualListDemo - 基于 duilib 自实现的虚表控件演示。
//   - 数据 Provider 持有 1 ~ 100000 行数据
//   - VirtualList 控件只创建 ~可见行 + 1 个真实子控件
//   - 表头列宽可拖动，下方表内容会随动
//   - 数据规模发生变化时自动显示/隐藏滚动条
//   - 表高度不是子项高度整数倍时无渲染缺失（最后一行部分可见）
//
// 编译前确保已经按"集成步骤"将 UIVirtualList.cpp/h 加入 DuiLib 工程。

#include <objbase.h>
#include <vector>
#include <sstream>

#include "..\\DuiLib\\UIlib.h"

using namespace DuiLib;

// =============================================================================
// 数据模型
// =============================================================================

struct RowData
{
    int     iIndex;
    CDuiString sName;
    int     iAge;
    CDuiString sDept;
    CDuiString sJoinDate;
    CDuiString sRemark;
};

class TableDataProvider : public IVirtualListProvider
{
public:
    void Generate(int nCount)
    {
        m_data.clear();
        m_data.reserve(nCount);
        for (int i = 0; i < nCount; ++i) {
            RowData r;
            r.iIndex = i;
            CDuiString s;
            s.Format(_T("员工_%05d"), i);                    r.sName = s;
            r.iAge = 20 + (i % 40);
            const TCHAR* depts[] = { _T("研发部"), _T("产品部"), _T("市场部"), _T("人事部"), _T("财务部"), _T("设计部") };
            r.sDept = depts[i % 6];
            s.Format(_T("2020-%02d-%02d"), 1 + (i % 12), 1 + (i % 28));  r.sJoinDate = s;
            s.Format(_T("第 %d 条数据，用于检验大量数据下虚表的性能与正确性"), i);
            r.sRemark = s;
            m_data.push_back(r);
        }
    }

    void Clear() { m_data.clear(); }

    // ============ IVirtualListProvider ============
    virtual CControlUI* CreateElement()
    {
        // 一行 = CListHBoxElement，内嵌 6 个 CLabel 列控件，
        // 列的真正 X/宽度由 CListHBoxElement::SetPos 用 ListInfo->rcColumn 决定，
        // 所以这里的子控件顺序必须与表头列顺序一致。
        CListHBoxElementUI* pRow = new CListHBoxElementUI;
        for (int i = 0; i < 6; ++i) {
            CLabelUI* pCell = new CLabelUI;
            pCell->SetAttribute(_T("align"), (i == 0 || i == 2) ? _T("center") : _T("left"));
            pCell->SetAttribute(_T("valign"), _T("vcenter"));
            pCell->SetAttribute(_T("textpadding"), _T("8,0,8,0"));
            pCell->SetAttribute(_T("endellipsis"), _T("true"));
            pRow->Add(pCell);
        }
        return pRow;
    }

    virtual void FillElement(CControlUI* pControl, int iIndex)
    {
        if (iIndex < 0 || iIndex >= (int)m_data.size()) return;
        const RowData& r = m_data[iIndex];
        CListHBoxElementUI* pRow = static_cast<CListHBoxElementUI*>(pControl);
        if (pRow->GetCount() < 6) return;

        CDuiString s;
        s.Format(_T("%d"), r.iIndex);
        static_cast<CLabelUI*>(pRow->GetItemAt(0))->SetText(s);
        static_cast<CLabelUI*>(pRow->GetItemAt(1))->SetText(r.sName);
        s.Format(_T("%d"), r.iAge);
        static_cast<CLabelUI*>(pRow->GetItemAt(2))->SetText(s);
        static_cast<CLabelUI*>(pRow->GetItemAt(3))->SetText(r.sDept);
        static_cast<CLabelUI*>(pRow->GetItemAt(4))->SetText(r.sJoinDate);
        static_cast<CLabelUI*>(pRow->GetItemAt(5))->SetText(r.sRemark);
    }

    virtual int GetElementCount() { return (int)m_data.size(); }

private:
    std::vector<RowData> m_data;
};

// =============================================================================
// 主窗口
// =============================================================================

class VirtualListMainForm : public CWindowWnd, public INotifyUI
{
public:
    VirtualListMainForm() : m_pTable(NULL), m_pInfo(NULL) {}

    LPCTSTR GetWindowClassName() const { return _T("VirtualListMainForm"); }
    UINT    GetClassStyle() const      { return CS_DBLCLKS; }
    void    OnFinalMessage(HWND) { delete this; }

    void Init()
    {
        m_pTable = static_cast<CVirtualListUI*>(m_pm.FindControl(_T("bigtable")));
        m_pInfo  = static_cast<CTextUI*>(m_pm.FindControl(_T("info")));
        ASSERT(m_pTable && "skin.xml: VirtualList name='bigtable' not found");

        m_pTable->SetDataProvider(&m_provider);
        m_pTable->SetItemHeight(34);
        // 默认加载 10 万行，体现虚表的优势
        //LoadRows(0);
    }

    void LoadRows(int n)
    {
        m_provider.Generate(n);
        m_pTable->Refresh();
        UpdateInfo();
    }

    void UpdateInfo()
    {
        if (!m_pInfo) return;
        CDuiString s;
        s.Format(_T("当前数据行数: %d   选中: %d"),
                 m_pTable->GetElementCount(), m_pTable->GetCurSelDataIndex());
        m_pInfo->SetText(s);
    }

    // ============ INotifyUI ============
    void Notify(TNotifyUI& msg)
    {
        if (msg.sType == _T("click")) {
            CDuiString name = msg.pSender->GetName();
            if (name == _T("closebtn")) { PostQuitMessage(0); return; }
            if (name == _T("minbtn"))   { SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
            if (name == _T("btn_load_100k")) { LoadRows(100000); return; }
            if (name == _T("btn_load_50"))   { LoadRows(50);     return; }
            if (name == _T("btn_clear"))     { LoadRows(0);      return; }
            if (name == _T("btn_jump_top"))  {
                if (m_pTable->GetElementCount() > 0) {
                    m_pTable->EnsureVisible(0, true);
                    m_pTable->SetCurSelDataIndex(0);
                    UpdateInfo();
                }
                return;
            }
            if (name == _T("btn_jump_88888")) {
                if (m_pTable->GetElementCount() > 88888) {
                    m_pTable->EnsureVisible(88888, true);
                    m_pTable->SetCurSelDataIndex(88888);
                    UpdateInfo();
                }
                return;
            }
        }
        else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
            UpdateInfo();
        }
        else if (msg.sType == DUI_MSGTYPE_ITEMCLICK) {
            UpdateInfo();
        }
    }

    // ============ Window 消息 ============
    LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        LONG style = ::GetWindowLong(*this, GWL_STYLE);
        style &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        m_pm.Init(m_hWnd);
        CDialogBuilder builder;
        CControlUI* pRoot = builder.Create(_T("skin.xml"), (UINT)0, NULL, &m_pm);
        ASSERT(pRoot && "Failed to parse skin.xml");
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);
        Init();
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnNcHitTest(UINT, WPARAM, LPARAM lParam, BOOL&)
    {
        POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(*this, &pt);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);

        if (!::IsZoomed(*this)) {
            RECT rcSizeBox = m_pm.GetSizeBox();
            if (pt.y < rcClient.top + rcSizeBox.top) {
                if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
                if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
                return HTTOP;
            }
            else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
                if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
                if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
                return HTBOTTOM;
            }
            if (pt.x < rcClient.left + rcSizeBox.left)  return HTLEFT;
            if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
        }

        RECT rcCaption = m_pm.GetCaptionRect();
        if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right
            && pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
            CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
            if (pControl
                && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0
                && _tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0
                && _tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
                return HTCAPTION;
        }
        return HTCLIENT;
    }

    LRESULT OnNcCalcSize(UINT, WPARAM, LPARAM, BOOL&)    { return 0; }
    LRESULT OnNcPaint(UINT, WPARAM, LPARAM, BOOL&)       { return 0; }
    LRESULT OnNcActivate(UINT, WPARAM, LPARAM wParam, BOOL& bHandled)
    {
        if (::IsIconic(*this)) bHandled = FALSE;
        return (wParam == 0) ? TRUE : FALSE;
    }
    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        ::PostQuitMessage(0);
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL& bHandled)
    {
        SIZE szRoundCorner = m_pm.GetRoundCorner();
        if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)) {
            CDuiRect rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            rcWnd.right++; rcWnd.bottom++;
            HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == SC_CLOSE) {
            ::PostQuitMessage(0);
            bHandled = TRUE;
            return 0;
        }
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch (uMsg) {
        case WM_CREATE:     lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_DESTROY:    lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
        case WM_NCACTIVATE: lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
        case WM_NCCALCSIZE: lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
        case WM_NCPAINT:    lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
        case WM_NCHITTEST:  lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
        case WM_SIZE:       lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
        case WM_SYSCOMMAND: lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
        default:
            bHandled = FALSE;
        }
        if (bHandled) return lRes;
        if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

private:
    CPaintManagerUI    m_pm;
    CVirtualListUI*    m_pTable;
    CTextUI*           m_pInfo;
    TableDataProvider  m_provider;
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\VirtualListRes"));

    VirtualListMainForm* pFrame = new VirtualListMainForm();
    pFrame->Create(NULL, _T("VirtualListDemo"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 1000, 600);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();
    return 0;
}
