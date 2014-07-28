#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXCtrl;


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXWnd : public CWindowWnd
{
public:
    HWND Init(CActiveXCtrl* pOwner, HWND hWndParent);

    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void DoVerb(LONG iVerb);

    LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    CActiveXCtrl* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXEnum : public IEnumUnknown
{
public:
    CActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
    {
        m_pUnk->AddRef();
    }
    ~CActiveXEnum()
    {
        m_pUnk->Release();
    }

    LONG m_iPos;
    ULONG m_dwRef;
    IUnknown* m_pUnk;

    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        LONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }
    STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
    {
        if( pceltFetched != NULL ) *pceltFetched = 0;
        if( ++m_iPos > 1 ) return S_FALSE;
        *rgelt = m_pUnk;
        (*rgelt)->AddRef();
        if( pceltFetched != NULL ) *pceltFetched = 1;
        return S_OK;
    }
    STDMETHOD(Skip)(ULONG celt)
    {
        m_iPos += celt;
        return S_OK;
    }
    STDMETHOD(Reset)(void)
    {
        m_iPos = 0;
        return S_OK;
    }
    STDMETHOD(Clone)(IEnumUnknown **ppenum)
    {
        return E_NOTIMPL;
    }
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXFrameWnd : public IOleInPlaceFrame
{
public:
    CActiveXFrameWnd(CActiveXUI* pOwner) : m_dwRef(1), m_pOwner(pOwner), m_pActiveObject(NULL)
    {
    }
    ~CActiveXFrameWnd()
    {
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
    }

    ULONG m_dwRef;
    CActiveXUI* m_pOwner;
    IOleInPlaceActiveObject* m_pActiveObject;

    // IUnknown
    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        ULONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
        else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }  
    // IOleInPlaceFrameWindow
    STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
    {
        return S_OK;
    }
    STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
    {
        return S_OK;
    }
    STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
    {
        return S_OK;
    }
    STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
    {
        return S_OK;
    }
    STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
    {
        return S_FALSE;
    }
    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd)
    {
        if( m_pOwner == NULL ) return E_UNEXPECTED;
        *phwnd = m_pOwner->GetManager()->GetPaintWindow();
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
    {
        return S_OK;
    }
    // IOleInPlaceUIWindow
    STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
    {
        return S_OK;
    }
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return INPLACE_E_NOTOOLSPACE;
    }
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
    {
        if( pActiveObject != NULL ) pActiveObject->AddRef();
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
        m_pActiveObject = pActiveObject;
        return S_OK;
    }
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CActiveXCtrl :
    public IOleClientSite,
    public IOleInPlaceSiteWindowless,
    public IOleControlSite,
    public IObjectWithSite,
    public IOleContainer
{
    friend class CActiveXUI;
    friend class CActiveXWnd;
public:
    CActiveXCtrl();
    ~CActiveXCtrl();

    // IUnknown
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

    // IObjectWithSite
    STDMETHOD(SetSite)(IUnknown *pUnkSite);
    STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

    // IOleClientSite
    STDMETHOD(SaveObject)(void);       
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
    STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
    STDMETHOD(ShowObject)(void);        
    STDMETHOD(OnShowWindow)(BOOL fShow);        
    STDMETHOD(RequestNewObjectLayout)(void);

    // IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)(void);
    STDMETHOD(GetCapture)(void);
    STDMETHOD(SetCapture)(BOOL fCapture);
    STDMETHOD(GetFocus)(void);
    STDMETHOD(SetFocus)(BOOL fFocus);
    STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
    STDMETHOD(ReleaseDC)(HDC hDC);
    STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
    STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
    STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
    STDMETHOD(AdjustRect)(LPRECT prc);
    STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

    // IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
    STDMETHOD(RequestUIActivate)(void);

    // IOleInPlaceSite
    STDMETHOD(CanInPlaceActivate)(void);       
    STDMETHOD(OnInPlaceActivate)(void);        
    STDMETHOD(OnUIActivate)(void);
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtant);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)(void);
    STDMETHOD(DiscardUndoState)( void);
    STDMETHOD(DeactivateAndUndo)( void);
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

    // IOleControlSite
    STDMETHOD(OnControlInfoChanged)(void);      
    STDMETHOD(LockInPlaceActive)(BOOL fLock);       
    STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
    STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
    STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
    STDMETHOD(OnFocus)(BOOL fGotFocus);
    STDMETHOD(ShowPropertyFrame)(void);

    // IOleContainer
    STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
    STDMETHOD(LockContainer)(BOOL fLock);

    // IParseDisplayName
    STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

