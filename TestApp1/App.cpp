// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#ifndef _DWMAPI_H_
typedef struct DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND;

typedef struct tagDWL_MARGINS {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} DWM_MARGINS, *PDWM_MARGINS;

// Window attributes
enum DWMWINDOWATTRIBUTE
{
    DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
    DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
    DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
    DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
    DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
    DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
    DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
    DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
    DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
    DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
    DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
    DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
    DWMWA_CLOAK,                        // [set] Cloak or uncloak the window
    DWMWA_CLOAKED,                      // [get] Gets the cloaked state of the window
    DWMWA_FREEZE_REPRESENTATION,        // [set] Force this window to freeze the thumbnail without live update
    DWMWA_LAST
};

// Non-client rendering policy attribute values
enum DWMNCRENDERINGPOLICY
{
    DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
    DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
    DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
    DWMNCRP_LAST
};

// Values designating how Flip3D treats a given window.
enum DWMFLIP3DWINDOWPOLICY
{
    DWMFLIP3D_DEFAULT,      // Hide or include the window in Flip3D based on window style and visibility.
    DWMFLIP3D_EXCLUDEBELOW, // Display the window under Flip3D and disabled.
    DWMFLIP3D_EXCLUDEABOVE, // Display the window above Flip3D and enabled.
    DWMFLIP3D_LAST
};

#define DWM_BB_ENABLE                 0x00000001
#define DWM_BB_BLURREGION             0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004

#define DWM_EC_DISABLECOMPOSITION     0x00000000
#define DWM_EC_ENABLECOMPOSITION      0x00000001
#endif // _DWMAPI_H_

class CDwm
{
public:
    typedef HRESULT (WINAPI *FNDWMENABLECOMPOSITION)(UINT);
    typedef HRESULT (WINAPI *FNDWNISCOMPOSITIONENABLED)(LPBOOL);
    typedef HRESULT (WINAPI *FNENABLEBLURBEHINDWINDOW)(HWND, CONST DWM_BLURBEHIND*);
    typedef HRESULT (WINAPI *FNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND, CONST DWM_MARGINS*);
    typedef HRESULT (WINAPI *FNDWMSETWINDOWATTRIBUTE)(HWND, DWORD, LPCVOID pvAttribute, DWORD);

    FNDWMENABLECOMPOSITION fnDwmEnableComposition;
    FNDWNISCOMPOSITIONENABLED fnDwmIsCompositionEnabled;
    FNENABLEBLURBEHINDWINDOW fnDwmEnableBlurBehindWindow;
    FNDWMEXTENDFRAMEINTOCLIENTAREA fnDwmExtendFrameIntoClientArea;
    FNDWMSETWINDOWATTRIBUTE fnDwmSetWindowAttribute;

    CDwm()
    {
        static HINSTANCE hDwmInstance = ::LoadLibrary(_T("dwmapi.dll"));
        if( hDwmInstance != NULL ) {
            fnDwmEnableComposition = (FNDWMENABLECOMPOSITION) ::GetProcAddress(hDwmInstance, "DwmEnableComposition");
            fnDwmIsCompositionEnabled = (FNDWNISCOMPOSITIONENABLED) ::GetProcAddress(hDwmInstance, "DwmIsCompositionEnabled");
            fnDwmEnableBlurBehindWindow = (FNENABLEBLURBEHINDWINDOW) ::GetProcAddress(hDwmInstance, "DwmEnableBlurBehindWindow");
            fnDwmExtendFrameIntoClientArea = (FNDWMEXTENDFRAMEINTOCLIENTAREA) ::GetProcAddress(hDwmInstance, "DwmExtendFrameIntoClientArea");
            fnDwmSetWindowAttribute = (FNDWMSETWINDOWATTRIBUTE) ::GetProcAddress(hDwmInstance, "DwmSetWindowAttribute");
        }
        else {
            fnDwmEnableComposition = NULL;
            fnDwmIsCompositionEnabled = NULL;
            fnDwmEnableBlurBehindWindow = NULL;
            fnDwmExtendFrameIntoClientArea = NULL;
            fnDwmSetWindowAttribute = NULL;
        }
    }

    BOOL IsCompositionEnabled() const
    {
        HRESULT Hr = E_NOTIMPL;
        BOOL bRes = FALSE;
        if( fnDwmIsCompositionEnabled != NULL ) Hr = fnDwmIsCompositionEnabled(&bRes);
        return SUCCEEDED(Hr) && bRes;
    }

