#include "StdAfx.h"
#include <zmouse.h>
#include <stdlib.h>

DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT MapKeyState()
{
    UINT uState = 0;
    if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
    if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_RBUTTON;
    if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_LBUTTON;
    if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
    if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
    return uState;
}

typedef struct tagFINDTABINFO
{
    CControlUI* pFocus;
    CControlUI* pLast;
    bool bForward;
    bool bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
    TCHAR ch;
    bool bPickNext;
} FINDSHORTCUT;

typedef struct tagTIMERINFO
{
    CControlUI* pSender;
    UINT nLocalID;
    HWND hWnd;
    UINT uWinTimer;
    bool bKilled;
} TIMERINFO;

/////////////////////////////////////////////////////////////////////////////////////

tagTDrawInfo::tagTDrawInfo()
{
	Clear();
}

tagTDrawInfo::tagTDrawInfo(LPCTSTR lpsz)
{
	Clear();
	sDrawString = lpsz;
}

void tagTDrawInfo::Clear()
{
	sDrawString.Empty();
	::ZeroMemory(&bLoaded, sizeof(tagTDrawInfo) - offsetof(tagTDrawInfo, bLoaded));
	uFade = 255;
}

/////////////////////////////////////////////////////////////////////////////////////

HPEN m_hUpdateRectPen = NULL;

HINSTANCE CPaintManagerUI::m_hResourceInstance = NULL;
CDuiString CPaintManagerUI::m_pStrResourcePath;
CDuiString CPaintManagerUI::m_pStrResourceZip;
HANDLE CPaintManagerUI::m_hResourceZip = NULL;
bool CPaintManagerUI::m_bCachedResourceZip = true;
TResInfo CPaintManagerUI::m_SharedResInfo;
HINSTANCE CPaintManagerUI::m_hInstance = NULL;
bool CPaintManagerUI::m_bUseHSL = false;
short CPaintManagerUI::m_H = 180;
short CPaintManagerUI::m_S = 100;
short CPaintManagerUI::m_L = 100;
CStdPtrArray CPaintManagerUI::m_aPreMessages;
CStdPtrArray CPaintManagerUI::m_aPlugins;

CPaintManagerUI::CPaintManagerUI() :
m_hWndPaint(NULL),
m_hDcPaint(NULL),
m_hDcOffscreen(NULL),
m_hDcBackground(NULL),
m_hbmpOffscreen(NULL),
m_hbmpBackground(NULL),
m_hwndTooltip(NULL),
m_bShowUpdateRect(false),
m_uTimerID(0x1000),
m_pRoot(NULL),
m_pFocus(NULL),
m_pEventHover(NULL),
m_pEventClick(NULL),
m_pEventKey(NULL),
m_bFirstLayout(true),
m_bFocusNeeded(false),
m_bUpdateNeeded(false),
m_bMouseTracking(false),
m_bMouseCapture(false),
m_bOffscreenPaint(true),
m_bAlphaBackground(false),
m_bUsedVirtualWnd(false),
m_nOpacity(255)
{
	if (m_SharedResInfo.m_DefaultFontInfo.sFontName.IsEmpty())
	{
		m_SharedResInfo.m_dwDefaultDisabledColor = 0xFFA7A6AA;
		m_SharedResInfo.m_dwDefaultFontColor = 0xFF000000;
		m_SharedResInfo.m_dwDefaultLinkFontColor = 0xFF0000FF;
		m_SharedResInfo.m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
		m_SharedResInfo.m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		lf.lfCharSet = DEFAULT_CHARSET;
		HFONT hDefaultFont = ::CreateFontIndirect(&lf);
		m_SharedResInfo.m_DefaultFontInfo.hFont = hDefaultFont;
		m_SharedResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
		m_SharedResInfo.m_DefaultFontInfo.iSize = -lf.lfHeight;
		m_SharedResInfo.m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
		m_SharedResInfo.m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
		m_SharedResInfo.m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
		::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
	}

	m_ResInfo.m_dwDefaultDisabledColor = m_SharedResInfo.m_dwDefaultDisabledColor;
	m_ResInfo.m_dwDefaultFontColor = m_SharedResInfo.m_dwDefaultFontColor;
	m_ResInfo.m_dwDefaultLinkFontColor = m_SharedResInfo.m_dwDefaultLinkFontColor;
	m_ResInfo.m_dwDefaultLinkHoverFontColor = m_SharedResInfo.m_dwDefaultLinkHoverFontColor;
	m_ResInfo.m_dwDefaultSelectedBkColor = m_SharedResInfo.m_dwDefaultSelectedBkColor;

    if( m_hUpdateRectPen == NULL ) {
        m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
        // Boot Windows Common Controls (for the ToolTip control)
        ::InitCommonControls();
        ::LoadLibrary(_T("msimg32.dll"));
    }

    m_szMinWindow.cx = 0;
    m_szMinWindow.cy = 0;
    m_szMaxWindow.cx = 0;
    m_szMaxWindow.cy = 0;
    m_szInitWindowSize.cx = 0;
    m_szInitWindowSize.cy = 0;
    m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
    ::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
    ::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
    m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
}

CPaintManagerUI::~CPaintManagerUI()
{
    // Delete the control-tree structures
    for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
    for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) delete static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
    m_mNameHash.Resize(0);
    delete m_pRoot;

    ::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
    RemoveAllFonts();
    RemoveAllImages();
    RemoveAllDefaultAttributeList();
    RemoveAllOptionGroups();
    RemoveAllTimers();

    // Reset other parts...
    if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
    if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
    if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
    if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
    if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
    if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
    m_aPreMessages.Remove(m_aPreMessages.Find(this));
}

void CPaintManagerUI::Init(HWND hWnd)
{
    ASSERT(::IsWindow(hWnd));
    // Remember the window context we came from
    m_hWndPaint = hWnd;
    m_hDcPaint = ::GetDC(hWnd);
    // We'll want to filter messages globally too
    m_aPreMessages.Add(this);
}

HINSTANCE CPaintManagerUI::GetInstance()
{
    return m_hInstance;
}

CDuiString CPaintManagerUI::GetInstancePath()
{
    if( m_hInstance == NULL ) return _T('\0');
    
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
    CDuiString sInstancePath = tszModule;
    int pos = sInstancePath.ReverseFind(_T('\\'));
    if( pos >= 0 ) sInstancePath = sInstancePath.Left(pos + 1);
    return sInstancePath;
}

CDuiString CPaintManagerUI::GetCurrentPath()
{
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetCurrentDirectory(MAX_PATH, tszModule);
    return tszModule;
}

HINSTANCE CPaintManagerUI::GetResourceDll()
{
    if( m_hResourceInstance == NULL ) return m_hInstance;
    return m_hResourceInstance;
}

const CDuiString& CPaintManagerUI::GetResourcePath()
{
    return m_pStrResourcePath;
}

const CDuiString& CPaintManagerUI::GetResourceZip()
{
    return m_pStrResourceZip;
}

bool CPaintManagerUI::IsCachedResourceZip()
{
    return m_bCachedResourceZip;
}

HANDLE CPaintManagerUI::GetResourceZipHandle()
{
    return m_hResourceZip;
}

void CPaintManagerUI::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}

void CPaintManagerUI::SetCurrentPath(LPCTSTR pStrPath)
{
    ::SetCurrentDirectory(pStrPath);
}

void CPaintManagerUI::SetResourceDll(HINSTANCE hInst)
{
    m_hResourceInstance = hInst;
}

void CPaintManagerUI::SetResourcePath(LPCTSTR pStrPath)
{
    m_pStrResourcePath = pStrPath;
    if( m_pStrResourcePath.IsEmpty() ) return;
    TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
    if( cEnd != _T('\\') && cEnd != _T('/') ) m_pStrResourcePath += _T('\\');
}

void CPaintManagerUI::SetResourceZip(LPVOID pVoid, unsigned int len)
{
    if( m_pStrResourceZip == _T("membuffer") ) return;
    if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
        CloseZip((HZIP)m_hResourceZip);
        m_hResourceZip = NULL;
    }
    m_pStrResourceZip = _T("membuffer");
    if( m_bCachedResourceZip ) 
        m_hResourceZip = (HANDLE)OpenZip(pVoid, len, 3);
}

void CPaintManagerUI::SetResourceZip(LPCTSTR pStrPath, bool bCachedResourceZip)
{
    if( m_pStrResourceZip == pStrPath && m_bCachedResourceZip == bCachedResourceZip ) return;
    if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
        CloseZip((HZIP)m_hResourceZip);
        m_hResourceZip = NULL;
    }
    m_pStrResourceZip = pStrPath;
    m_bCachedResourceZip = bCachedResourceZip;
    if( m_bCachedResourceZip ) {
        CDuiString sFile = CPaintManagerUI::GetResourcePath();
        sFile += CPaintManagerUI::GetResourceZip();
        m_hResourceZip = (HANDLE)OpenZip((void*)sFile.GetData(), 0, 2);
    }
}