protected:
    HRESULT CreateActiveXWnd();

protected:
    LONG m_dwRef;
    CActiveXUI* m_pOwner;
    CActiveXWnd* m_pWindow;
    IUnknown* m_pUnkSite;
    IViewObject* m_pViewObject;
    IOleInPlaceObjectWindowless* m_pInPlaceObject;
    bool m_bLocked;
    bool m_bFocused;
    bool m_bCaptured;
    bool m_bUIActivated;
    bool m_bInPlaceActive;
    bool m_bWindowless;
};

CActiveXCtrl::CActiveXCtrl() : 
m_dwRef(1), 
m_pOwner(NULL), 
m_pWindow(NULL),
m_pUnkSite(NULL), 
m_pViewObject(NULL),
m_pInPlaceObject(NULL),
m_bLocked(false), 
m_bFocused(false),
m_bCaptured(false),
m_bWindowless(true),
m_bUIActivated(false),
m_bInPlaceActive(false)
{
}

CActiveXCtrl::~CActiveXCtrl()
{
    if( m_pWindow != NULL ) {
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
    }
    if( m_pUnkSite != NULL ) m_pUnkSite->Release();
    if( m_pViewObject != NULL ) m_pViewObject->Release();
    if( m_pInPlaceObject != NULL ) m_pInPlaceObject->Release();
}

