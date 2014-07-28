#pragma once

inline HBITMAP CreateMyBitmap(HDC hDC, int cx, int cy, COLORREF** pBits)
{
    LPBITMAPINFO lpbiSrc = NULL;
    lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
    if (lpbiSrc == NULL) return NULL;

    lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiSrc->bmiHeader.biWidth = cx;
    lpbiSrc->bmiHeader.biHeight = cy;
    lpbiSrc->bmiHeader.biPlanes = 1;
    lpbiSrc->bmiHeader.biBitCount = 32;
    lpbiSrc->bmiHeader.biCompression = BI_RGB;
    lpbiSrc->bmiHeader.biSizeImage = cx * cy;
    lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biClrUsed = 0;
    lpbiSrc->bmiHeader.biClrImportant = 0;

    HBITMAP hBitmap = CreateDIBSection (hDC, lpbiSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
    delete [] lpbiSrc;
    return hBitmap;
}

class CShadowWnd : public CWindowWnd
{
private:
    bool m_bNeedUpdate;
    CPaintManagerUI m_pm;

public:
    CShadowWnd() { };
    LPCTSTR GetWindowClassName() const { return _T("UIShadow"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void RePaint() 
    {
        RECT rcClient = { 0 };
        ::GetClientRect(m_hWnd, &rcClient);
        DWORD dwWidth = rcClient.right - rcClient.left;
        DWORD dwHeight = rcClient.bottom - rcClient.top;

        HDC hDcPaint = ::GetDC(m_hWnd);
        HDC hDcBackground = ::CreateCompatibleDC(hDcPaint);
        COLORREF* pBackgroundBits;
        HBITMAP hbmpBackground = CreateMyBitmap(hDcPaint, dwWidth, dwHeight, &pBackgroundBits);
        ::ZeroMemory(pBackgroundBits, dwWidth * dwHeight * 4);
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hDcBackground, hbmpBackground);
        CRenderEngine::DrawImageString(hDcBackground, &m_pm, rcClient, rcClient, _T("file='menu_bk.png' corner='40,8,8,8'"), NULL);

        RECT rcWnd = { 0 };
        ::GetWindowRect(m_hWnd, &rcWnd);

        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        POINT ptPos   = { rcWnd.left, rcWnd.top };
        SIZE sizeWnd  = { dwWidth, dwHeight };
        POINT ptSrc   = { 0, 0 };
        UpdateLayeredWindow(m_hWnd, hDcPaint, &ptPos, &sizeWnd, hDcBackground, &ptSrc, 0, &bf, ULW_ALPHA);

        ::SelectObject(hDcBackground, hOldBitmap);
        if( hDcBackground != NULL ) ::DeleteDC(hDcBackground);
        if( hbmpBackground != NULL ) ::DeleteObject(hbmpBackground);
        ::ReleaseDC(m_hWnd, hDcPaint);

        m_bNeedUpdate = false;
    }
    
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) SetTimer(m_hWnd, 9000, 10, NULL);
        else if( uMsg == WM_SIZE ) m_bNeedUpdate = true;
        else if( uMsg == WM_CLOSE ) KillTimer(m_hWnd, 9000);
        else if( uMsg == WM_TIMER ) {
            if( LOWORD(wParam) == 9000 && m_bNeedUpdate == true) {
                if( !::IsIconic(m_hWnd) ) RePaint();
            }
        }

        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }
};

class CMenuWnd : public CWindowWnd, public INotifyUI
{
public:
    CMenuWnd() : m_pOwner(NULL), m_pShadowWnd(NULL) { };

    void Init(CControlUI* pOwner, POINT pt) {
        if( pOwner == NULL ) return;
        m_pOwner = pOwner;
        m_ptPos = pt;
        Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
        HWND hWndParent = m_hWnd;
        while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
        ::ShowWindow(m_hWnd, SW_SHOW);
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
    }