    BOOL EnableComposition(UINT fEnable)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableComposition != NULL ) bRes = SUCCEEDED(fnDwmEnableComposition(fEnable));
        return bRes;
    }

    BOOL EnableBlurBehindWindow(HWND hWnd)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableBlurBehindWindow != NULL ) {
            DWM_BLURBEHIND bb = { 0 };
            bb.dwFlags = DWM_BB_ENABLE;
            bb.fEnable = TRUE;
            bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
        }
        return bRes;
    }

    BOOL EnableBlurBehindWindow(HWND hWnd, CONST DWM_BLURBEHIND& bb)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableBlurBehindWindow != NULL ) {
            bRes = SUCCEEDED(fnDwmEnableBlurBehindWindow(hWnd, &bb));
        }
        return bRes;
    }

    BOOL ExtendFrameIntoClientArea(HWND hWnd, CONST DWM_MARGINS& Margins)
    {
        BOOL bRes = FALSE;
        if( fnDwmEnableComposition != NULL ) bRes = SUCCEEDED(fnDwmExtendFrameIntoClientArea(hWnd, &Margins));
        return bRes;
    }

    BOOL SetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
    {
        BOOL bRes = FALSE;
        if( fnDwmSetWindowAttribute != NULL ) bRes = SUCCEEDED(fnDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute));
        return bRes;
    }
};

#ifndef WM_DPICHANGED
#   define WM_DPICHANGED       0x02E0
#endif


#ifndef _SHELLSCALINGAPI_H_
typedef enum _PROCESS_DPI_AWARENESS { 
    PROCESS_DPI_UNAWARE            = 0,
    PROCESS_SYSTEM_DPI_AWARE       = 1,
    PROCESS_PER_MONITOR_DPI_AWARE  = 2
} PROCESS_DPI_AWARENESS;

typedef enum _MONITOR_DPI_TYPE { 
    MDT_EFFECTIVE_DPI  = 0,
    MDT_ANGULAR_DPI    = 1,
    MDT_RAW_DPI        = 2,
    MDT_DEFAULT        = MDT_EFFECTIVE_DPI
} Monitor_DPI_Type;
#endif // _SHELLSCALINGAPI_H_

class CDPI
{
public:
    typedef BOOL (WINAPI *FNSETPROCESSDPIAWARE)(VOID);
    typedef HRESULT (WINAPI *FNSETPROCESSDPIAWARENESS)(PROCESS_DPI_AWARENESS);
    typedef HRESULT (WINAPI *FNGETDPIFORMONITOR)(HMONITOR, Monitor_DPI_Type, UINT*, UINT*);

    FNSETPROCESSDPIAWARE fnSetProcessDPIAware; // vista,win7
    FNSETPROCESSDPIAWARENESS fnSetProcessDpiAwareness; // win8+
    FNGETDPIFORMONITOR fnGetDpiForMonitor; //

    CDPI() {
        m_nScaleFactor = 0;
        m_nScaleFactorSDA = 0;
        m_Awareness = PROCESS_DPI_UNAWARE;

        static HINSTANCE hUser32Instance = ::LoadLibrary(_T("User32.dll"));
        static HINSTANCE hShcoreInstance = ::LoadLibrary(_T("Shcore.dll"));
        if( hUser32Instance != NULL ) {
            fnSetProcessDPIAware = (FNSETPROCESSDPIAWARE) ::GetProcAddress(hUser32Instance, "SetProcessDPIAware");
        }
        else {
            fnSetProcessDPIAware = NULL;
        }

        if( hShcoreInstance != NULL ) {
            fnSetProcessDpiAwareness = (FNSETPROCESSDPIAWARENESS) ::GetProcAddress(hShcoreInstance, "SetProcessDpiAwareness");
            fnGetDpiForMonitor = (FNGETDPIFORMONITOR) ::GetProcAddress(hShcoreInstance, "GetDpiForMonitor");
        }
        else {
            fnSetProcessDpiAwareness = NULL;
            fnGetDpiForMonitor = NULL;
        }

        if (fnGetDpiForMonitor != NULL) {
            UINT     dpix = 0, dpiy = 0;
            HRESULT  hr = E_FAIL;
            POINT pt = {1, 1};
            HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            hr = fnGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
            SetScale(dpix);
        }
        else {
            UINT     dpix = 0;
            HDC hDC = ::GetDC(::GetDesktopWindow());
            dpix = GetDeviceCaps(hDC, LOGPIXELSX);
            ::ReleaseDC(::GetDesktopWindow(), hDC);
            SetScale(dpix);
        }

        SetAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    }

    PROCESS_DPI_AWARENESS GetAwareness()
    {
        return m_Awareness;
    }