STDMETHODIMP CActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    *ppvObject = NULL;
    if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
    else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
    else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
    else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
    else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
    else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
    else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
    if( *ppvObject != NULL ) AddRef();
    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::AddRef()
{
    return ++m_dwRef;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::Release()
{
    LONG lRef = --m_dwRef;
    if( lRef == 0 ) delete this;
    return lRef;
}

STDMETHODIMP CActiveXCtrl::SetSite(IUnknown *pUnkSite)
{
    DUITRACE(_T("AX: CActiveXCtrl::SetSite"));
    if( m_pUnkSite != NULL ) {
        m_pUnkSite->Release();
        m_pUnkSite = NULL;
    }
    if( pUnkSite != NULL ) {
        m_pUnkSite = pUnkSite;
        m_pUnkSite->AddRef();
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetSite"));
    if( ppvSite == NULL ) return E_POINTER;
    *ppvSite = NULL;
    if( m_pUnkSite == NULL ) return E_FAIL;
    return m_pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP CActiveXCtrl::SaveObject(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::SaveObject"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetMoniker"));
    if( ppmk != NULL ) *ppmk = NULL;
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetContainer(IOleContainer** ppContainer)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetContainer"));
    if( ppContainer == NULL ) return E_POINTER;
    *ppContainer = NULL;
    HRESULT Hr = E_NOTIMPL;
    if( m_pUnkSite != NULL ) Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    if( FAILED(Hr) ) Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::ShowObject(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::ShowObject"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    HDC hDC = ::GetDC(m_pOwner->m_hwndHost);
    if( hDC == NULL ) return E_FAIL;
    if( m_pViewObject != NULL ) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_pOwner->m_rcItem, (RECTL*) &m_pOwner->m_rcItem, NULL, NULL);
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnShowWindow(BOOL fShow)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnShowWindow"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::RequestNewObjectLayout(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::RequestNewObjectLayout"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::CanWindowlessActivate(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::CanWindowlessActivate"));
    return S_OK;  // Yes, we can!!
}

STDMETHODIMP CActiveXCtrl::GetCapture(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetCapture"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return m_bCaptured ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetCapture(BOOL fCapture)
{
    DUITRACE(_T("AX: CActiveXCtrl::SetCapture"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    m_bCaptured = (fCapture == TRUE);
    if( fCapture ) ::SetCapture(m_pOwner->m_hwndHost); else ::ReleaseCapture();
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetFocus(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetFocus"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return m_bFocused ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetFocus(BOOL fFocus)
{
    DUITRACE(_T("AX: CActiveXCtrl::SetFocus"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( fFocus ) m_pOwner->SetFocus();
    m_bFocused = (fFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetDC"));
    if( phDC == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *phDC = ::GetDC(m_pOwner->m_hwndHost);
    if( (grfFlags & OLEDC_PAINTBKGND) != 0 ) {
        CDuiRect rcItem = m_pOwner->GetPos();
        if( !m_bWindowless ) rcItem.ResetOffset();
        ::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ReleaseDC(HDC hDC)
{
    DUITRACE(_T("AX: CActiveXCtrl::ReleaseDC"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
    DUITRACE(_T("AX: CActiveXCtrl::InvalidateRect"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    return ::InvalidateRect(m_pOwner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
    DUITRACE(_T("AX: CActiveXCtrl::InvalidateRgn"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return ::InvalidateRgn(m_pOwner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    DUITRACE(_T("AX: CActiveXCtrl::ScrollRect"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::AdjustRect(LPRECT prc)
{
    DUITRACE(_T("AX: CActiveXCtrl::AdjustRect"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnDefWindowMessage"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *plResult = ::DefWindowProc(m_pOwner->m_hwndHost, msg, wParam, lParam);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
    DUITRACE(_T("AX: CActiveXCtrl::OnInPlaceActivateEx"));
    ASSERT(m_pInPlaceObject==NULL);
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    ::OleLockRunning(m_pOwner->m_pUnk, TRUE, FALSE);
    HWND hWndFrame = m_pOwner->GetManager()->GetPaintWindow();
    HRESULT Hr = E_FAIL;
    if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 ) {
        m_bWindowless = true;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
        m_pOwner->m_hwndHost = hWndFrame;
        m_pOwner->GetManager()->AddMessageFilter(m_pOwner);
    }
    if( FAILED(Hr) ) {
        m_bWindowless = false;
        Hr = CreateActiveXWnd();
        if( FAILED(Hr) ) return Hr;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
    }
    if( m_pInPlaceObject != NULL ) {
        CDuiRect rcItem = m_pOwner->m_rcItem;
        if( !m_bWindowless ) rcItem.ResetOffset();
        m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    m_bInPlaceActive = SUCCEEDED(Hr);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
    DUITRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivateEx"));
    m_bInPlaceActive = false;
    if( m_pInPlaceObject != NULL ) {
        m_pInPlaceObject->Release();
        m_pInPlaceObject = NULL;
    }
    if( m_pWindow != NULL ) {
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
        m_pWindow = NULL;
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::RequestUIActivate(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::RequestUIActivate"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::CanInPlaceActivate(void)       
{
    DUITRACE(_T("AX: CActiveXCtrl::CanInPlaceActivate"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivate(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnInPlaceActivate"));
    BOOL bDummy = FALSE;
    return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP CActiveXCtrl::OnUIActivate(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnUIActivate"));
    m_bUIActivated = true;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetWindowContext"));
    if( ppDoc == NULL ) return E_POINTER;
    if( ppFrame == NULL ) return E_POINTER;
    if( lprcPosRect == NULL ) return E_POINTER;
    if( lprcClipRect == NULL ) return E_POINTER;
	if (m_pWindow)
	{
		::GetClientRect(m_pWindow->GetHWND(),lprcPosRect);
		::GetClientRect(m_pWindow->GetHWND(),lprcClipRect);
	}
    *ppFrame = new CActiveXFrameWnd(m_pOwner);
    *ppDoc = NULL;
    ACCEL ac = { 0 };
    HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = m_pOwner->GetManager()->GetPaintWindow();
    lpFrameInfo->haccel = hac;
    lpFrameInfo->cAccelEntries = 1;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::Scroll(SIZE scrollExtant)
{
    DUITRACE(_T("AX: CActiveXCtrl::Scroll"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnUIDeactivate"));
    m_bUIActivated = false;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivate(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivate"));
    return OnInPlaceDeactivateEx(TRUE);
}

STDMETHODIMP CActiveXCtrl::DiscardUndoState(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::DiscardUndoState"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::DeactivateAndUndo(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::DeactivateAndUndo"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnPosRectChange"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetWindow(HWND* phwnd)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetWindow"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_hwndHost == NULL ) CreateActiveXWnd();
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    *phwnd = m_pOwner->m_hwndHost;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
{
    DUITRACE(_T("AX: CActiveXCtrl::ContextSensitiveHelp"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnControlInfoChanged(void)      
{
    DUITRACE(_T("AX: CActiveXCtrl::OnControlInfoChanged"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockInPlaceActive(BOOL fLock)       
{
    DUITRACE(_T("AX: CActiveXCtrl::LockInPlaceActive"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
{
    DUITRACE(_T("AX: CActiveXCtrl::GetExtendedControl"));
    if( ppDisp == NULL ) return E_POINTER;   
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    return m_pOwner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
}

STDMETHODIMP CActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
    DUITRACE(_T("AX: CActiveXCtrl::TransformCoords"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
    DUITRACE(_T("AX: CActiveXCtrl::TranslateAccelerator"));
    return S_FALSE;
}

STDMETHODIMP CActiveXCtrl::OnFocus(BOOL fGotFocus)
{
    DUITRACE(_T("AX: CActiveXCtrl::OnFocus"));
    m_bFocused = (fGotFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ShowPropertyFrame(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::ShowPropertyFrame"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
    DUITRACE(_T("AX: CActiveXCtrl::EnumObjects"));
    if( ppenum == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *ppenum = new CActiveXEnum(m_pOwner->m_pUnk);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockContainer(BOOL fLock)
{
    DUITRACE(_T("AX: CActiveXCtrl::LockContainer"));
    m_bLocked = fLock != FALSE;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
    DUITRACE(_T("AX: CActiveXCtrl::ParseDisplayName"));
    return E_NOTIMPL;
}

HRESULT CActiveXCtrl::CreateActiveXWnd()
{
    if( m_pWindow != NULL ) return S_OK;
    m_pWindow = new CActiveXWnd;
    if( m_pWindow == NULL ) return E_OUTOFMEMORY;
    m_pOwner->m_hwndHost = m_pWindow->Init(this, m_pOwner->GetManager()->GetPaintWindow());
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

HWND CActiveXWnd::Init(CActiveXCtrl* pOwner, HWND hWndParent)
{
    m_pOwner = pOwner;
    UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    Create(hWndParent, _T("UIActiveX"), uStyle, 0L, 0,0,0,0, NULL);
    return m_hWnd;
}

LPCTSTR CActiveXWnd::GetWindowClassName() const
{
    return _T("ActiveXWnd");
}

void CActiveXWnd::OnFinalMessage(HWND hWnd)
{
    //delete this; // 这里不需要清理，CActiveXUI会清理的
}

void CActiveXWnd::DoVerb(LONG iVerb)
{
    if( m_pOwner == NULL ) return;
    if( m_pOwner->m_pOwner == NULL ) return;
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL ) return;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    IOleClientSite* pOleClientSite = NULL;
    m_pOwner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &m_pOwner->m_pOwner->GetPos());
}

LRESULT CActiveXWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes=0;
    BOOL bHandled = TRUE;
    switch( uMsg ) {
    case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
    case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
    case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL: break;
    default:
        bHandled = FALSE;
    }
    if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT CActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if( m_pOwner->m_pViewObject == NULL ) bHandled = FALSE;
    return 1;
}

LRESULT CActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL ) return 0;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    DWORD dwMiscStatus = 0;
    pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
    if( !m_pOwner->m_bInPlaceActive ) DoVerb(OLEIVERB_INPLACEACTIVATE);
    bHandled = FALSE;
    return 0;
}

LRESULT CActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = true;
    if( !m_pOwner->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
    return 0;
}

LRESULT CActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = false;
    return 0;
}

LRESULT CActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(m_hWnd, &ps);
    ::EndPaint(m_hWnd, &ps);
    return 1;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CActiveXUI::CActiveXUI() : m_pUnk(NULL), m_pControl(NULL), m_hwndHost(NULL), m_bCreated(false), m_bDelayCreate(true)
{
    m_clsid = IID_NULL;
}

CActiveXUI::~CActiveXUI()
{
    ReleaseControl();
}

LPCTSTR CActiveXUI::GetClass() const
{
    return _T("ActiveXUI");
}

LPVOID CActiveXUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_ACTIVEX) == 0 ) return static_cast<CActiveXUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

HWND CActiveXUI::GetHostWindow() const
{
    return m_hwndHost;
}

static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
    int nPixelsPerInchX;    // Pixels per logical inch along width
    int nPixelsPerInchY;    // Pixels per logical inch along height
    HDC hDCScreen = ::GetDC(NULL);
    nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
    nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    ::ReleaseDC(NULL, hDCScreen);
    lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
    lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}

void CActiveXUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);
    if( m_hwndHost != NULL && !m_pControl->m_bWindowless ) 
        ::ShowWindow(m_hwndHost, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CActiveXUI::SetInternVisible(bool bVisible)
{
    CControlUI::SetInternVisible(bVisible);
    if( m_hwndHost != NULL && !m_pControl->m_bWindowless ) 
        ::ShowWindow(m_hwndHost, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CActiveXUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);

    if( !m_bCreated ) DoCreateControl();

    if( m_pUnk == NULL ) return;
    if( m_pControl == NULL ) return;

    SIZEL hmSize = { 0 };
    SIZEL pxSize = { 0 };
    pxSize.cx = m_rcItem.right - m_rcItem.left;
    pxSize.cy = m_rcItem.bottom - m_rcItem.top;
    PixelToHiMetric(&pxSize, &hmSize);

    if( m_pUnk != NULL ) {
        m_pUnk->SetExtent(DVASPECT_CONTENT, &hmSize);
    }
    if( m_pControl->m_pInPlaceObject != NULL ) {
        CDuiRect rcItem = m_rcItem;
        if( !m_pControl->m_bWindowless ) rcItem.ResetOffset();
        m_pControl->m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    if( !m_pControl->m_bWindowless ) {
        ASSERT(m_pControl->m_pWindow);
        ::MoveWindow(*m_pControl->m_pWindow, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, TRUE);
    }
}

void CActiveXUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    if( m_pControl != NULL && m_pControl->m_bWindowless && m_pControl->m_pViewObject != NULL )
    {
        m_pControl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_rcItem, (RECTL*) &m_rcItem, NULL, NULL); 
    }
}

void CActiveXUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("clsid")) == 0 ) CreateControl(pstrValue);
    else if( _tcscmp(pstrName, _T("modulename")) == 0 ) SetModuleName(pstrValue);
    else if( _tcscmp(pstrName, _T("delaycreate")) == 0 ) SetDelayCreate(_tcscmp(pstrValue, _T("true")) == 0);
    else CControlUI::SetAttribute(pstrName, pstrValue);
}

LRESULT CActiveXUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( m_pControl == NULL ) return 0;
    ASSERT(m_pControl->m_bWindowless);
    if( !m_pControl->m_bInPlaceActive ) return 0;
    if( m_pControl->m_pInPlaceObject == NULL ) return 0;
    if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pControl->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE && m_pControl->m_pViewObject != NULL ) {
            IViewObjectEx* pViewEx = NULL;
            m_pControl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
            if( pViewEx != NULL ) {
                POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcItem, ptMouse, 0, &dwHitResult);
                pViewEx->Release();
            }
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
    }
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
        // Keyboard messages just go when we have focus
        if( !IsFocused() ) return 0;
    }
    else {
        switch( uMsg ) {
        case WM_HELP:
        case WM_CONTEXTMENU:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }
    LRESULT lResult = 0;
    HRESULT Hr = m_pControl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    return lResult;
}

bool CActiveXUI::IsDelayCreate() const
{
    return m_bDelayCreate;
}

void CActiveXUI::SetDelayCreate(bool bDelayCreate)
{
    if( m_bDelayCreate == bDelayCreate ) return;
    if( bDelayCreate == false ) {
        if( m_bCreated == false && m_clsid != IID_NULL ) DoCreateControl();
    }
    m_bDelayCreate = bDelayCreate;
}

bool CActiveXUI::CreateControl(LPCTSTR pstrCLSID)
{
    CLSID clsid = { 0 };
    OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
    _tcsncpy(szCLSID, pstrCLSID, lengthof(szCLSID) - 1);
#endif
    if( pstrCLSID[0] == '{' ) ::CLSIDFromString(szCLSID, &clsid);
    else ::CLSIDFromProgID(szCLSID, &clsid);
    return CreateControl(clsid);
}

bool CActiveXUI::CreateControl(const CLSID clsid)
{
    ASSERT(clsid!=IID_NULL);
    if( clsid == IID_NULL ) return false;
    m_bCreated = false;
    m_clsid = clsid;
    if( !m_bDelayCreate ) DoCreateControl();
    return true;
}

void CActiveXUI::ReleaseControl()
{
    m_hwndHost = NULL;
    if( m_pUnk != NULL ) {
        IObjectWithSite* pSite = NULL;
        m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
        if( pSite != NULL ) {
            pSite->SetSite(NULL);
            pSite->Release();
        }
        m_pUnk->Close(OLECLOSE_NOSAVE);
        m_pUnk->SetClientSite(NULL);
        m_pUnk->Release(); 
        m_pUnk = NULL;
    }
    if( m_pControl != NULL ) {
        m_pControl->m_pOwner = NULL;
        m_pControl->Release();
        m_pControl = NULL;
    }
    m_pManager->RemoveMessageFilter(this);
}

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 

bool CActiveXUI::DoCreateControl()
{
    ReleaseControl();
    // At this point we'll create the ActiveX control
    m_bCreated = true;
    IOleControl* pOleControl = NULL;

    HRESULT Hr = -1;
    if( !m_sModuleName.IsEmpty() ) {
        HMODULE hModule = ::LoadLibrary((LPCTSTR)m_sModuleName);
        if( hModule != NULL ) {
            IClassFactory* aClassFactory = NULL;
            DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc)::GetProcAddress(hModule, "DllGetClassObject");
            Hr = aDllGetClassObjectFunc(m_clsid, IID_IClassFactory, (LPVOID*)&aClassFactory);
            if( SUCCEEDED(Hr) ) {
                Hr = aClassFactory->CreateInstance(NULL, IID_IOleObject, (LPVOID*)&pOleControl);
            }
            aClassFactory->Release();
        }
    }
    if( FAILED(Hr) ) {
        Hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);
    }
    ASSERT(SUCCEEDED(Hr));
    if( FAILED(Hr) ) return false;
    pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &m_pUnk);
    pOleControl->Release();
    if( m_pUnk == NULL ) return false;
    // Create the host too
    m_pControl = new CActiveXCtrl();
    m_pControl->m_pOwner = this;
    // More control creation stuff
    DWORD dwMiscStatus = 0;
    m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    IOleClientSite* pOleClientSite = NULL;
    m_pControl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    // Initialize control
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) m_pUnk->SetClientSite(pOleClientSite);
    IPersistStreamInit* pPersistStreamInit = NULL;
    m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
    if( pPersistStreamInit != NULL ) {
        Hr = pPersistStreamInit->InitNew();
        pPersistStreamInit->Release();
    }
    if( FAILED(Hr) ) return false;
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 ) m_pUnk->SetClientSite(pOleClientSite);
    // Grab the view...
    Hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_pControl->m_pViewObject);
    // Activate and done...
    m_pUnk->SetHostNames(OLESTR("UIActiveX"), NULL);
    if( m_pManager != NULL ) m_pManager->SendNotify((CControlUI*)this, DUI_MSGTYPE_SHOWACTIVEX, 0, 0, false);
    if( (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 ) {
        Hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, m_pManager->GetPaintWindow(), &m_rcItem);
        //::RedrawWindow(m_pManager->GetPaintWindow(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
    }
    IObjectWithSite* pSite = NULL;
    m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
    if( pSite != NULL ) {
        pSite->SetSite(static_cast<IOleClientSite*>(m_pControl));
        pSite->Release();
    }
    return SUCCEEDED(Hr);
}

HRESULT CActiveXUI::GetControl(const IID iid, LPVOID* ppRet)
{
    ASSERT(ppRet!=NULL);
    ASSERT(*ppRet==NULL);
    if( ppRet == NULL ) return E_POINTER;
    if( m_pUnk == NULL ) return E_PENDING;
    return m_pUnk->QueryInterface(iid, (LPVOID*) ppRet);
}

CLSID CActiveXUI::GetClisd() const
{
	return m_clsid;
}

CDuiString CActiveXUI::GetModuleName() const
{
    return m_sModuleName;
}

void CActiveXUI::SetModuleName(LPCTSTR pstrText)
{
    m_sModuleName = pstrText;
}

} // namespace DuiLib