bool CPaintManagerUI::GetHSL(short* H, short* S, short* L)
{
    *H = m_H;
    *S = m_S;
    *L = m_L;
	return m_bUseHSL;
}

void CPaintManagerUI::SetHSL(bool bUseHSL, short H, short S, short L)
{
	if( m_bUseHSL || m_bUseHSL != bUseHSL ) {
		m_bUseHSL = bUseHSL;
		if( H == m_H && S == m_S && L == m_L ) return;
		m_H = CLAMP(H, 0, 360);
		m_S = CLAMP(S, 0, 200);
		m_L = CLAMP(L, 0, 200);
		AdjustSharedImagesHSL();
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
			CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			if( pManager != NULL ) pManager->AdjustImagesHSL();
		}
	}
}

void CPaintManagerUI::ReloadSkin()
{
	ReloadSharedImages();
    for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
        CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
        pManager->ReloadImages();
    }
}

bool CPaintManagerUI::LoadPlugin(LPCTSTR pstrModuleName)
{
    ASSERT( !::IsBadStringPtr(pstrModuleName,-1) || pstrModuleName == NULL );
    if( pstrModuleName == NULL ) return false;
    HMODULE hModule = ::LoadLibrary(pstrModuleName);
    if( hModule != NULL ) {
        LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
        if( lpCreateControl != NULL ) {
            if( m_aPlugins.Find(lpCreateControl) >= 0 ) return true;
            m_aPlugins.Add(lpCreateControl);
            return true;
        }
    }
    return false;
}

CStdPtrArray* CPaintManagerUI::GetPlugins()
{
    return &m_aPlugins;
}

HWND CPaintManagerUI::GetPaintWindow() const
{
    return m_hWndPaint;
}

HWND CPaintManagerUI::GetTooltipWindow() const
{
    return m_hwndTooltip;
}

HDC CPaintManagerUI::GetPaintDC() const
{
    return m_hDcPaint;
}

POINT CPaintManagerUI::GetMousePos() const
{
    return m_ptLastMousePos;
}

SIZE CPaintManagerUI::GetClientSize() const
{
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    return CDuiSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

SIZE CPaintManagerUI::GetInitSize()
{
    return m_szInitWindowSize;
}

void CPaintManagerUI::SetInitSize(int cx, int cy)
{
    m_szInitWindowSize.cx = cx;
    m_szInitWindowSize.cy = cy;
    if( m_pRoot == NULL && m_hWndPaint != NULL ) {
        ::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
}

RECT& CPaintManagerUI::GetSizeBox()
{
    return m_rcSizeBox;
}

void CPaintManagerUI::SetSizeBox(RECT& rcSizeBox)
{
    m_rcSizeBox = rcSizeBox;
}

RECT& CPaintManagerUI::GetCaptionRect()
{
    return m_rcCaption;
}

void CPaintManagerUI::SetCaptionRect(RECT& rcCaption)
{
    m_rcCaption = rcCaption;
}

SIZE CPaintManagerUI::GetRoundCorner() const
{
    return m_szRoundCorner;
}

void CPaintManagerUI::SetRoundCorner(int cx, int cy)
{
    m_szRoundCorner.cx = cx;
    m_szRoundCorner.cy = cy;
}

SIZE CPaintManagerUI::GetMinInfo() const
{
	return m_szMinWindow;
}

void CPaintManagerUI::SetMinInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMinWindow.cx = cx;
    m_szMinWindow.cy = cy;
}

SIZE CPaintManagerUI::GetMaxInfo() const
{
    return m_szMaxWindow;
}

void CPaintManagerUI::SetMaxInfo(int cx, int cy)
{
    ASSERT(cx>=0 && cy>=0);
    m_szMaxWindow.cx = cx;
    m_szMaxWindow.cy = cy;
}

int CPaintManagerUI::GetTransparent() const
{
	return m_nOpacity;
}
void CPaintManagerUI::SetTransparent(int nOpacity)
{
	if (nOpacity<0)
		m_nOpacity = 0;
	else if (nOpacity>255)
		m_nOpacity = 255;
	else
		m_nOpacity = nOpacity;
    if( m_hWndPaint != NULL ) {
        typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
        PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes;

        HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
        if (hUser32)
        {
            fSetLayeredWindowAttributes = 
                (PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
            if( fSetLayeredWindowAttributes == NULL ) return;
        }

        DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
        DWORD dwNewStyle = dwStyle;
        if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
        else dwNewStyle &= ~WS_EX_LAYERED;
        if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
        fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
    }
}

void CPaintManagerUI::SetBackgroundTransparent(bool bTrans)
{
    m_bAlphaBackground = bTrans;
}

bool CPaintManagerUI::IsShowUpdateRect() const
{
	return m_bShowUpdateRect;
}

void CPaintManagerUI::SetShowUpdateRect(bool show)
{
    m_bShowUpdateRect = show;
}

bool CPaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
{
    for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) 
    {
        bool bHandled = false;
        LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
        if( bHandled ) {
            return true;
        }
    }
    switch( uMsg ) {
    case WM_KEYDOWN:
        {
           // Tabbing between controls
           if( wParam == VK_TAB ) {
               if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL ) {
                   if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() ) return false;
               }
               SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
               return true;
           }
        }
        break;
    case WM_SYSCHAR:
        {
           // Handle ALT-shortcut key-combinations
           FINDSHORTCUT fs = { 0 };
           fs.ch = toupper((int)wParam);
           CControlUI* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
           if( pControl != NULL ) {
               pControl->SetFocus();
               pControl->Activate();
               return true;
           }
        }
        break;
    case WM_SYSKEYDOWN:
        {
           if( m_pFocus != NULL ) {
               TEventUI event = { 0 };
               event.Type = UIEVENT_SYSKEY;
               event.chKey = (TCHAR)wParam;
               event.ptMouse = m_ptLastMousePos;
               event.wKeyState = MapKeyState();
               event.dwTimestamp = ::GetTickCount();
               m_pFocus->Event(event);
           }
        }
        break;
    }
    return false;
}

bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
//#ifdef _DEBUG
//    switch( uMsg ) {
//    case WM_NCPAINT:
//    case WM_NCHITTEST:
//    case WM_SETCURSOR:
//       break;
//    default:
//       DUITRACE(_T("MSG: %-20s (%08ld)"), DUITRACEMSG(uMsg), ::GetTickCount());
//    }
//#endif
    // Not ready yet?
    if( m_hWndPaint == NULL ) return false;
    
    TNotifyUI* pMsg = NULL;
    while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
        m_aAsyncNotify.Remove(0);
        if( pMsg->pSender != NULL ) {
            if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
        }
        for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
            static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
        }
        delete pMsg;
    }
    
    // Cycle through listeners
    for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
    {
        bool bHandled = false;
        LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
        if( bHandled ) {
            lRes = lResult;
            return true;
        }
    }
    // Custom handling of events
    switch( uMsg ) {
    case WM_APP + 1:
        {
            for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) 
                delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
            m_aDelayedCleanup.Empty();
        }
        break;
    case WM_CLOSE:
        {
            // Make sure all matching "closing" events are sent
            TEventUI event = { 0 };
            event.ptMouse = m_ptLastMousePos;
            event.dwTimestamp = ::GetTickCount();
            if( m_pEventHover != NULL ) {
                event.Type = UIEVENT_MOUSELEAVE;
                event.pSender = m_pEventHover;
                m_pEventHover->Event(event);
            }
            if( m_pEventClick != NULL ) {
                event.Type = UIEVENT_BUTTONUP;
                event.pSender = m_pEventClick;
                m_pEventClick->Event(event);
            }

            SetFocus(NULL);

            // Hmmph, the usual Windows tricks to avoid
            // focus loss...
            HWND hwndParent = GetWindowOwner(m_hWndPaint);
            if( hwndParent != NULL ) ::SetFocus(hwndParent);
        }
        break;
    case WM_ERASEBKGND:
        {
            // We'll do the painting here...
            lRes = 1;
        }
        return true;
    case WM_PAINT:
        {
            // Should we paint?
            RECT rcPaint = { 0 };
            if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return true;
            if( m_pRoot == NULL ) {
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(m_hWndPaint, &ps);
                ::EndPaint(m_hWndPaint, &ps);
                return true;
            }            
            // Do we need to resize anything?
            // This is the time where we layout the controls on the form.
            // We delay this even from the WM_SIZE messages since resizing can be
            // a very expensize operation.
            if( m_bUpdateNeeded ) {
                m_bUpdateNeeded = false;
                RECT rcClient = { 0 };
                ::GetClientRect(m_hWndPaint, &rcClient);
                if( !::IsRectEmpty(&rcClient) ) {
                    if( m_pRoot->IsUpdateNeeded() ) {
                        if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
                        if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
                        if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
                        if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
                        m_hDcOffscreen = NULL;
                        m_hDcBackground = NULL;
                        m_hbmpOffscreen = NULL;
                        m_hbmpBackground = NULL;
						m_pRoot->SetPos(rcClient, true);
                    }
                    else {
						CControlUI* pControl = NULL;
						m_aFoundControls.Empty();
						m_pRoot->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
						for( int it = 0; it < m_aFoundControls.GetSize(); it++ ) {
							pControl = static_cast<CControlUI*>(m_aFoundControls[it]);
							if( !pControl->IsFloat() ) pControl->SetPos(pControl->GetPos(), true);
							else pControl->SetPos(pControl->GetRelativePos(), true);
						}
                    }
                    // We'll want to notify the window when it is first initialized
                    // with the correct layout. The window form would take the time
                    // to submit swipes/animations.
                    if( m_bFirstLayout ) {
                        m_bFirstLayout = false;
                        SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT,  0, 0, false);
                    }
                }
            }
            // Set focus to first control?
            if( m_bFocusNeeded ) {
                SetNextTabControl();
            }
            //
            // Render screen
            //
            // Prepare offscreen bitmap?
            if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
            {
                RECT rcClient = { 0 };
                ::GetClientRect(m_hWndPaint, &rcClient);
                m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
                m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
                ASSERT(m_hDcOffscreen);
                ASSERT(m_hbmpOffscreen);
            }
            // Begin Windows paint
            PAINTSTRUCT ps = { 0 };
            ::BeginPaint(m_hWndPaint, &ps);
            if( m_bOffscreenPaint )
            {
                HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
                int iSaveDC = ::SaveDC(m_hDcOffscreen);
                if( m_bAlphaBackground ) {
                    if( m_hbmpBackground == NULL ) {
                        RECT rcClient = { 0 };
                        ::GetClientRect(m_hWndPaint, &rcClient);
                        m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);;
                        m_hbmpBackground = ::CreateCompatibleBitmap(m_hDcPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
                        ASSERT(m_hDcBackground);
                        ASSERT(m_hbmpBackground);
                        ::SelectObject(m_hDcBackground, m_hbmpBackground);
                        ::BitBlt(m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                            ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                    }
                    else
                        ::SelectObject(m_hDcBackground, m_hbmpBackground);
                    ::BitBlt(m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                        ps.rcPaint.bottom - ps.rcPaint.top, m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                }
                m_pRoot->DoPaint(m_hDcOffscreen, ps.rcPaint);
                for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
                    CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
                    pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
                }
                ::RestoreDC(m_hDcOffscreen, iSaveDC);
                ::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                    ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                ::SelectObject(m_hDcOffscreen, hOldBitmap);

                if( m_bShowUpdateRect ) {
                    HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
                    ::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
                    ::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
                    ::SelectObject(ps.hdc, hOldPen);
                }
            }
            else
            {
                // A standard paint job
                int iSaveDC = ::SaveDC(ps.hdc);
                m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
                ::RestoreDC(ps.hdc, iSaveDC);
            }
            // All Done!
            ::EndPaint(m_hWndPaint, &ps);
        }
        // If any of the painting requested a resize again, we'll need
        // to invalidate the entire window once more.
        if( m_bUpdateNeeded ) {
            ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        }
        return true;
    case WM_PRINTCLIENT:
        {
            RECT rcClient;
            ::GetClientRect(m_hWndPaint, &rcClient);
            HDC hDC = (HDC) wParam;
            int save = ::SaveDC(hDC);
            m_pRoot->DoPaint(hDC, rcClient);
            // Check for traversing children. The crux is that WM_PRINT will assume
            // that the DC is positioned at frame coordinates and will paint the child
            // control at the wrong position. We'll simulate the entire thing instead.
            if( (lParam & PRF_CHILDREN) != 0 ) {
                HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
                while( hWndChild != NULL ) {
                    RECT rcPos = { 0 };
                    ::GetWindowRect(hWndChild, &rcPos);
                    ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
                    ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
                    // NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
                    //       since the latter will not print the nonclient correctly for
                    //       EDIT controls.
                    ::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
                    hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
                }
            }
            ::RestoreDC(hDC, save);
        }
        break;
    case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
            if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
            if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
            if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
        }
        break;
    case WM_SIZE:
        {
            if( m_pFocus != NULL ) {
                TEventUI event = { 0 };
                event.Type = UIEVENT_WINDOWSIZE;
                event.pSender = m_pFocus;
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
            }
            if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
        }
        return true;
    case WM_TIMER:
        {
            for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
                const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
                if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false) {
                    TEventUI event = { 0 };
                    event.Type = UIEVENT_TIMER;
                    event.pSender = pTimer->pSender;
                    event.wParam = pTimer->nLocalID;
                    event.dwTimestamp = ::GetTickCount();
                    pTimer->pSender->Event(event);
                    break;
                }
            }
        }
        break;
    case WM_MOUSEHOVER:
        {
            m_bMouseTracking = false;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            CControlUI* pHover = FindControl(pt);
            if( pHover == NULL ) break;
            // Generate mouse hover event
            if( m_pEventHover != NULL ) {
                TEventUI event = { 0 };
                event.ptMouse = pt;
                event.Type = UIEVENT_MOUSEHOVER;
                event.pSender = m_pEventHover;
                event.dwTimestamp = ::GetTickCount();
                m_pEventHover->Event(event);
            }
            // Create tooltip information
            CDuiString sToolTip = pHover->GetToolTip();
            if( sToolTip.IsEmpty() ) return true;
			ProcessMultiLanguageTokens(sToolTip);
            ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
            m_ToolTip.cbSize = sizeof(TOOLINFO);
            m_ToolTip.uFlags = TTF_IDISHWND;
            m_ToolTip.hwnd = m_hWndPaint;
            m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
            m_ToolTip.hinst = m_hInstance;
            m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip );
            m_ToolTip.rect = pHover->GetPos();
            if( m_hwndTooltip == NULL ) {
                m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
                ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
            }
			::SendMessage( m_hwndTooltip,TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
            ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
            ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
        }
        return true;
    case WM_MOUSELEAVE:
        {
            if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
            if( m_bMouseTracking ) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
            m_bMouseTracking = false;
        }
        break;
    case WM_MOUSEMOVE:
        {
            // Start tracking this entire window again...
            if( !m_bMouseTracking ) {
                TRACKMOUSEEVENT tme = { 0 };
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_HOVER | TME_LEAVE;
                tme.hwndTrack = m_hWndPaint;
                tme.dwHoverTime = m_hwndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                _TrackMouseEvent(&tme);
                m_bMouseTracking = true;
            }
            // Generate the appropriate mouse messages
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pNewHover = FindControl(pt);
            if( pNewHover != NULL && pNewHover->GetManager() != this ) break;
            TEventUI event = { 0 };
            event.ptMouse = pt;
            event.dwTimestamp = ::GetTickCount();
            if( pNewHover != m_pEventHover && m_pEventHover != NULL ) {
                event.Type = UIEVENT_MOUSELEAVE;
                event.pSender = m_pEventHover;
                m_pEventHover->Event(event);
                m_pEventHover = NULL;
                if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
            }
            if( pNewHover != m_pEventHover && pNewHover != NULL ) {
                event.Type = UIEVENT_MOUSEENTER;
                event.pSender = pNewHover;
                pNewHover->Event(event);
                m_pEventHover = pNewHover;
            }
            if( m_pEventClick != NULL ) {
                event.Type = UIEVENT_MOUSEMOVE;
                event.pSender = m_pEventClick;
                m_pEventClick->Event(event);
            }
            else if( pNewHover != NULL ) {
                event.Type = UIEVENT_MOUSEMOVE;
                event.pSender = pNewHover;
                pNewHover->Event(event);
            }
        }
        break;
    case WM_LBUTTONDOWN:
        {
            // We alway set focus back to our app (this helps
            // when Win32 child windows are placed on the dialog
            // and we need to remove them on focus change).
            ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            m_pEventClick = pControl;
            pControl->SetFocus();
            SetCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_BUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
        }
        break;
    case WM_LBUTTONDBLCLK:
        {
            ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            SetCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_DBLCLICK;
            event.pSender = pControl;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
            m_pEventClick = pControl;
        }
        break;
    case WM_LBUTTONUP:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            if( m_pEventClick == NULL ) break;
            ReleaseCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_BUTTONUP;
            event.pSender = m_pEventClick;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
			// By daviyang35 at 2015-6-5 16:10:13
			// 在Click事件中弹出了模态对话框，退出阶段窗口实例可能已经删除
			// this成员属性赋值将会导致heap错误
			// this成员函数调用将会导致野指针异常
			// 使用栈上的成员来调用响应，提前清空成员
			// 当阻塞的模态窗口返回时，回栈阶段不访问任何类实例方法或属性
			// 将不会触发异常
			CControlUI* pClick = m_pEventClick;
			m_pEventClick = NULL;
            pClick->Event(event);
        }
        break;
    case WM_RBUTTONDOWN:
        {
            ::SetFocus(m_hWndPaint);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            pControl->SetFocus();
            SetCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_RBUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
            m_pEventClick = pControl;
        }
        break;
    case WM_CONTEXTMENU:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            if( m_pEventClick == NULL ) break;
            ReleaseCapture();
            TEventUI event = { 0 };
            event.Type = UIEVENT_CONTEXTMENU;
            event.pSender = m_pEventClick;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.lParam = (LPARAM)m_pEventClick;
            event.dwTimestamp = ::GetTickCount();
            m_pEventClick->Event(event);
            m_pEventClick = NULL;
        }
        break;
    case WM_MOUSEWHEEL:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( pControl->GetManager() != this ) break;
            int zDelta = (int) (short) HIWORD(wParam);
            TEventUI event = { 0 };
            event.Type = UIEVENT_SCROLLWHEEL;
            event.pSender = pControl;
            event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
            event.lParam = lParam;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);

            // Let's make sure that the scroll item below the cursor is the same as before...
            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
        }
        break;
    case WM_CHAR:
        {
            if( m_pFocus == NULL ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_CHAR;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
        }
        break;
    case WM_KEYDOWN:
        {
            if( m_pFocus == NULL ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            m_pEventKey = m_pFocus;
        }
        break;
    case WM_KEYUP:
        {
            if( m_pEventKey == NULL ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_KEYUP;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pEventKey->Event(event);
            m_pEventKey = NULL;
        }
        break;
    case WM_SETCURSOR:
        {
            if( LOWORD(lParam) != HTCLIENT ) break;
            if( m_bMouseCapture ) return true;

            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_hWndPaint, &pt);
            CControlUI* pControl = FindControl(pt);
            if( pControl == NULL ) break;
            if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
            TEventUI event = { 0 };
            event.Type = UIEVENT_SETCURSOR;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
        }
        return true;
    case WM_NOTIFY:
        {
            LPNMHDR lpNMHDR = (LPNMHDR) lParam;
            if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_COMMAND:
        {
            if( lParam == 0 ) break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
        {
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
            if( lParam == 0 ) break;
            HWND hWndChild = (HWND) lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        }
        break;
    default:
        break;
    }

    pMsg = NULL;
    while( pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify.GetAt(0)) ) {
        m_aAsyncNotify.Remove(0);
        if( pMsg->pSender != NULL ) {
            if( pMsg->pSender->OnNotify ) pMsg->pSender->OnNotify(pMsg);
        }
        for( int j = 0; j < m_aNotifiers.GetSize(); j++ ) {
            static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
        }
        delete pMsg;
    }

    return false;
}

bool CPaintManagerUI::IsUpdateNeeded() const
{
	return m_bUpdateNeeded;
}

void CPaintManagerUI::NeedUpdate()
{
    m_bUpdateNeeded = true;
}

void CPaintManagerUI::Invalidate()
{
	::InvalidateRect(m_hWndPaint, NULL, FALSE);
	//if( !m_bLayered ) ::InvalidateRect(m_hWndPaint, NULL, FALSE);
	//else {
	//	RECT rcClient = { 0 };
	//	::GetClientRect(m_hWndPaint, &rcClient);
	//	::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcClient);
	//}
}

void CPaintManagerUI::Invalidate(RECT& rcItem)
{
    ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

bool CPaintManagerUI::AttachDialog(CControlUI* pControl)
{
    ASSERT(::IsWindow(m_hWndPaint));
    // Reset any previous attachment
    SetFocus(NULL);
    m_pEventKey = NULL;
    m_pEventHover = NULL;
    m_pEventClick = NULL;
    // Remove the existing control-tree. We might have gotten inside this function as
    // a result of an event fired or similar, so we cannot just delete the objects and
    // pull the internal memory of the calling code. We'll delay the cleanup.
    if( m_pRoot != NULL ) {
        m_aPostPaintControls.Empty();
        AddDelayedCleanup(m_pRoot);
    }
    // Set the dialog root element
    m_pRoot = pControl;
    // Go ahead...
    m_bUpdateNeeded = true;
    m_bFirstLayout = true;
    m_bFocusNeeded = true;
    // Initiate all control
    return InitControls(pControl);
}

bool CPaintManagerUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
{
    ASSERT(pControl);
    if( pControl == NULL ) return false;
    pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(), true);
    pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
    return true;
}

void CPaintManagerUI::ReapObjects(CControlUI* pControl)
{
    if( pControl == NULL ) return;
    if( pControl == m_pEventKey ) m_pEventKey = NULL;
    if( pControl == m_pEventHover ) m_pEventHover = NULL;
    if( pControl == m_pEventClick ) m_pEventClick = NULL;
    if( pControl == m_pFocus ) m_pFocus = NULL;
    KillTimer(pControl);
    const CDuiString& sName = pControl->GetName();
    if( !sName.IsEmpty() ) {
        if( pControl == FindControl(sName) ) m_mNameHash.Remove(sName);
    }
    for( int i = 0; i < m_aAsyncNotify.GetSize(); i++ ) {
        TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_aAsyncNotify[i]);
        if( pMsg->pSender == pControl ) pMsg->pSender = NULL;
    }    
}

