#include "StdAfx.h"
#include "WinImplBase.h"

namespace DuiLib
{
	LONG WindowImplBase::GetStyle()
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;

		return styleValue;
	}

	LRESULT WindowImplBase::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg)
		{
		case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
#endif
		case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_CHAR:		lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
		case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSEHOVER:	lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
		case WM_MOUSELEAVE:		lRes = OnMouseLeave(uMsg,wParam,lParam,bHandled);	break;
		default:				bHandled = FALSE; break;
		}
		if (bHandled) return lRes;

		lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled) return lRes;

		if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
			return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT WindowImplBase::OnMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSetFocus( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnKillFocus( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnKeyDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~( WS_CAPTION );
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

		m_PaintManager.Init(m_hWnd);
		m_PaintManager.AddPreMessageFilter(this);

		CDialogBuilder builder;
		if (m_PaintManager.GetResourcePath().IsEmpty())
		{	// 允许更灵活的资源路径定义
			CDuiString strResourcePath=m_PaintManager.GetInstancePath();
			strResourcePath+=GetSkinFolder();
			m_PaintManager.SetResourcePath(strResourcePath.GetData());
		}

		switch(GetResourceType())
		{
		case UILIB_ZIP:
			m_PaintManager.SetResourceZip(GetZIPFileName(), true);
			break;
		case UILIB_ZIPRESOURCE:
			{
				HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), GetResID(), GetResType());
				if( hResource == NULL )
					break;
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
				if( hGlobal == NULL ) 
				{
#if defined(WIN32) && !defined(UNDER_CE)
					::FreeResource(hResource);
#endif
					break;
				}
				dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
				if( dwSize == 0 )
				{
#if defined(WIN32) && !defined(UNDER_CE)
					::FreeResource(hResource);
#endif
					break;
				}
				m_lpResourceZIPBuffer = new BYTE[ dwSize ];
				if (m_lpResourceZIPBuffer != NULL)
				{
					::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
				}
#if defined(WIN32) && !defined(UNDER_CE)
				::FreeResource(hResource);
#endif
				m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
			}
			break;
		}

		CControlUI* pRoot = builder.Create(GetSkinFile(), (UINT)0, this, &m_PaintManager);
		ASSERT(pRoot);
		if (pRoot==NULL)
		{
			MessageBox(NULL,_T("加载资源文件失败"),_T("Duilib"),MB_OK|MB_ICONERROR);
			ExitProcess(1);
			return 0;
		}
		m_PaintManager.AttachDialog(pRoot);
		m_PaintManager.AddNotifier(this);
		m_PaintManager.SetBackgroundTransparent(TRUE);

		InitWindow();
		return 0;
	}

	LRESULT WindowImplBase::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnChar( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		SIZE szRoundCorner = m_PaintManager.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetClientRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
#endif
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseHover( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnMouseWheel( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnGetMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		CDuiRect rcMonitor = oMonitor.rcMonitor;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		// 计算最大化时，正确的原点坐标
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;

		lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
		lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();

		lpMMI->ptMinTrackSize.x =m_PaintManager.GetMinInfo().cx;
		lpMMI->ptMinTrackSize.y =m_PaintManager.GetMinInfo().cy;


		bHandled = TRUE;
		return 0;
	}

	LRESULT WindowImplBase::OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) )
		{
			RECT rcSizeBox = m_PaintManager.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
			{
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}

			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_PaintManager.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
				if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 &&
					_tcsicmp(pControl->GetClass(), _T("EditUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT WindowImplBase::OnNcPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnNcCalcSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = TRUE;
		LPRECT pRect=NULL;

		if ( wParam == TRUE)
		{
			LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
			pRect=&pParam->rgrc[0];
		}
		else
		{
			pRect=(LPRECT)lParam;
		}

		if ( ::IsZoomed(m_hWnd))
		{	// 最大化时，计算当前显示器最适合宽高度
			// Win7屏幕扩展模式不会超过边界
			MONITORINFO oMonitor = {};
			oMonitor.cbSize = sizeof(oMonitor);
			::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
			CDuiRect rcWork = oMonitor.rcWork;
			CDuiRect rcMonitor = oMonitor.rcMonitor;
			rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

			pRect->right = pRect->left + rcWork.GetWidth();
			pRect->bottom = pRect->top + rcWork.GetHeight();
			return WVR_REDRAW;	// 要求重绘，防止黑色背景闪烁
		}

		return 0;
	}

	LRESULT WindowImplBase::OnNcActivate( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT WindowImplBase::OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::OnClose( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT WindowImplBase::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/ )
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return ResponseDefaultKeyEvent(wParam);
			default:
				break;
			}
		}
		return FALSE;
	}

	CControlUI* WindowImplBase::CreateControl( LPCTSTR pstrClass )
	{
		return NULL;
	}

	LPCTSTR WindowImplBase::GetResID() const
	{
		return _T("");
	}

	LPCTSTR WindowImplBase::GetZIPFileName() const
	{
		return _T("");
	}

	UILIB_RESOURCETYPE WindowImplBase::GetResourceType() const
	{
		return UILIB_FILE;
	}

	UINT WindowImplBase::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}

	BOOL WindowImplBase::OnEscapeCloseWindow( void ) const
	{
		return TRUE;
	}

	LRESULT WindowImplBase::ResponseDefaultKeyEvent( WPARAM wParam )
	{
		if (wParam == VK_RETURN)
		{
			return FALSE;
		}
		else if (wParam == VK_ESCAPE)
		{
			if (OnEscapeCloseWindow())
				Close(IDCANCEL);
			return TRUE;
		}

		return FALSE;
	}

	LPCTSTR WindowImplBase::GetWindowClassName( void ) const
	{
		return _T("WDGuiFoundation");
	}

	void WindowImplBase::OnFinalMessage( HWND hWnd )
	{
		m_PaintManager.RemovePreMessageFilter(this);
		m_PaintManager.RemoveNotifier(this);
		m_PaintManager.ReapObjects(m_PaintManager.GetRoot());
	}

	void WindowImplBase::InitWindow()
	{

	}

	WindowImplBase::~WindowImplBase()
	{

	}

	WindowImplBase::WindowImplBase()
	{

	}

	LPCTSTR WindowImplBase::GetResType() const
	{
		return _T("");
	}

	void WindowImplBase::AddNotify(INotifyUI *pNotify)
	{
		m_PaintManager.AddNotifier(pNotify);
	}

	CPaintManagerUI* WindowImplBase::GetPaintManager()
	{
		return &m_PaintManager;
	}



	__declspec(selectany) LPBYTE WindowImplBase::m_lpResourceZIPBuffer=NULL;
}