    void AdjustPostion() {
        CDuiRect rcWnd;
        GetWindowRect(m_hWnd, &rcWnd);
        int nWidth = rcWnd.GetWidth();
        int nHeight = rcWnd.GetHeight();
        rcWnd.left = m_ptPos.x;
        rcWnd.top = m_ptPos.y;
        rcWnd.right = rcWnd.left + nWidth;
        rcWnd.bottom = rcWnd.top + nHeight;
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        CDuiRect rcWork = oMonitor.rcWork;

        if( rcWnd.bottom > rcWork.bottom ) {
            if( nHeight >= rcWork.GetHeight() ) {
                rcWnd.top = 0;
                rcWnd.bottom = nHeight;
            }
            else {
                rcWnd.bottom = rcWork.bottom;
                rcWnd.top = rcWnd.bottom - nHeight;
            }
        }
        if( rcWnd.right > rcWork.right ) {
            if( nWidth >= rcWork.GetWidth() ) {
                rcWnd.left = 0;
                rcWnd.right = nWidth;
            }
            else {
                rcWnd.right = rcWork.right;
                rcWnd.left = rcWnd.right - nWidth;
            }
        }
        ::SetWindowPos(m_hWnd, NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    LPCTSTR GetWindowClassName() const { return _T("MenuWnd"); };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("itemselect") ) {
            Close();
        }
        else if( msg.sType == _T("itemclick") ) {
            if( msg.pSender->GetName() == _T("menu_Delete") ) {
                if( m_pOwner ) m_pOwner->GetManager()->SendNotify(m_pOwner, _T("menu_Delete"), 0, 0, true);
            }
        }
    }

    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL)
    {
        if( m_pShadowWnd == NULL ) m_pShadowWnd = new CShadowWnd;
        m_pShadowWnd->Create(hwndParent, _T(""), WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS, WS_EX_LAYERED | WS_EX_TOOLWINDOW, x, y, cx, cy, NULL);

        dwExStyle |= WS_EX_TOOLWINDOW;
        return CWindowWnd::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu);
    }

    void ShowWindow(bool bShow = true, bool bTakeFocus = true)
    {
        if( m_pShadowWnd != NULL ) m_pShadowWnd->ShowWindow(bShow, false);
        CWindowWnd::ShowWindow(bShow, bTakeFocus);
    }

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_pm.Init(m_hWnd);
        CDialogBuilder builder;
        CControlUI* pRoot = builder.Create(_T("menu.xml"), (UINT)0, NULL, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);
        m_pm.SetRoundCorner(3, 3);

        CListUI* pList = static_cast<CListUI*>(m_pOwner);
        int nSel = pList->GetCurSel();
        if( nSel < 0 ) {
            CControlUI* pControl = m_pm.FindControl(_T("menu_Delete"));
            if( pControl ) pControl->SetEnabled(false);
        }

        AdjustPostion();
        return 0;
    }

    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( m_pShadowWnd != NULL ) m_pShadowWnd->Close();
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( (HWND)wParam == m_hWnd ) bHandled = TRUE;
        else if( m_pShadowWnd != NULL && (HWND)wParam == m_pShadowWnd->GetHWND() ) {
            CWindowWnd::HandleMessage(uMsg, wParam, lParam);
            ::SetFocus( m_hWnd );
            bHandled = TRUE;
        }
        else {
            Close();
            bHandled = FALSE;
        }
        return 0;
    }

    LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( wParam == VK_ESCAPE ) Close();
        return 0;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( !::IsIconic(*this) ) {
            CDuiRect rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            HRGN hRgn = ::CreateRectRgn(rcWnd.left + 8, rcWnd.top + 8, rcWnd.right - 8, rcWnd.bottom - 8);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
        if( m_pShadowWnd != NULL ) {
            RECT rcWnd = { 0 };
            ::GetWindowRect(m_hWnd, &rcWnd );
            ::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left,
                rcWnd.bottom -rcWnd.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
        }
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( m_pShadowWnd != NULL ) {
            RECT rcWnd = { 0 };
            ::GetWindowRect(m_hWnd, &rcWnd );
            ::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left,
                rcWnd.bottom -rcWnd.top, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
        }
        bHandled = FALSE;
        return 0;
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch( uMsg ) {
        case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
        case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break; 
        case WM_KEYDOWN:       lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
        case WM_MOUSEWHEEL:    break;
        case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
        case WM_MOVE:          lRes = OnMove(uMsg, wParam, lParam, bHandled); break;
        default:
            bHandled = FALSE;
        }
        if( bHandled ) return lRes;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

public:
    CPaintManagerUI m_pm;
    CControlUI* m_pOwner;
    POINT m_ptPos;
    CShadowWnd* m_pShadowWnd;
};