bool CPaintManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if( lp ) {
        CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
        for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
            if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
                return false;
            }
        }
        aOptionGroup->Add(pControl);
    }
    else {
        CStdPtrArray* aOptionGroup = new CStdPtrArray(6);
        aOptionGroup->Add(pControl);
        m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
    }
    return true;
}

CStdPtrArray* CPaintManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if( lp ) return static_cast<CStdPtrArray*>(lp);
    return NULL;
}

void CPaintManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl)
{
	LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
	if( lp ) {
		CStdPtrArray* aOptionGroup = static_cast<CStdPtrArray*>(lp);
		if( aOptionGroup == NULL ) return;
		for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
			if( static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl ) {
				aOptionGroup->Remove(i);
				break;
			}
		}
		if( aOptionGroup->IsEmpty() ) {
			delete aOptionGroup;
			m_mOptionGroup.Remove(pStrGroupName);
		}
	}
}

void CPaintManagerUI::RemoveAllOptionGroups()
{
	CStdPtrArray* aOptionGroup;
	for( int i = 0; i< m_mOptionGroup.GetSize(); i++ ) {
		if(LPCTSTR key = m_mOptionGroup.GetAt(i)) {
			aOptionGroup = static_cast<CStdPtrArray*>(m_mOptionGroup.Find(key));
			delete aOptionGroup;
		}
	}
	m_mOptionGroup.RemoveAll();
}

void CPaintManagerUI::MessageLoop()
{
    MSG msg = { 0 };
    while( ::GetMessage(&msg, NULL, 0, 0) ) {
        if( !CPaintManagerUI::TranslateMessage(&msg) ) {
            ::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			//try{
   //         ::DispatchMessage(&msg);
			//} catch(...) {
			//	DUITRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);
			//	#ifdef _DEBUG
			//	throw "CPaintManagerUI::MessageLoop";
			//	#endif
			//}
        }
    }
}

void CPaintManagerUI::Term()
{
    if( m_bCachedResourceZip && m_hResourceZip != NULL ) {
        CloseZip((HZIP)m_hResourceZip);
        m_hResourceZip = NULL;
    }
}

CControlUI* CPaintManagerUI::GetFocus() const
{
    return m_pFocus;
}

void CPaintManagerUI::SetFocus(CControlUI* pControl)
{
    // Paint manager window has focus?
    HWND hFocusWnd = ::GetFocus();
    if( hFocusWnd != m_hWndPaint && pControl != m_pFocus ) ::SetFocus(m_hWndPaint);
    // Already has focus?
    if( pControl == m_pFocus ) return;
    // Remove focus from old control
    if( m_pFocus != NULL ) 
    {
        TEventUI event = { 0 };
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
        m_pFocus = NULL;
    }
    if( pControl == NULL ) return;
    // Set focus to new control
    if( pControl != NULL 
        && pControl->GetManager() == this 
        && pControl->IsVisible() 
        && pControl->IsEnabled() ) 
    {
        m_pFocus = pControl;
        TEventUI event = { 0 };
        event.Type = UIEVENT_SETFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_SETFOCUS);
    }
}

void CPaintManagerUI::SetFocusNeeded(CControlUI* pControl)
{
    ::SetFocus(m_hWndPaint);
    if( pControl == NULL ) return;
    if( m_pFocus != NULL ) {
        TEventUI event = { 0 };
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
        m_pFocus = NULL;
    }
    FINDTABINFO info = { 0 };
    info.pFocus = pControl;
    info.bForward = false;
    m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    m_bFocusNeeded = true;
    if( m_pRoot != NULL ) m_pRoot->NeedUpdate();
}