    int  Scale(int x)
    {
        if (m_Awareness == PROCESS_DPI_UNAWARE) return x;
        if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) return MulDiv(x, m_nScaleFactorSDA, 100);
        return MulDiv(x, m_nScaleFactor, 100); // PROCESS_PER_MONITOR_DPI_AWARE
    }

    UINT GetScale()
    {
        if (m_Awareness == PROCESS_DPI_UNAWARE) return 100;
        if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) return m_nScaleFactorSDA;
        return m_nScaleFactor;
    }

    void ScaleRect(__inout RECT *pRect)
    {
        pRect->left = Scale(pRect->left);
        pRect->right = Scale(pRect->right);
        pRect->top = Scale(pRect->top);
        pRect->bottom = Scale(pRect->bottom);
    }

    void ScalePoint(__inout POINT *pPoint)
    {
        pPoint->x = Scale(pPoint->x);
        pPoint->y = Scale(pPoint->y);
    }
    
    void OnDPIChanged(HWND hWnd, WPARAM wParam, LPARAM lParam) 
    {
        SetScale(LOWORD(wParam));
        RECT* const prcNewWindow = (RECT*)lParam;
        ::SetWindowPos(hWnd,
            NULL,
            prcNewWindow ->left,
            prcNewWindow ->top,
            prcNewWindow->right - prcNewWindow->left,
            prcNewWindow->bottom - prcNewWindow->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }

private:
    UINT m_nScaleFactor;
    UINT m_nScaleFactorSDA;
    PROCESS_DPI_AWARENESS m_Awareness;

    BOOL SetAwareness(PROCESS_DPI_AWARENESS value) 
    {
        if( fnSetProcessDpiAwareness != NULL ) {
            HRESULT Hr = E_NOTIMPL;
            Hr = fnSetProcessDpiAwareness(value);
            if (Hr == S_OK) {
                m_Awareness = value;
                return TRUE;
            }
            else {
                return FALSE;
            }
        }
        else {
            if (fnSetProcessDPIAware) {
                BOOL bRet = fnSetProcessDPIAware();
                if (bRet) m_Awareness = PROCESS_SYSTEM_DPI_AWARE;
                return bRet;
            }
        }
        return FALSE;
    }

    void SetScale(__in UINT iDPI)
    {
        m_nScaleFactor = MulDiv(iDPI, 100, 96);
        if (m_nScaleFactorSDA == 0) m_nScaleFactorSDA = m_nScaleFactor;
    }
};


class CFrameWindowWnd : public CWindowWnd, public INotifyUI, public CDwm, public CDPI
{
public:
    CFrameWindowWnd() : m_pWndShadow(NULL) { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void Init() { }

    bool OnHChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), S, L);
        }
        return true;
    }

    bool OnSChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, H, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue(), L);
        }
        return true;
    }

    bool OnLChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            short H, S, L;
            CPaintManagerUI::GetHSL(&H, &S, &L);
            CPaintManagerUI::SetHSL(true, H, S, (static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        }
        return true;
    }

    bool OnAlphaChanged(void* param) {
        TNotifyUI* pMsg = (TNotifyUI*)param;
        if( pMsg->sType == _T("valuechanged") ) {
            m_pm.SetOpacity((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
        }
        return true;
    }

    void OnPrepare() 
    {
        CSliderUI* pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("alpha_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnAlphaChanged);
        pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("h_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnHChanged);
        pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("s_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnSChanged);
        pSilder = static_cast<CSliderUI*>(m_pm.FindControl(_T("l_controlor")));
        if( pSilder ) pSilder->OnNotify += MakeDelegate(this, &CFrameWindowWnd::OnLChanged);
    }

    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
        else if( msg.sType == _T("click") ) {
            if( msg.pSender->GetName() == _T("insertimagebtn") ) {
                CRichEditUI* pRich = static_cast<CRichEditUI*>(m_pm.FindControl(_T("testrichedit")));
                if( pRich ) {
                    pRich->RemoveAll();
                }
            }
            else if( msg.pSender->GetName() == _T("changeskinbtn") ) {
                if( CPaintManagerUI::GetResourcePath() == CPaintManagerUI::GetInstancePath() )
                    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\FlashRes"));
                else
                    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
                CPaintManagerUI::ReloadSkin();
            }
        }
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if( uMsg == WM_CREATE ) {
            m_pm.Init(m_hWnd);
            CDialogBuilder builder;
            CControlUI* pRoot = builder.Create(_T("test1.xml"), (UINT)0, NULL, &m_pm);
            ASSERT(pRoot && "Failed to parse XML");
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);

            m_pWndShadow = new CWndShadow;
            m_pWndShadow->Create(m_hWnd);
            RECT rcCorner = {3,3,4,4};
            RECT rcHoleOffset = {0,0,0,0};
            m_pWndShadow->SetImage(_T("LeftWithFill.png"), rcCorner, rcHoleOffset);

            DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
            SetWindowAttribute(m_hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &ncrp, sizeof(ncrp));

            //DWM_BLURBEHIND bb = {0};
            //bb.dwFlags = DWM_BB_ENABLE;
            //bb.fEnable = true;
            //bb.hRgnBlur = NULL;
            //EnableBlurBehindWindow(m_hWnd, bb);

            //DWM_MARGINS margins = {-1}/*{0,0,0,25}*/;
            //ExtendFrameIntoClientArea(m_hWnd, margins);

            Init();
            return 0;
        }
        else if( uMsg == WM_DESTROY ) {
            ::PostQuitMessage(0L);
        }
        else if( uMsg == WM_NCACTIVATE ) {
            if( !::IsIconic(*this) ) return (wParam == 0) ? TRUE : FALSE;
        }
        LRESULT lRes = 0;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

public:
    CPaintManagerUI m_pm;
    CWndShadow* m_pWndShadow;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CWndShadow::Initialize(hInstance);

    CFrameWindowWnd* pFrame = new CFrameWindowWnd();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T("这是一个最简单的测试用exe，修改test1.xml就可以看到效果"), UI_WNDSTYLE_FRAME|WS_CLIPCHILDREN, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