bool CPaintManagerUI::SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse)
{
    ASSERT(pControl!=NULL);
    ASSERT(uElapse>0);
    for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->pSender == pControl
            && pTimer->hWnd == m_hWndPaint
            && pTimer->nLocalID == nTimerID ) {
            if( pTimer->bKilled == true ) {
                if( ::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL) ) {
                    pTimer->bKilled = false;
                    return true;
                }
                return false;
            }
            return false;
        }
    }

    m_uTimerID = (++m_uTimerID) % 0xFF;
    if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
    TIMERINFO* pTimer = new TIMERINFO;
    if( pTimer == NULL ) return FALSE;
    pTimer->hWnd = m_hWndPaint;
    pTimer->pSender = pControl;
    pTimer->nLocalID = nTimerID;
    pTimer->uWinTimer = m_uTimerID;
    pTimer->bKilled = false;
    return m_aTimers.Add(pTimer);
}

bool CPaintManagerUI::KillTimer(CControlUI* pControl, UINT nTimerID)
{
    ASSERT(pControl!=NULL);
    for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->pSender == pControl
            && pTimer->hWnd == m_hWndPaint
            && pTimer->nLocalID == nTimerID )
        {
            if( pTimer->bKilled == false ) {
                if( ::IsWindow(m_hWndPaint) ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
                pTimer->bKilled = true;
                return true;
            }
        }
    }
    return false;
}

void CPaintManagerUI::KillTimer(CControlUI* pControl)
{
    ASSERT(pControl!=NULL);
    int count = m_aTimers.GetSize();
    for( int i = 0, j = 0; i < count; i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
        if( pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint ) {
            if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
            delete pTimer;
            m_aTimers.Remove(i - j);
            j++;
        }
    }
}

void CPaintManagerUI::RemoveAllTimers()
{
    for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
        TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
        if( pTimer->hWnd == m_hWndPaint ) {
            if( pTimer->bKilled == false ) {
                if( ::IsWindow(m_hWndPaint) ) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
            }
            delete pTimer;
        }
    }

    m_aTimers.Empty();
}

void CPaintManagerUI::SetCapture()
{
    ::SetCapture(m_hWndPaint);
    m_bMouseCapture = true;
}

void CPaintManagerUI::ReleaseCapture()
{
    ::ReleaseCapture();
    m_bMouseCapture = false;
}

bool CPaintManagerUI::IsCaptured()
{
    return m_bMouseCapture;
}

bool CPaintManagerUI::SetNextTabControl(bool bForward)
{
    // If we're in the process of restructuring the layout we can delay the
    // focus calulation until the next repaint.
    if( m_bUpdateNeeded && bForward ) {
        m_bFocusNeeded = true;
        ::InvalidateRect(m_hWndPaint, NULL, FALSE);
        return true;
    }
    // Find next/previous tabbable control
    FINDTABINFO info1 = { 0 };
    info1.pFocus = m_pFocus;
    info1.bForward = bForward;
    CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    if( pControl == NULL ) {  
        if( bForward ) {
            // Wrap around
            FINDTABINFO info2 = { 0 };
            info2.pFocus = bForward ? NULL : info1.pLast;
            info2.bForward = bForward;
            pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        }
        else {
            pControl = info1.pLast;
        }
    }
    if( pControl != NULL ) SetFocus(pControl);
    m_bFocusNeeded = false;
    return true;
}

bool CPaintManagerUI::AddNotifier(INotifyUI* pNotifier)
{
    ASSERT(m_aNotifiers.Find(pNotifier)<0);
    return m_aNotifiers.Add(pNotifier);
}

bool CPaintManagerUI::RemoveNotifier(INotifyUI* pNotifier)
{
    for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
        if( static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier ) {
            return m_aNotifiers.Remove(i);
        }
    }
    return false;
}

bool CPaintManagerUI::AddPreMessageFilter(IMessageFilterUI* pFilter)
{
    ASSERT(m_aPreMessageFilters.Find(pFilter)<0);
    return m_aPreMessageFilters.Add(pFilter);
}

bool CPaintManagerUI::RemovePreMessageFilter(IMessageFilterUI* pFilter)
{
    for( int i = 0; i < m_aPreMessageFilters.GetSize(); i++ ) {
        if( static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter ) {
            return m_aPreMessageFilters.Remove(i);
        }
    }
    return false;
}

bool CPaintManagerUI::AddMessageFilter(IMessageFilterUI* pFilter)
{
    ASSERT(m_aMessageFilters.Find(pFilter)<0);
    return m_aMessageFilters.Add(pFilter);
}

bool CPaintManagerUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
{
    for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
        if( static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter ) {
            return m_aMessageFilters.Remove(i);
        }
    }
    return false;
}

int CPaintManagerUI::GetPostPaintCount() const
{
    return m_aPostPaintControls.GetSize();
}

bool CPaintManagerUI::AddPostPaint(CControlUI* pControl)
{
    ASSERT(m_aPostPaintControls.Find(pControl) < 0);
    return m_aPostPaintControls.Add(pControl);
}

bool CPaintManagerUI::RemovePostPaint(CControlUI* pControl)
{
    for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
        if( static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl ) {
            return m_aPostPaintControls.Remove(i);
        }
    }
    return false;
}

bool CPaintManagerUI::SetPostPaintIndex(CControlUI* pControl, int iIndex)
{
    RemovePostPaint(pControl);
    return m_aPostPaintControls.InsertAt(iIndex, pControl);
}

void CPaintManagerUI::AddDelayedCleanup(CControlUI* pControl)
{
    pControl->SetManager(this, NULL, false);
    m_aDelayedCleanup.Add(pControl);
    ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
}

void CPaintManagerUI::SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
    TNotifyUI Msg;
    Msg.pSender = pControl;
    Msg.sType = pstrMessage;
    Msg.wParam = wParam;
    Msg.lParam = lParam;
    SendNotify(Msg, bAsync);
}

void CPaintManagerUI::SendNotify(TNotifyUI& Msg, bool bAsync /*= false*/)
{
    Msg.ptMouse = m_ptLastMousePos;
    Msg.dwTimestamp = ::GetTickCount();
	if( m_bUsedVirtualWnd )
	{
		Msg.sVirtualWnd = Msg.pSender->GetVirtualWnd();
	}

    if( !bAsync ) {
        // Send to all listeners
        if( Msg.pSender != NULL ) {
            if( Msg.pSender->OnNotify ) Msg.pSender->OnNotify(&Msg);
        }
        for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
            static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
        }
    }
    else {
        TNotifyUI *pMsg = new TNotifyUI;
        pMsg->pSender = Msg.pSender;
        pMsg->sType = Msg.sType;
        pMsg->wParam = Msg.wParam;
        pMsg->lParam = Msg.lParam;
        pMsg->ptMouse = Msg.ptMouse;
        pMsg->dwTimestamp = Msg.dwTimestamp;
        m_aAsyncNotify.Add(pMsg);
    }
}

DWORD CPaintManagerUI::GetDefaultDisabledColor() const
{
    return m_ResInfo.m_dwDefaultDisabledColor;
}

void CPaintManagerUI::SetDefaultDisabledColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultDisabledColor == m_SharedResInfo.m_dwDefaultDisabledColor)
			m_ResInfo.m_dwDefaultDisabledColor = dwColor;
		m_SharedResInfo.m_dwDefaultDisabledColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultDisabledColor = dwColor;
	}
}

DWORD CPaintManagerUI::GetDefaultFontColor() const
{
	return m_ResInfo.m_dwDefaultFontColor;
}

void CPaintManagerUI::SetDefaultFontColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultFontColor == m_SharedResInfo.m_dwDefaultFontColor)
			m_ResInfo.m_dwDefaultFontColor = dwColor;
		m_SharedResInfo.m_dwDefaultFontColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultFontColor = dwColor;
	}
}

DWORD CPaintManagerUI::GetDefaultLinkFontColor() const
{
    return m_ResInfo.m_dwDefaultLinkFontColor;
}

void CPaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultLinkFontColor == m_SharedResInfo.m_dwDefaultLinkFontColor)
			m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
		m_SharedResInfo.m_dwDefaultLinkFontColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
	}
}

DWORD CPaintManagerUI::GetDefaultLinkHoverFontColor() const
{
    return m_ResInfo.m_dwDefaultLinkHoverFontColor;
}

void CPaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultLinkHoverFontColor == m_SharedResInfo.m_dwDefaultLinkHoverFontColor)
			m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
		m_SharedResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
	}
}

DWORD CPaintManagerUI::GetDefaultSelectedBkColor() const
{
    return m_ResInfo.m_dwDefaultSelectedBkColor;
}

void CPaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor, bool bShared)
{
	if (bShared)
	{
		if (m_ResInfo.m_dwDefaultSelectedBkColor == m_SharedResInfo.m_dwDefaultSelectedBkColor)
			m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
		m_SharedResInfo.m_dwDefaultSelectedBkColor = dwColor;
	}
	else
	{
		m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
	}
}

TFontInfo* CPaintManagerUI::GetDefaultFontInfo()
{
	if (m_ResInfo.m_DefaultFontInfo.sFontName.IsEmpty())
	{
		if( m_SharedResInfo.m_DefaultFontInfo.tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_SharedResInfo.m_DefaultFontInfo.hFont);
			::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return &m_SharedResInfo.m_DefaultFontInfo;
	}
	else
	{
		if( m_ResInfo.m_DefaultFontInfo.tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, m_ResInfo.m_DefaultFontInfo.hFont);
			::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
		return &m_ResInfo.m_DefaultFontInfo;
	}
}

void CPaintManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return;

	if (bShared)
	{
		::DeleteObject(m_SharedResInfo.m_DefaultFontInfo.hFont);
		m_SharedResInfo.m_DefaultFontInfo.hFont = hFont;
		m_SharedResInfo.m_DefaultFontInfo.sFontName = pStrFontName;
		m_SharedResInfo.m_DefaultFontInfo.iSize = nSize;
		m_SharedResInfo.m_DefaultFontInfo.bBold = bBold;
		m_SharedResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
		m_SharedResInfo.m_DefaultFontInfo.bItalic = bItalic;
		::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
	}
	else
	{
		::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
		m_ResInfo.m_DefaultFontInfo.hFont = hFont;
		m_ResInfo.m_DefaultFontInfo.sFontName = pStrFontName;
		m_ResInfo.m_DefaultFontInfo.iSize = nSize;
		m_ResInfo.m_DefaultFontInfo.bBold = bBold;
		m_ResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
		m_ResInfo.m_DefaultFontInfo.bItalic = bItalic;
		::ZeroMemory(&m_ResInfo.m_DefaultFontInfo.tm, sizeof(m_ResInfo.m_DefaultFontInfo.tm));
		if( m_hDcPaint ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
			::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
			::SelectObject(m_hDcPaint, hOldFont);
		}
	}
}

DWORD CPaintManagerUI::GetCustomFontCount(bool bShared) const
{
	if (bShared)
		return m_SharedResInfo.m_CustomFonts.GetSize();
	else
		return m_ResInfo.m_CustomFonts.GetSize();
}

HFONT CPaintManagerUI::AddFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    _tcsncpy(lf.lfFaceName, pStrFontName, LF_FACESIZE);
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = -nSize;
    if( bBold ) lf.lfWeight += FW_BOLD;
    if( bUnderline ) lf.lfUnderline = TRUE;
    if( bItalic ) lf.lfItalic = TRUE;
    HFONT hFont = ::CreateFontIndirect(&lf);
    if( hFont == NULL ) return NULL;

    TFontInfo* pFontInfo = new TFontInfo;
    if( !pFontInfo ) return false;
    ::ZeroMemory(pFontInfo, sizeof(TFontInfo));
    pFontInfo->hFont = hFont;
    pFontInfo->sFontName = pStrFontName;
    pFontInfo->iSize = nSize;
    pFontInfo->bBold = bBold;
    pFontInfo->bUnderline = bUnderline;
    pFontInfo->bItalic = bItalic;
    if( m_hDcPaint ) {
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	if (bShared)
	{
		TFontInfo* pOldFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		if (pOldFontInfo)
		{
			::DeleteObject(pOldFontInfo->hFont);
			delete pOldFontInfo;
            m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
		}

		if( !m_SharedResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo) ) 
		{
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}
	}
	else
	{
		TFontInfo* pOldFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
		if (pOldFontInfo)
		{
			::DeleteObject(pOldFontInfo->hFont);
			delete pOldFontInfo;
            m_ResInfo.m_CustomFonts.Remove(idBuffer);
		}

		if( !m_ResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo) ) 
		{
			::DeleteObject(hFont);
			delete pFontInfo;
			return NULL;
		}
	}

    return hFont;
}

HFONT CPaintManagerUI::GetFont(int id)
{
	if (id < 0) return GetDefaultFontInfo()->hFont;
		
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
	if( !pFontInfo ) pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
	if (!pFontInfo) return GetDefaultFontInfo()->hFont;
	return pFontInfo->hFont;
}

HFONT CPaintManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    TFontInfo* pFontInfo = NULL;
	for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
		if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
			pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
			if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}
	}
	for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
		if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
			pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
			if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
				pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
				return pFontInfo->hFont;
		}
	}

	return NULL;
}

int CPaintManagerUI::GetFontIndex(HFONT hFont, bool bShared)
{
	TFontInfo* pFontInfo = NULL;
	if (bShared)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) return _ttoi(key);
			}
		}
	}
	else
	{
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) return _ttoi(key);
			}
		}
	}

	return -1;
}

int CPaintManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared)
{
	TFontInfo* pFontInfo = NULL;
	if (bShared)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
					pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
					return _ttoi(key);
			}
		}
	}
	else
	{
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize && 
					pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) 
					return _ttoi(key);
			}
		}
	}

	return -1;
}

void CPaintManagerUI::RemoveFont(HFONT hFont, bool bShared)
{
    TFontInfo* pFontInfo = NULL;
	if (bShared)
	{
		for( int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) 
			{
				pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) 
				{
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
					m_SharedResInfo.m_CustomFonts.Remove(key);
					return;
				}
			}
		}
	}
	else
	{
		for( int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) 
			{
				pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) 
				{
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
					m_ResInfo.m_CustomFonts.Remove(key);
					return;
				}
			}
		}
	}
}

void CPaintManagerUI::RemoveFont(int id, bool bShared)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	TFontInfo* pFontInfo = NULL;
	if (bShared)
	{
		pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
		if (pFontInfo)
		{
			::DeleteObject(pFontInfo->hFont);
			delete pFontInfo;
			m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
		}
	}
	else
	{
		pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
		if (pFontInfo)
		{
			::DeleteObject(pFontInfo->hFont);
			delete pFontInfo;
			m_ResInfo.m_CustomFonts.Remove(idBuffer);
		}
	}
}

void CPaintManagerUI::RemoveAllFonts(bool bShared)
{
	TFontInfo* pFontInfo;
	if (bShared)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key, false));
				if (pFontInfo) {
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
				}
			}
		}
		m_SharedResInfo.m_CustomFonts.RemoveAll();
	}
	else
	{
		for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
				pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(key, false));
				if (pFontInfo) {
					::DeleteObject(pFontInfo->hFont);
					delete pFontInfo;
				}
			}
		}
		m_ResInfo.m_CustomFonts.RemoveAll();
	}
}

TFontInfo* CPaintManagerUI::GetFontInfo(int id)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);
	TFontInfo* pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
	if (!pFontInfo) pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
	if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
    if (pFontInfo->tm.tmHeight == 0) 
	{
        HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    return pFontInfo;
}

TFontInfo* CPaintManagerUI::GetFontInfo(HFONT hFont)
{
	TFontInfo* pFontInfo = NULL;
	for( int i = 0; i< m_ResInfo.m_CustomFonts.GetSize(); i++ ) 
	{
		if(LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) 
		{
			pFontInfo = static_cast<TFontInfo*>(m_ResInfo.m_CustomFonts.Find(key));
			if (pFontInfo && pFontInfo->hFont == hFont) break;
		}
	}
	if (!pFontInfo)
	{
		for( int i = 0; i< m_SharedResInfo.m_CustomFonts.GetSize(); i++ ) 
		{
			if(LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) 
			{
				pFontInfo = static_cast<TFontInfo*>(m_SharedResInfo.m_CustomFonts.Find(key));
				if (pFontInfo && pFontInfo->hFont == hFont) break;
			}
		}
	}
	if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
	if( pFontInfo->tm.tmHeight == 0 ) {
		HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, pFontInfo->hFont);
		::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
		::SelectObject(m_hDcPaint, hOldFont);
	}
	return pFontInfo;
}

const TImageInfo* CPaintManagerUI::GetImage(LPCTSTR bitmap)
{
    TImageInfo* data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
    if( !data ) data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
    return data;
}

const TImageInfo* CPaintManagerUI::GetImageEx(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL)
{
    const TImageInfo* data = GetImage(bitmap);
    if( !data ) {
        if( AddImage(bitmap, type, mask, bUseHSL, false) ) {
            data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
        }
    }

    return data;
}

const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL, bool bShared)
{
	if( bitmap == NULL || bitmap[0] == _T('\0') ) return NULL;

    TImageInfo* data = NULL;
    if( type != NULL ) {
        if( isdigit(*bitmap) ) {
            LPTSTR pstr = NULL;
            int iIndex = _tcstol(bitmap, &pstr, 10);
            data = CRenderEngine::LoadImage(iIndex, type, mask);
        }
    }
    else {
        data = CRenderEngine::LoadImage(bitmap, NULL, mask);
    }

	if( data == NULL ) return NULL;
	data->bUseHSL = bUseHSL;
	if( type != NULL ) data->sResType = type;
	data->dwMask = mask;
	if( data->bUseHSL ) {
		data->pSrcBits = new BYTE[data->nX * data->nY * 4];
		::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
	}
	else data->pSrcBits = NULL;
	if( m_bUseHSL ) CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);
	if (data)
	{
		if (bShared)
		{
            TImageInfo* pOldImageInfo = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
            if (pOldImageInfo)
            {
                CRenderEngine::FreeImage(pOldImageInfo);
                m_SharedResInfo.m_ImageHash.Remove(bitmap);
            }

			if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
				CRenderEngine::FreeImage(data);
				data = NULL;
			}
		}
		else
		{
            TImageInfo* pOldImageInfo = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
            if (pOldImageInfo)
            {
                CRenderEngine::FreeImage(pOldImageInfo);
                m_ResInfo.m_ImageHash.Remove(bitmap);
            }

			if( !m_ResInfo.m_ImageHash.Insert(bitmap, data) ) {
				CRenderEngine::FreeImage(data);
				data = NULL;
			}
		}
	}

    return data;
}

const TImageInfo* CPaintManagerUI::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha, bool bShared)
{
	// 因无法确定外部HBITMAP格式，不能使用hsl调整
	if( bitmap == NULL || bitmap[0] == _T('\0') ) return NULL;
    if( hBitmap == NULL || iWidth <= 0 || iHeight <= 0 ) return NULL;

	TImageInfo* data = new TImageInfo;
	data->hBitmap = hBitmap;
	data->pBits = NULL;
	data->nX = iWidth;
	data->nY = iHeight;
	data->alphaChannel = bAlpha;
	data->bUseHSL = false;
	data->pSrcBits = NULL;
	//data->sResType = _T("");
	data->dwMask = 0;

	if (bShared)
	{
		if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
			CRenderEngine::FreeImage(data);
			data = NULL;
		}
	}
	else
	{
		if( !m_SharedResInfo.m_ImageHash.Insert(bitmap, data) ) {
			CRenderEngine::FreeImage(data);
			data = NULL;
		}
	}

    return data;
}

void CPaintManagerUI::RemoveImage(LPCTSTR bitmap, bool bShared)
{
	TImageInfo* data = NULL;
	if (bShared) 
	{
		data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
		if (data)
		{
			CRenderEngine::FreeImage(data) ;
			m_SharedResInfo.m_ImageHash.Remove(bitmap);
		}
	}
	else
	{
		data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
		if (data)
		{
			CRenderEngine::FreeImage(data) ;
			m_ResInfo.m_ImageHash.Remove(bitmap);
		}
	}
}

void CPaintManagerUI::RemoveAllImages(bool bShared)
{
	if (bShared)
	{
		TImageInfo* data;
		for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
				data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(key, false));
				if (data) {
					CRenderEngine::FreeImage(data);
				}
			}
		}
		m_SharedResInfo.m_ImageHash.RemoveAll();
	}
	else
	{
		TImageInfo* data;
		for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
				data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(key, false));
				if (data) {
					CRenderEngine::FreeImage(data);
				}
			}
		}
		m_ResInfo.m_ImageHash.RemoveAll();
	}
}

void CPaintManagerUI::AdjustSharedImagesHSL()
{
	TImageInfo* data;
	for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(key));
			if( data && data->bUseHSL ) {
				CRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}
}

void CPaintManagerUI::AdjustImagesHSL()
{
	TImageInfo* data;
	for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(key));
			if( data && data->bUseHSL ) {
				CRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
			}
		}
	}
	Invalidate();
}

void CPaintManagerUI::ReloadSharedImages()
{
	TImageInfo* data;
	TImageInfo* pNewData;
	for( int i = 0; i< m_SharedResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR bitmap = m_SharedResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<TImageInfo*>(m_SharedResInfo.m_ImageHash.Find(bitmap));
			if( data != NULL ) {
				if( !data->sResType.IsEmpty() ) {
					if( isdigit(*bitmap) ) {
						LPTSTR pstr = NULL;
						int iIndex = _tcstol(bitmap, &pstr, 10);
						pNewData = CRenderEngine::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
					}
				}
				else {
					pNewData = CRenderEngine::LoadImage(bitmap, NULL, data->dwMask);
				}
				if( pNewData == NULL ) continue;

				CRenderEngine::FreeImage(data, false);
				data->hBitmap = pNewData->hBitmap;
				data->pBits = pNewData->pBits;
				data->nX = pNewData->nX;
				data->nY = pNewData->nY;
				data->alphaChannel = pNewData->alphaChannel;
				data->pSrcBits = NULL;
				if( data->bUseHSL ) {
					data->pSrcBits = new BYTE[data->nX * data->nY * 4];
					::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
				}
				else data->pSrcBits = NULL;
				if( m_bUseHSL ) CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);

				delete pNewData;
			}
		}
	}
}

void CPaintManagerUI::ReloadImages()
{
	TImageInfo* data;
	TImageInfo* pNewData;
	for( int i = 0; i< m_ResInfo.m_ImageHash.GetSize(); i++ ) {
		if(LPCTSTR bitmap = m_ResInfo.m_ImageHash.GetAt(i)) {
			data = static_cast<TImageInfo*>(m_ResInfo.m_ImageHash.Find(bitmap));
			if( data != NULL ) {
				if( !data->sResType.IsEmpty() ) {
					if( isdigit(*bitmap) ) {
						LPTSTR pstr = NULL;
						int iIndex = _tcstol(bitmap, &pstr, 10);
						pNewData = CRenderEngine::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
					}
				}
				else {
					pNewData = CRenderEngine::LoadImage(bitmap, NULL, data->dwMask);
				}
				if( pNewData == NULL ) continue;

				CRenderEngine::FreeImage(data, false);
				data->hBitmap = pNewData->hBitmap;
				data->pBits = pNewData->pBits;
				data->nX = pNewData->nX;
				data->nY = pNewData->nY;
				data->alphaChannel = pNewData->alphaChannel;
				data->pSrcBits = NULL;
				if( data->bUseHSL ) {
					data->pSrcBits = new BYTE[data->nX * data->nY * 4];
					::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
				}
				else data->pSrcBits = NULL;
				if( m_bUseHSL ) CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);

				delete pNewData;
			}
		}
	}
    if( m_pRoot ) m_pRoot->Invalidate();
}

void CPaintManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, bool bShared)
{
	if (bShared)
	{
		CDuiString* pDefaultAttr = new CDuiString(pStrControlAttrList);
		if (pDefaultAttr != NULL)
		{
			CDuiString* pOldDefaultAttr = static_cast<CDuiString*>(m_SharedResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
			if (pOldDefaultAttr) delete pOldDefaultAttr;
		}
	}
	else
	{
		CDuiString* pDefaultAttr = new CDuiString(pStrControlAttrList);
		if (pDefaultAttr != NULL)
		{
			CDuiString* pOldDefaultAttr = static_cast<CDuiString*>(m_ResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
			if (pOldDefaultAttr) delete pOldDefaultAttr;
		}
	}
}

LPCTSTR CPaintManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
{
    CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_ResInfo.m_AttrHash.Find(pStrControlName));
	if( !pDefaultAttr ) pDefaultAttr = static_cast<CDuiString*>(m_SharedResInfo.m_AttrHash.Find(pStrControlName));
	if (pDefaultAttr) return pDefaultAttr->GetData();
	return NULL;
}

bool CPaintManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared)
{
	if (bShared)
	{
		CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_SharedResInfo.m_AttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;

		delete pDefaultAttr;
		return m_SharedResInfo.m_AttrHash.Remove(pStrControlName);
	}
	else
	{
		CDuiString* pDefaultAttr = static_cast<CDuiString*>(m_ResInfo.m_AttrHash.Find(pStrControlName));
		if( !pDefaultAttr ) return false;

		delete pDefaultAttr;
		return m_ResInfo.m_AttrHash.Remove(pStrControlName);
	}
}

void CPaintManagerUI::RemoveAllDefaultAttributeList(bool bShared)
{
	if (bShared)
	{
		CDuiString* pDefaultAttr;
		for( int i = 0; i< m_SharedResInfo.m_AttrHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_SharedResInfo.m_AttrHash.GetAt(i)) {
				pDefaultAttr = static_cast<CDuiString*>(m_SharedResInfo.m_AttrHash.Find(key));
				if (pDefaultAttr) delete pDefaultAttr;
			}
		}
		m_SharedResInfo.m_AttrHash.RemoveAll();
	}
	else
	{
		CDuiString* pDefaultAttr;
		for( int i = 0; i< m_ResInfo.m_AttrHash.GetSize(); i++ ) {
			if(LPCTSTR key = m_ResInfo.m_AttrHash.GetAt(i)) {
				pDefaultAttr = static_cast<CDuiString*>(m_ResInfo.m_AttrHash.Find(key));
				if (pDefaultAttr) delete pDefaultAttr;
			}
		}
		m_ResInfo.m_AttrHash.RemoveAll();
	}
}

void CPaintManagerUI::AddMultiLanguageString(int id, LPCTSTR pStrMultiLanguage)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	CDuiString* pMultiLanguage = new CDuiString(pStrMultiLanguage);
	if (pMultiLanguage != NULL)
	{
		CDuiString* pOldMultiLanguage = static_cast<CDuiString*>(m_SharedResInfo.m_MultiLanguageHash.Set(idBuffer, (LPVOID)pMultiLanguage));
		if (pOldMultiLanguage) delete pOldMultiLanguage;
	}
}

LPCTSTR CPaintManagerUI::GetMultiLanguageString(int id)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	CDuiString* pMultiLanguage = static_cast<CDuiString*>(m_SharedResInfo.m_MultiLanguageHash.Find(idBuffer));
	if (pMultiLanguage) return pMultiLanguage->GetData();
	return NULL;
}

bool CPaintManagerUI::RemoveMultiLanguageString(int id)
{
	TCHAR idBuffer[16];
	::ZeroMemory(idBuffer, sizeof(idBuffer));
	_itot(id, idBuffer, 10);

	CDuiString* pMultiLanguage = static_cast<CDuiString*>(m_SharedResInfo.m_MultiLanguageHash.Find(idBuffer));
	if( !pMultiLanguage ) return false;

	delete pMultiLanguage;
	return m_SharedResInfo.m_MultiLanguageHash.Remove(idBuffer);
}

void CPaintManagerUI::RemoveAllMultiLanguageString()
{
	CDuiString* pMultiLanguage;
	for( int i = 0; i< m_SharedResInfo.m_MultiLanguageHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_SharedResInfo.m_MultiLanguageHash.GetAt(i)) {
			pMultiLanguage = static_cast<CDuiString*>(m_SharedResInfo.m_MultiLanguageHash.Find(key));
			if (pMultiLanguage) delete pMultiLanguage;
		}
	}
	m_SharedResInfo.m_MultiLanguageHash.RemoveAll();
}

void CPaintManagerUI::ProcessMultiLanguageTokens(CDuiString& pStrMultiLanguage)
{
	// Replace string-tokens: %{nnn}, nnn=int
	int iPos = pStrMultiLanguage.Find(_T('%'));
	while( iPos >= 0 ) {
		if( pStrMultiLanguage.GetAt(iPos + 1) == _T('{') ) {
			int iEndPos = iPos + 2;
			while( isdigit(pStrMultiLanguage.GetAt(iEndPos)) ) iEndPos++;
			if( pStrMultiLanguage.GetAt(iEndPos) == '}' ) {
				LPCTSTR pStrTemp = CPaintManagerUI::GetMultiLanguageString((UINT)_ttoi(pStrMultiLanguage.GetData() + iPos + 2));
                if (pStrTemp)
				    pStrMultiLanguage.Replace(pStrMultiLanguage.Mid(iPos, iEndPos - iPos + 1), pStrTemp);
			}
		}
		iPos = pStrMultiLanguage.Find(_T('%'), iPos + 1);
	}
}

CControlUI* CPaintManagerUI::GetRoot() const
{
    ASSERT(m_pRoot);
    return m_pRoot;
}

CControlUI* CPaintManagerUI::FindControl(POINT pt) const
{
    ASSERT(m_pRoot);
    return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI* CPaintManagerUI::FindControl(LPCTSTR pstrName) const
{
    ASSERT(m_pRoot);
    return static_cast<CControlUI*>(m_mNameHash.Find(pstrName));
}

CControlUI* CPaintManagerUI::FindSubControlByPoint(CControlUI* pParent, POINT pt) const
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI* CPaintManagerUI::FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
}

CControlUI* CPaintManagerUI::FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex)
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    m_aFoundControls.Resize(iIndex + 1);
    return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
}

CStdPtrArray* CPaintManagerUI::FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass)
{
    if( pParent == NULL ) pParent = GetRoot();
    ASSERT(pParent);
    m_aFoundControls.Empty();
    pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
    return &m_aFoundControls;
}

CStdPtrArray* CPaintManagerUI::GetFoundControls()
{
    return &m_aFoundControls;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromNameHash(CControlUI* pThis, LPVOID pData)
{
    CPaintManagerUI* pManager = static_cast<CPaintManagerUI*>(pData);
    const CDuiString& sName = pThis->GetName();
    if( sName.IsEmpty() ) return NULL;
    // Add this control to the hash list
    pManager->m_mNameHash.Set(sName, pThis);
    return NULL; // Attempt to add all controls
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromCount(CControlUI* /*pThis*/, LPVOID pData)
{
    int* pnCount = static_cast<int*>(pData);
    (*pnCount)++;
    return NULL;  // Count all controls
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromPoint(CControlUI* pThis, LPVOID pData)
{
    LPPOINT pPoint = static_cast<LPPOINT>(pData);
    return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromTab(CControlUI* pThis, LPVOID pData)
{
    FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
    if( pInfo->pFocus == pThis ) {
        if( pInfo->bForward ) pInfo->bNextIsIt = true;
        return pInfo->bForward ? NULL : pInfo->pLast;
    }
    if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 ) return NULL;
    pInfo->pLast = pThis;
    if( pInfo->bNextIsIt ) return pThis;
    if( pInfo->pFocus == NULL ) return pThis;
    return NULL;  // Examine all controls
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromShortcut(CControlUI* pThis, LPVOID pData)
{
    if( !pThis->IsVisible() ) return NULL; 
    FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
    if( pFS->ch == toupper(pThis->GetShortcut()) ) pFS->bPickNext = true;
    if( _tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL ) return NULL;   // Labels never get focus!
    return pFS->bPickNext ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromName(CControlUI* pThis, LPVOID pData)
{
    LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
    const CDuiString& sName = pThis->GetName();
    if( sName.IsEmpty() ) return NULL;
    return (_tcsicmp(sName, pstrName) == 0) ? pThis : NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlFromClass(CControlUI* pThis, LPVOID pData)
{
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    CStdPtrArray* pFoundControls = pThis->GetManager()->GetFoundControls();
    if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) {
        int iIndex = -1;
        while( pFoundControls->GetAt(++iIndex) != NULL ) ;
        if( iIndex < pFoundControls->GetSize() ) pFoundControls->SetAt(iIndex, pThis);
    }
    if( pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL ) return pThis; 
    return NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromClass(CControlUI* pThis, LPVOID pData)
{
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    if( _tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0 ) 
        pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
    return NULL;
}

CControlUI* CALLBACK CPaintManagerUI::__FindControlsFromUpdate(CControlUI* pThis, LPVOID pData)
{
	if( pThis->IsUpdateNeeded() ) {
		pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
		return pThis;
	}
	return NULL;
}

bool CPaintManagerUI::TranslateAccelerator(LPMSG pMsg)
{
	for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
	{
		LRESULT lResult = static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i])->TranslateAccelerator(pMsg);
		if( lResult == S_OK ) return true;
	}
	return false;
}

bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
{
	// Pretranslate Message takes care of system-wide messages, such as
	// tabbing and shortcut key-combos. We'll look for all messages for
	// each window and any child control attached.
	UINT uStyle = GetWindowStyle(pMsg->hwnd);
	UINT uChildRes = uStyle & WS_CHILD;	
	LRESULT lRes = 0;
	if (uChildRes != 0)
	{
		HWND hWndParent = ::GetParent(pMsg->hwnd);
		//code by redrain 2014.12.3,解决edit和webbrowser按tab无法切换焦点的bug
		//		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
		for( int i = m_aPreMessages.GetSize() - 1; i >= 0 ; --i ) 
		{
			CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);        
			HWND hTempParent = hWndParent;
			while(hTempParent)
			{

				if(pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;

					pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
					// 					if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
					// 						return true;
					// 
					// 					return false;  
				}
				hTempParent = GetParent(hTempParent);
			}

		}
	}
	else
	{
		for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) 
		{
			int size = m_aPreMessages.GetSize();
			CPaintManagerUI* pT = static_cast<CPaintManagerUI*>(m_aPreMessages[i]);
			if(pMsg->hwnd == pT->GetPaintWindow())
			{
				if (pT->TranslateAccelerator(pMsg))
					return true;

				if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
					return true;

				return false;
			}
		}
	}
	return false;
}

bool CPaintManagerUI::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
	ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
	return m_aTranslateAccelerator.Add(pTranslateAccelerator);
}

bool CPaintManagerUI::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
	for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++)
	{
		if (static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator[i]) == pTranslateAccelerator)
		{
			return m_aTranslateAccelerator.Remove(i);
		}
	}
	return false;
}

void CPaintManagerUI::UsedVirtualWnd(bool bUsed)
{
	m_bUsedVirtualWnd = bUsed;
}

} // namespace DuiLib
