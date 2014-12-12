#include "stdafx.h"
#include "WindowUI.h"
#include "Render/GDIRender.h"
#include "Control/ControlUI.h"
#include "Base/TemplateObject.h"
#include "Define/UIDefine.h"

#pragma comment(lib,"comctl32.lib")

CWindowUI::CWindowUI(void)
	: m_pRenderEngine(NULL)
	, m_pRootControl(NULL)
	, m_pEventClick(NULL)
	, m_pFocus(NULL)
	, m_pEventKey(NULL)
	, m_pEventHover(NULL)
	, m_hWndTooltip(NULL)
	, m_hInstance(NULL)
	, m_uTimerID(0x1000)
	, m_nAlpha(255)
	, m_bMouseTracking(false)
	, m_bMouseCapture(false)
	, m_bLayedWindow(false)
	, m_bShowUpdateRect(false)
	, m_bFirstLayout(true)
	, m_bUpdateNeeded(false)
	, m_bFocusNeeded(false)
	, m_hUpdateRectPen(NULL)
	, m_pDefaultFont(NULL)
{
	m_ptLastMousePos.x = -1;
	m_ptLastMousePos.y = -1;
	ZeroMemory(&m_ToolTip,sizeof(TOOLINFO));
}


CWindowUI::~CWindowUI(void)
{
	if ( m_hUpdateRectPen != NULL)
	{
		::DeleteObject(m_hUpdateRectPen);
		m_hUpdateRectPen = NULL;
	}
	for( int i = 0; i < m_arrayDelayedCleanup.GetSize(); i++ )
		delete static_cast<CControlUI*>(m_arrayDelayedCleanup[i]);
	for( int i = 0; i < m_arrayAsyncNotify.GetSize(); i++ )
		delete static_cast<TNotifyUI*>(m_arrayAsyncNotify[i]);
	m_mapNameHash.Resize(0);

	if ( m_pRootControl != NULL)
	{
		delete m_pRootControl;
		m_pRootControl = NULL;
	}

	RemoveAllTimers();
}

SIZE CWindowUI::GetInitSize()
{
	return m_szInitWindowSize;
}

void CWindowUI::SetInitSize(int cx, int cy)
{
	m_szInitWindowSize.cx = cx;
	m_szInitWindowSize.cy = cy;
	if( m_pRootControl == NULL && m_hWnd != NULL )
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

RECT CWindowUI::GetSizeBox()
{
	return m_rcSizeBox;
}

void CWindowUI::SetSizeBox(RECT& rcSizeBox)
{
	m_rcSizeBox = rcSizeBox;
}

RECT CWindowUI::GetCaptionRect()
{
	return m_rcCaption;
}

void CWindowUI::SetCaptionRect(RECT& rcCaption)
{
	m_rcCaption = rcCaption;
}

void CWindowUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcscmp(lpszName, _T("size")) == 0 )
	{
		CDuiCodeOperation::StringToSize(lpszValue,&m_szInitWindowSize);
		this->SetInitSize(m_szInitWindowSize.cx,m_szInitWindowSize.cy);
	} 
	else if( _tcscmp(lpszName, _T("sizebox")) == 0 )
	{
		CDuiCodeOperation::StringToRect(lpszValue,&m_rcSizeBox);
	}
	else if( _tcscmp(lpszName, _T("caption")) == 0 )
	{
		CDuiCodeOperation::StringToRect(lpszValue,&m_rcCaption);
	}
	else if( _tcscmp(lpszName, _T("roundcorner")) == 0 )
	{
		CDuiCodeOperation::StringToSize(lpszValue,&m_szRoundCorner);
	} 
	else if( _tcscmp(lpszName, _T("mininfo")) == 0 )
	{
		CDuiCodeOperation::StringToSize(lpszValue,&m_szMinWindow);
	}
	else if( _tcscmp(lpszName, _T("maxinfo")) == 0 )
	{
		CDuiCodeOperation::StringToSize(lpszValue,&m_szMaxWindow);
	}
	else if( _tcscmp(lpszName, _T("showdirty")) == 0 )
	{
		m_bShowUpdateRect = _tcscmp(lpszValue, _T("true")) == 0;
	} 
	else if( _tcscmp(lpszName, _T("alpha")) == 0 )
	{
		m_nAlpha = _ttoi(lpszValue);
	} 
	else if( _tcscmp(lpszName, _T("bktrans")) == 0 )
	{
		m_bLayedWindow = _tcscmp(lpszValue, _T("true")) == 0;
	}
}

CControlUI * CWindowUI::GetRoot() const
{
	return m_pRootControl;
}

CControlUI * CWindowUI::GetItem(LPCTSTR lpszItemPath) const
{
	// Split lpszItemPath with dot
	// 使用 . 拆分lpszItemPath，按顺序查找
	VecString itemPath;
	CDuiStringOperation::splite(lpszItemPath,_T("."),itemPath);
	size_t nCount = itemPath.size();
	if ( nCount == 0 )
		return NULL;
	
	CControlUI* pControl = this->FindControl(itemPath[0].c_str());
	if ( nCount == 1 || pControl == NULL)
		return pControl;

	IContainerUI *pContainer =static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")));
	for ( size_t i =1 ;i<nCount ; ++i)
	{
			pControl = pContainer->FindSubControl(itemPath[i].c_str());
			if ( pControl == NULL)
				break;

			pContainer =static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer")));
			if ( pContainer == NULL )
				return NULL;
	}

	return pControl;
}

CControlUI* CWindowUI::FindControl(POINT pt) const
{
	if (m_pRootControl)
		return m_pRootControl->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);

	return NULL;
}

CControlUI* CWindowUI::FindControl(LPCTSTR lpszName) const
{
	if (m_pRootControl && m_mapNameHash.GetSize() > 0 )
		return static_cast<CControlUI*>(m_mapNameHash.Find(lpszName));

	return NULL;
}

CControlUI* CWindowUI::FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const
{
	if( pParent == NULL )
		pParent = m_pRootControl;
	if (pParent)
		return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);

	return NULL;
}

UINT CWindowUI::DoModal()
{
	if ( !::IsWindow(m_hWnd))
		return -1;

	m_bIsDoModal = true;
	CUIEngine *pUIEngine = CUIEngine::GetInstance();

	// 获取父窗口句柄，并在自绘窗口队列中查找
	HWND hWndOwner = ::GetWindow(m_hWnd,GW_OWNER);
	CWindowUI* pParentWindow = pUIEngine->GetWindow(hWndOwner);
	if ( pParentWindow && pParentWindow->m_bMouseCapture )
	{   // 父窗口也是自绘窗口，关闭父窗口响应鼠标消息
		if ( ::GetCapture() == pParentWindow->m_hWnd )
			::ReleaseCapture();
		pParentWindow->m_bMouseCapture  = false;
	}
	::ShowWindow(m_hWnd,SW_SHOWNORMAL);
	::EnableWindow(hWndOwner,FALSE);		// 禁用父窗口鼠标键盘输入

	UINT nRet = 0;
	MSG msg = { 0 };
	do 
	{
		if ( !::IsWindow(m_hWnd))
			break;
		if ( ! ::GetMessage(&msg, NULL, 0, 0) )
			break;

		if ( msg.hwnd == m_hWnd && msg.message == WM_CLOSE )
		{
			::EnableWindow(hWndOwner,TRUE);		// 恢复父窗口所有消息输入
			::SetFocus(hWndOwner);
			nRet = msg.wParam;
		}

		if( !pUIEngine->TranslateMessage(&msg) )
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT );

	::EnableWindow(hWndOwner,TRUE);
	::SetFocus(hWndOwner);
	::SetCapture(hWndOwner);

	if ( msg.message == WM_QUIT)
		::PostQuitMessage(msg.wParam);

	if ( m_bIsDoModal && m_bIsAutoDelete)
		delete this;

	return nRet;
}

bool CWindowUI::IsModal()
{
	return m_bIsDoModal;
}

void CWindowUI::MaximizeWindow()
{
	if ( ::IsWindow(m_hWnd))
	{
		::PostMessage(m_hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0);
	}
}

void CWindowUI::MinimizeWindow()
{
	if ( ::IsWindow(m_hWnd))
	{
		::PostMessage(m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);
	}
}

void CWindowUI::RestoreWindow()
{
	if ( ::IsWindow(m_hWnd))
	{
		::PostMessage(m_hWnd,WM_SYSCOMMAND,SC_RESTORE,0);
	}
}

bool CWindowUI::IsMaximized()
{
	return ::IsZoomed(m_hWnd) == 0;
}

bool CWindowUI::IsMinimized()
{
	return ::IsIconic(m_hWnd) == 0 ;
}

HWND CWindowUI::CreateDuiWindow(HWND hwndParent, LPCTSTR lpszWindowName,DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/)
{
#ifdef _DEBUG
	m_dwRunningThread = ::GetCurrentThreadId();
#endif // _DEBUG
	return CWindowWnd::Create(hwndParent,lpszWindowName,dwStyle,dwExStyle);
}

LRESULT CWindowUI::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( ::IsWindow(m_hWnd))
	{
		bool bHandled = false;
		LRESULT lRes= this->MessageHandler(uMsg,wParam,lParam,bHandled);
		if ( bHandled == true)
		{
			return lRes;
		}
		else
		{
			return CWindowWnd::WindowProc(uMsg,wParam,lParam);
		}
	}

	return S_FALSE;
}

LRESULT CWindowUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{   // 自绘窗口真实处理所有窗口消息

	if ( uMsg == WM_DIRECTUI_MESSAGE )
	{
		// 优先处理异步事件队列
		TNotifyUI* pMsg = NULL;
		while( pMsg = static_cast<TNotifyUI*>(m_arrayAsyncNotify.GetAt(0)) )
		{
			// Pop出一个内部事件
			m_arrayAsyncNotify.Remove(0);
			SendNotifyEvent(pMsg);
			delete pMsg;
		}

		// 延迟控件删除
		CControlUI* pControl = NULL;
		while ( pControl = static_cast<CControlUI*>(m_arrayDelayedCleanup.GetAt(0)) )
		{
			m_arrayDelayedCleanup.Remove(0);
			delete pControl;
		}
		
		bHandled = true;
		return S_OK;
	}

	LRESULT lResult = S_OK;

	int nCount = m_arrayWindowMessageFilters.GetSize();
	for( int i = 0; i < nCount; i++ ) 
	{   // Windows Message Filters
		// 给注册了的接口发送消息，使其有机会过滤消息
		lResult = static_cast<IMessageFilterUI*>(m_arrayWindowMessageFilters[i])->MessageFilter(uMsg, wParam, lParam, bHandled);
		if( bHandled )
			return lResult;
	}
	
/*
	窗口消息处理一般有如下几种情况
1.		直接处理并返回，如WM_CREATE，WM_NCCALCSIZE
2.		转发为内部事件【TEventUI】，分发给UI控件自己处理，如WM_LBUTTONDOWN
3.		转发为外部事件【TNotifyUI】，分发给外部监听回调进一步处理
4.		【2】中，UI控件自己处理完成后，也可能发送一个外部事件，通知控件状态
*/
	// 详细消息处理
	switch (uMsg)
	{
	case WM_CREATE:
		{
			bHandled = true;

			// 去掉标题栏，边框
			this->ModifyStyle(WS_CLIPSIBLINGS | WS_CLIPCHILDREN,WS_CAPTION);
			this->ModifyExStyle(0,WS_EX_WINDOWEDGE);

			// 等同于发送WM_NCCALCSIZE消息
			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			::SetWindowPos(m_hWnd, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
				rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

			CUIEngine *pUIEngine = CUIEngine::GetInstance();
			pUIEngine->SkinWindow(this);
			m_hInstance = pUIEngine->GetInstanceHandler();
			m_pRenderEngine = new CGDIRender;
			m_pRenderEngine->SetDevice(&m_OffscreenDC);
			m_bUpdateNeeded = true;
			m_bFirstLayout = true;
			TCHAR pWindowName[MAX_PATH] = { 0 };
			::GetWindowText(m_hWnd,pWindowName,MAX_PATH);

			CResourceManager * pResourceManager = pUIEngine->GetResourceManager();
			TemplateObject* pWindowTemplate = pResourceManager->GetWindowTemplate(pWindowName);
			if ( pWindowTemplate != NULL )
			{
				AttributeMap::iterator iter=pWindowTemplate->m_mapAttribute.begin();
				AttributeMap::iterator end=pWindowTemplate->m_mapAttribute.end();
				while (iter != end )
				{
					this->SetAttribute(iter->first.c_str(),iter->second.c_str());
					++iter;
				}

				TemplateObject* pControl = pWindowTemplate->GetChild(0);
				m_pRootControl = pResourceManager->CreateControlFromTemplate(pControl,this);
			}

			if ( m_pRootControl == NULL)
			{
				::MessageBox(m_hWnd,_T("DirectUI窗口构建失败"),_T("UIEngine"),MB_OK | MB_ICONERROR);
				PostMessage(WM_DESTROY);
			}

			this->InitControls(m_pRootControl);
		}
		break;
	case WM_DESTROY:
		{
			CUIEngine::GetInstance()->UnSkinWindow(this);
			if ( m_pRenderEngine != NULL)
			{
				delete m_pRenderEngine;
				m_pRenderEngine = NULL;
			}
			// 底层继续处理销毁事件
			lResult = S_FALSE;
		}
		break;
	case WM_NCCALCSIZE:
		{
			bHandled = true;
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
		}
		break;
	case WM_NCACTIVATE:
		{
			bHandled = true;
			//if( ::IsIconic(m_hWnd) )
			//	bHandled = true;

			if ( wParam == FALSE )
			{
				lResult = S_FALSE;
				//return TRUE;
			}
		}
		break;
	case WM_NCPAINT:
		{
			// ???
			//lResult = S_FALSE;
			bHandled = false;
		}
		break;
	case WM_NCHITTEST:
		{
			bHandled = true;

			POINT pt;
			pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
			::ScreenToClient(*this, &pt);

			RECT rcClient;
			::GetClientRect(*this, &rcClient);

			if( !::IsZoomed(*this) )
			{
				RECT rcSizeBox = m_rcSizeBox;
				if( pt.y < rcClient.top + rcSizeBox.top )
				{
					if( pt.x < rcClient.left + rcSizeBox.left )
						return HTTOPLEFT;
					if( pt.x > rcClient.right - rcSizeBox.right )
						return HTTOPRIGHT;
					return HTTOP;
				}
				else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
				{
					if( pt.x < rcClient.left + rcSizeBox.left )
						return HTBOTTOMLEFT;
					if( pt.x > rcClient.right - rcSizeBox.right )
						return HTBOTTOMRIGHT;
					return HTBOTTOM;
				}

				if( pt.x < rcClient.left + rcSizeBox.left )
					return HTLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right )
					return HTRIGHT;
			}

			RECT rcCaption = m_rcCaption;
			if( pt.x >= rcClient.left + rcCaption.left
				&& pt.x < rcClient.right - rcCaption.right
				&& pt.y >= rcCaption.top
				&& pt.y < rcCaption.bottom )
			{
					CControlUI* pControl = static_cast<CControlUI*>(FindControl(pt));
					if( pControl != NULL )
					{
						// 如果是注册的活动控件，则需要返回HTCLIENT以响应鼠标事件
						// 如果不是，则返回HTCAPTION处理标题栏拖动
						static CUIEngine * pEngine = CUIEngine::GetInstance();
						if ( !pEngine->IsActiveControl(pControl->GetClass()))
							return HTCAPTION;
					}
			}

			return HTCLIENT;
		}
		break;
	case WM_GETMINMAXINFO:
		{
			bHandled = true;
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

			// TODO 使用窗口属性控制最小窗口尺寸
			lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
			lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
		}
		break;
	case WM_ERASEBKGND:
		{   // 屏蔽背景擦除，防闪烁
			lResult = S_FALSE;
			bHandled = true;
		}
		break;
	case WM_PRINTCLIENT:
		{
			bHandled = true;

			RECT rcClient;
			::GetClientRect(m_hWnd, &rcClient);
			HDC hDC = (HDC) wParam;
			int save = ::SaveDC(hDC);
			m_pRenderEngine->SetDevice(hDC);
			m_pRootControl->Render(m_pRenderEngine,&rcClient);
			// Check for traversing children. The crux is that WM_PRINT will assume
			// that the DC is positioned at frame coordinates and will paint the child
			// control at the wrong position. We'll simulate the entire thing instead.
			if( (lParam & PRF_CHILDREN) != 0 )
			{
				HWND hWndChild = ::GetWindow(m_hWnd, GW_CHILD);
				while( hWndChild != NULL )
				{
					RECT rcPos = { 0 };
					::GetWindowRect(hWndChild, &rcPos);
					::MapWindowPoints(HWND_DESKTOP, m_hWnd, reinterpret_cast<LPPOINT>(&rcPos), 2);
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
	case WM_PAINT:
		{
			bHandled = true;
			// 自绘，最大工程！！！

			//////////////////////////////////////////////////////////////////////////
			// 计算是否需要重绘
			RECT rcPaint = { 0 };
			if( !::GetUpdateRect(m_hWnd, &rcPaint, FALSE) )
				return 0;
			if( m_pRootControl == NULL )
			{
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWnd, &ps);
				::EndPaint(m_hWnd, &ps);
				return 0;
			}

			//////////////////////////////////////////////////////////////////////////
			// Window size changed
			if ( m_bUpdateNeeded )
			{
				m_bUpdateNeeded = false;
				CDuiRect rcClient ;
				::GetClientRect(m_hWnd, &rcClient);
				if( !rcClient.IsRectEmpty() )
				{
					if( m_pRootControl->IsUpdateNeeded() )
					{
						m_pRootControl->SetPosition(&rcClient);
					}
					else
					{
						CControlUI* pControl = NULL;
						while( pControl = m_pRootControl->FindControl(__FindControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST) )
						{
							pControl->SetPosition( &pControl->GetPosition() );
						}
					}

					if ( rcClient.GetWidth() != m_OffscreenDC.GetWidth()
						|| rcClient.GetHeight() != m_OffscreenDC.GetHeight() )
					{   // 窗口大小改变，重建画布
						m_OffscreenDC.Create(m_hPaintDC,rcClient.GetWidth(),rcClient.GetHeight());
					}
				}

				if ( m_bFirstLayout)
				{
					m_bFirstLayout = false;
					SendNotify(m_pRootControl, UINOTIFY_WindowInit, rcClient.GetWidth(), rcClient.GetHeight(), false);
				}
				else
				{
					SendNotify(m_pRootControl, UINOTIFY_WindowPaint, rcClient.GetWidth(), rcClient.GetHeight(), false);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Window Size Changed

			if ( m_bFocusNeeded )
			{
				SetNextTabControl();				
			}

			PAINTSTRUCT ps = {0};
			HDC hDC = ::BeginPaint(m_hWnd,&ps);
			int iSaveDC = ::SaveDC(m_OffscreenDC.GetSafeHdc());
			{
				m_pRenderEngine->SetDevice(&m_OffscreenDC);
				m_pRenderEngine->SetInvalidateRect(ps.rcPaint);
				m_pRootControl->Render(m_pRenderEngine,&ps.rcPaint);
				int nCount = m_arrayPostPaintControls.GetSize();
				for( int i = 0; i < nCount; i++ )
				{
					CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_arrayPostPaintControls[i]);
					pPostPaintControl->PostRender(m_pRenderEngine, &ps.rcPaint);
				}
			}

			if ( !m_bLayedWindow )
			{
				::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top, m_OffscreenDC.GetSafeHdc(), ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

				if( m_bShowUpdateRect )
				{
					if ( m_hUpdateRectPen == NULL)
					{
						m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
					}
					HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_hUpdateRectPen);
					::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
					::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
					::SelectObject(ps.hdc, hOldPen);
				}
			}
			else
			{

			}
			::RestoreDC(m_OffscreenDC.GetSafeHdc(),iSaveDC);
			::EndPaint(m_hWnd,&ps	);

			// If any of the painting requested a resize again, we'll need
			// to invalidate the entire window once more.
			if( m_bUpdateNeeded )
			{
				::InvalidateRect(m_hWnd, NULL, FALSE);
			}
		}
		break;
	case WM_SIZE:
		{
			bHandled = true;

#if defined(WIN32) && !defined(UNDER_CE)
			if( !::IsIconic(*this) && (m_szRoundCorner.cx != 0 || m_szRoundCorner.cy != 0) ) {
				CDuiRect rcWnd;
				::GetClientRect(*this, &rcWnd);
				rcWnd.Offset(-rcWnd.left, -rcWnd.top);
				rcWnd.right++; rcWnd.bottom++;
				HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, m_szRoundCorner.cx, m_szRoundCorner.cy);
				::SetWindowRgn(*this, hRgn, TRUE);
				::DeleteObject(hRgn);
			}
#endif

			if( m_pFocus != NULL )
			{
				// 焦点控件不为空，则发送事件消息给他
				TEventUI event;
				event.dwType = UIEVENT_WINDOWSIZE;
				event.pSender = m_pFocus;
				event.dwTimestamp = ::GetTickCount();
				event.wParam = wParam;
				event.lParam = lParam;
				m_pFocus->EventHandler(event);
			}
			// 标记根控件需要刷新
			// 为什么只添加标记，而不直接计算新坐标呢？
			if( m_pRootControl != NULL )
				m_pRootControl->NeedUpdate();
		}
		break;
		// WM_LBUTTONDOWN
		// WM_LBUTTONUP
		// WM_LBUTTONDBLCLK
		// WM_RBUTTONDOWN
		// WM_RBUTTONUP
		// WM_RBUTTONDBLCLK
		// 
		// 以上消息都使用鼠标指针坐标查找对应的控件，没有或者不属于当前窗口的跳过
		// xBUTTONDOWN中
		//		1.设置控件为窗口m_pEventClick成员
		//		2.设置控件为焦点
		//		3.设置当前窗口为鼠标捕获
		//		4.给控件发送 xBUTTONDOWN 事件
		// xBUTTONUP中
		//		1.释放鼠标捕获
		//		2.给控件发送 xBUTTONUP 事件
		//		3.设置窗口m_pEventClick为空
		// xBUTTONDBLCLK 消息处理与 BUTTONDOWN 类似
		//		唯一区别是发送给控件的是 xDBLCLICK 事件
	case WM_LBUTTONDOWN:
		{
			bHandled = true;

			// We alway set focus back to our app (this helps
			// when Win32 child windows are placed on the dialog
			// and we need to remove them on focus change).
			::SetFocus(m_hWnd);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL )
				break;
			if( pControl->GetManager() != this )
				break;
			m_pEventClick = pControl;
			pControl->SetFocus();
			SetCapture();
			TEventUI event;
			event.dwType = UIEVENT_LBUTTONDOWN;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->EventHandler(event);
		}
		break;
	case WM_LBUTTONUP:
		{
			bHandled = true;

			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL )
				break;
			ReleaseCapture();
			TEventUI event;
			event.dwType = UIEVENT_LBUTTONUP;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->EventHandler(event);
			m_pEventClick = NULL;
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			bHandled = true;

			::SetFocus(m_hWnd);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL )
				break;
			if( pControl->GetManager() != this )
				break;
			SetCapture();
			TEventUI event;
			event.dwType = UIEVENT_LDBLCLICK;
			event.pSender = pControl;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->EventHandler(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_RBUTTONDOWN:
		{
			bHandled = true;

			::SetFocus(m_hWnd);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL )
				break;
			if( pControl->GetManager() != this )
				break;
			pControl->SetFocus();
			SetCapture();
			TEventUI event;
			event.dwType = UIEVENT_RBUTTONDOWN;
			event.pSender = pControl;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->EventHandler(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_RBUTTONUP:
		{
			bHandled = true;

			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL )
				break;
			ReleaseCapture();
			TEventUI event;
			event.dwType = UIEVENT_RBUTTONUP;
			event.pSender = m_pEventClick;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->EventHandler(event);
			m_pEventClick = NULL;
		}
		break;
	case WM_RBUTTONDBLCLK:
		{
			bHandled = true;

			::SetFocus(m_hWnd);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL )
				break;
			if( pControl->GetManager() != this )
				break;
			SetCapture();
			TEventUI event;
			event.dwType = UIEVENT_RDBLCLICK;
			event.pSender = pControl;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.dwTimestamp = ::GetTickCount();
			pControl->EventHandler(event);
			m_pEventClick = pControl;
		}
		break;
	case WM_CONTEXTMENU:
		{
			bHandled = true;

			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(m_hWnd, &pt);
			m_ptLastMousePos = pt;
			if( m_pEventClick == NULL )
				break;
			ReleaseCapture();
			TEventUI event;
			event.dwType = UIEVENT_CONTEXTMENU;
			event.pSender = m_pEventClick;
			event.ptMouse = pt;
			event.wKeyState = (WORD)wParam;
			event.lParam = (LPARAM)m_pEventClick;
			event.dwTimestamp = ::GetTickCount();
			m_pEventClick->EventHandler(event);
			m_pEventClick = NULL;
		}
		break;
	//case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		{
			//bHandled = true;

			// 已经注册了鼠标跟踪则跳过
			if( !m_bMouseTracking )
			{
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_HOVER | TME_LEAVE;
				tme.hwndTrack = m_hWnd;
				tme.dwHoverTime = m_hWndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hWndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
				_TrackMouseEvent(&tme);
				// 标记已注册鼠标跟踪
				m_bMouseTracking = true;
			}

			// 查找当前鼠标指针所在控件，并更新最后鼠标指针坐标
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			m_ptLastMousePos = pt;
			CControlUI* pNewHover = FindControl(pt);

			// 鼠标指针没有对应的控件，返回
			if( pNewHover != NULL && pNewHover->GetManager() != this )
				break;

			TEventUI event;
			event.ptMouse = pt;
			event.dwTimestamp = ::GetTickCount();

			// 本次鼠标指针悬停控件与上一次不同，且上次悬停控件不为空
			// 发送MouseLeave事件给上一次的悬停控件
			if( pNewHover != m_pEventHover && m_pEventHover != NULL )
			{
				event.dwType = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventHover;
				m_pEventHover->EventHandler(event);
				m_pEventHover = NULL;
				if( m_hWndTooltip != NULL )
					::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			}

			// 本次鼠标指针悬停控件与上一次不同，且本次悬停控件不为空
			// 发送MouseEnter事件给本次的悬停控件
			// 更新最后悬停控件
			if( pNewHover != m_pEventHover && pNewHover != NULL )
			{
				event.dwType = UIEVENT_MOUSEENTER;
				event.pSender = pNewHover;
				pNewHover->EventHandler(event);
				m_pEventHover = pNewHover;
			}

			// 发送MouseMove事件
			if( m_pEventClick != NULL )
			{
				event.dwType = UIEVENT_MOUSEMOVE;
				event.pSender = m_pEventClick;
				m_pEventClick->EventHandler(event);
			}
			else if( pNewHover != NULL )
			{
				event.dwType = UIEVENT_MOUSEMOVE;
				event.pSender = pNewHover;
				pNewHover->EventHandler(event);
			}
		}
		break;
	case WM_MOUSEHOVER:
		{
			bHandled = true;

			// 鼠标悬停消息触发，关闭跟踪标志
			m_bMouseTracking = false;

			// 查找当前鼠标指针所在控件，没有则返回
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			CControlUI* pHover = FindControl(pt);
			if( pHover == NULL )
				break;
			
			// 向最后悬停控件发送MouseHover事件
			if( m_pEventHover != NULL )
			{
				TEventUI event;
				event.ptMouse = pt;
				event.dwType = UIEVENT_MOUSEHOVER;
				event.pSender = m_pEventHover;
				event.dwTimestamp = ::GetTickCount();
				m_pEventHover->EventHandler(event);
			}

			// 读取当前悬停控件Tooltip字符串，没有则返回
			CDuiString sToolTip = pHover->GetToolTip();
			if( sToolTip.empty() ) 
				return 0;

			// 显示当前悬停控件Tooltip
			::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
			m_ToolTip.cbSize = sizeof(TOOLINFO);
			m_ToolTip.uFlags = TTF_IDISHWND;
			m_ToolTip.hwnd = m_hWnd;
			m_ToolTip.uId = (UINT_PTR) m_hWnd;
			m_ToolTip.hinst = m_hInstance;
			m_ToolTip.lpszText = const_cast<LPTSTR>( sToolTip.c_str() );
			m_ToolTip.rect = pHover->GetPosition();
			if( m_hWndTooltip == NULL )
			{
				m_hWndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd, NULL, m_hInstance, NULL);
				::SendMessage(m_hWndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
			}
			::SendMessage( m_hWndTooltip,TTM_SETMAXTIPWIDTH,0, pHover->GetToolTipWidth());
			::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
			::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
			return 0;
		}
		break;
	case WM_MOUSELEAVE:
		{
			bHandled = true;

			if( m_hWndTooltip != NULL )
				::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			// 如果鼠标指针处于跟踪状态，强制发送WM_MOUSEMOVE消息
			// 使控件有机会处理Hover到Leave状态切换
			if( m_bMouseTracking )
				::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM) -1);

			// 鼠标悬停消息触发，关闭跟踪标志
			m_bMouseTracking = false;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			bHandled = true;

			// 将鼠标指针坐标转换为客户区坐标
			// 基于坐标查找对应控件
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(m_hWnd, &pt);
			m_ptLastMousePos = pt;
			CControlUI* pControl = FindControl(pt);

			// 没有对应控件，或者不属于当前窗口则跳过
			if( pControl == NULL )
				break;
			if( pControl->GetManager() != this )
				break;

			// 计算滚动方向，发送ScrollWheel事件
			int zDelta = (int) (short) HIWORD(wParam);
			TEventUI event;
			event.dwType = UIEVENT_SCROLLWHEEL;
			event.pSender = pControl;
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.lParam = lParam;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			pControl->EventHandler(event);

			// Let's make sure that the scroll item below the cursor is the same as before...
			::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
		}
		break;
	case WM_KEYDOWN:
		{
			if( m_pFocus == NULL )
				break;
			TEventUI event;
			event.dwType = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->EventHandler(event);
			m_pEventKey = m_pFocus;
		}
		break;
	case WM_KEYUP:
		{
			if( m_pEventKey == NULL )
				break;
			TEventUI event;
			event.dwType = UIEVENT_KEYUP;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pEventKey->EventHandler(event);
			m_pEventKey = NULL;
		}
		break;
	case WM_CHAR:
		{
			if( m_pFocus == NULL )
				break;
			TEventUI event;
			event.dwType = UIEVENT_CHAR;
			event.chKey = (TCHAR)wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pFocus->EventHandler(event);
		}
		break;
	case WM_SETCURSOR:
		{
			if( LOWORD(lParam) != HTCLIENT )
				break;
			if( m_bMouseCapture )
			{
				bHandled = true;
				return 0;
			}

			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_hWnd, &pt);
			CControlUI* pControl = FindControl(pt);
			if( pControl == NULL )
				break;
			if( (pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 )
				break;
			TEventUI event;
			event.dwType = UIEVENT_SETCURSOR;
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			pControl->EventHandler(event);
			bHandled = true;
		}
		break;
	case WM_TIMER:
		{
			int nCount = m_arrayTimers.GetSize();
			for( int i = 0; i < nCount; i++ )
			{
				const TimerInfo* pTimer = static_cast<TimerInfo*>(m_arrayTimers[i]);
				if( pTimer->hWnd == m_hWnd&& pTimer->uWinTimer == LOWORD(wParam) && pTimer->bKilled == false)
				{
					TEventUI event;
					event.dwType = UIEVENT_TIMER;
					event.pSender = pTimer->pSender;
					event.wParam = pTimer->nLocalID;
					event.dwTimestamp = ::GetTickCount();
					pTimer->pSender->EventHandler(event);
					break;
				}
			}
		}
		break;
	case WM_CLOSE:
		{
			// Make sure all matching "closing" events are sent
			TEventUI event ;
			event.ptMouse = m_ptLastMousePos;
			event.dwTimestamp = ::GetTickCount();
			if( m_pEventHover != NULL )
			{
				event.dwType = UIEVENT_MOUSELEAVE;
				event.pSender = m_pEventHover;
				m_pEventHover->EventHandler(event);
			}
			if( m_pEventClick != NULL )
			{
				event.dwType = UIEVENT_LBUTTONUP;
				event.pSender = m_pEventClick;
				m_pEventClick->EventHandler(event);
			}

			SetFocus(NULL);

			// Hmmph, the usual Windows tricks to avoid
			// focus loss...
			HWND hwndParent = GetWindowOwner(m_hWnd);
			if( hwndParent != NULL )
				::SetFocus(hwndParent);
		}
		break;
	//case WM_NOTIFY:
	//	{
	//		bHandled = true;
	//		LPNMHDR lpNMHDR = (LPNMHDR) lParam;
	//		if( lpNMHDR != NULL )
	//			return ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
	//	}
	//	break;
	//case WM_COMMAND:
	//	{
	//		if( lParam == 0 )
	//			break;
	//		bHandled = true;
	//		HWND hWndChild = (HWND) lParam;
	//		return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
	//	}
	//	break;
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORSTATIC:
	//	{
	//		// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
	//		// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
	//		if( lParam == 0 )
	//			break;
	//		bHandled = true;
	//		HWND hWndChild = (HWND) lParam;
	//		return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
	//	}
	//	break;
	default:
		{
			LRESULT lRes = ReflectNotifications(uMsg,wParam,lParam,bHandled);
			if ( bHandled == true)
				return lRes;
		}
		break;
	}

	return lResult;
}

LRESULT CWindowUI::ReflectNotifications(
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam,
	_Inout_ bool& bHandled)
{
	HWND hWndChild = NULL;

	switch(uMsg)
	{
	case WM_COMMAND:
		if(lParam != NULL)	// not from a menu
			hWndChild = (HWND)lParam;
		break;
	case WM_NOTIFY:
		hWndChild = ((LPNMHDR)lParam)->hwndFrom;
		break;
	//case WM_PARENTNOTIFY:
	//	switch(LOWORD(wParam))
	//	{
	//	case WM_CREATE:
	//	case WM_DESTROY:
	//		hWndChild = (HWND)lParam;
	//		break;
	//	default:

	//		hWndChild = GetDlgItem(HIWORD(wParam));
	//		break;
	//	}
	//	break;
	case WM_DRAWITEM:
		if(wParam)	// not from a menu
			hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
		break;
	//case WM_MEASUREITEM:
	//	if(wParam)	// not from a menu
	//		hWndChild = GetDlgItem(((LPMEASUREITEMSTRUCT)lParam)->CtlID);
	//	break;
	case WM_COMPAREITEM:
		if(wParam)	// not from a menu
			hWndChild = ((LPCOMPAREITEMSTRUCT)lParam)->hwndItem;
		break;
	case WM_DELETEITEM:
		if(wParam)	// not from a menu
			hWndChild = ((LPDELETEITEMSTRUCT)lParam)->hwndItem;

		break;
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
	case WM_HSCROLL:
	case WM_VSCROLL:
		hWndChild = (HWND)lParam;
		break;
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
		hWndChild = (HWND)lParam;
		break;
	default:
		break;
	}

	if(hWndChild == NULL)
	{
		bHandled = FALSE;
		return 1;
	}

	ATLASSERT(::IsWindow(hWndChild));
	return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
}

void CWindowUI::AddNotify(INotifyUI *pNotify)
{
	if ( m_arrayNotifyFilters.Find(pNotify) == -1)
	{
		m_arrayNotifyFilters.Add(pNotify);
	}
}

void CWindowUI::RemoveNotify(INotifyUI *pNotify)
{
	int nIndex = m_arrayNotifyFilters.Find(pNotify);
	if ( nIndex != -1 )
	{
		m_arrayNotifyFilters.Remove(nIndex);
	}
}

void CWindowUI::AddWindowMessageFilter(IMessageFilterUI* pFilter)
{
	if ( m_arrayWindowMessageFilters.Find(pFilter) == -1)
	{
		m_arrayWindowMessageFilters.Add(pFilter);
	}
}

void CWindowUI::RemoveWindowMessageFilter(IMessageFilterUI* pFilter)
{
	int nIndex = m_arrayWindowMessageFilters.Find(pFilter);
	if ( nIndex != -1 )
	{
		m_arrayWindowMessageFilters.Remove(nIndex);
	}
}

void CWindowUI::AddPreMessageFilter(IMessageFilterUI* pFilter)
{
	if ( m_arrayPreMessageFilters.Find(pFilter) == -1)
	{
		m_arrayPreMessageFilters.Add(pFilter);
	}
}

void CWindowUI::RemovePreMessageFilter(IMessageFilterUI* pFilter)
{
	int nIndex = m_arrayPreMessageFilters.Find(pFilter);
	if ( nIndex != -1 )
	{
		m_arrayPreMessageFilters.Remove(nIndex);
	}
}

void CWindowUI::SetWindowTitle(LPCTSTR lpszWindowTitle)
{
	m_strWindowTitle = lpszWindowTitle;
	::SetWindowText(m_hWnd,lpszWindowTitle);
}

LPCTSTR CWindowUI::GetWindowTitle()
{
	return m_strWindowTitle.c_str();
}

LPCTSTR CWindowUI::GetWindowClassName() const
{
	return _T("DaviGuiFoundation");
}

UINT CWindowUI::GetClassStyle() const
{
	return CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
}

bool CWindowUI::TranslateAccelerator(MSG *pMsg)
{
	int nCount = m_arrayTranslateAccelerators.GetSize();
	for ( int i=0 ; i<nCount ; ++i)
	{
		ITranslateAccelerator * pFilter = static_cast<ITranslateAccelerator*>(m_arrayTranslateAccelerators.GetAt(i));
		if ( pFilter->TranslateAccelerator(pMsg) == S_OK)
		{
			return true;
		}
	}
	return false;
}

bool CWindowUI::PreMessageHandler(const LPMSG pMsg, LRESULT& lRes)
{
	int nCount = m_arrayPreMessageFilters.GetSize();
	bool bHandled = false;
	for ( int i=0 ; i<nCount ; ++i)
	{
		IMessageFilterUI * pFilter = static_cast<IMessageFilterUI*>(m_arrayPreMessageFilters.GetAt(i));
		pFilter->MessageFilter(pMsg->message,pMsg->wParam,pMsg->lParam,bHandled);
		if ( bHandled == true)
		{
			return true;
		}
	}
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			// Tabbing between controls
			if( pMsg->wParam == VK_TAB )
			{
				if( m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL )
				{
					if( static_cast<CRichEditUI*>(m_pFocus)->IsWantTab() )
						return false;
				}
				SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
				return true;
			}
		}
		break;
	case WM_SYSCHAR:
		{
			// Handle ALT-shortcut key-combinations
			FindShortCut fs = {0};
			fs.ch = toupper((int)pMsg->wParam);
			CControlUI* pControl = m_pRootControl->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
			if( pControl != NULL )
			{
				pControl->SetFocus();
				pControl->Activate();
				return true;
			}
		}
		break;
	case WM_SYSKEYDOWN:
		{
			if( m_pFocus != NULL )
			{
				TEventUI event;
				event.dwType = UIEVENT_SYSKEY;
				event.chKey = (TCHAR)pMsg->wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				m_pFocus->EventHandler(event);
			}
		}
		break;
	}
	return false;
}

bool CWindowUI::SetNextTabControl(bool bForward /*= true*/)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	if( m_bUpdateNeeded && bForward )
	{
		m_bFocusNeeded = true;
		::InvalidateRect(m_hWnd, NULL, FALSE);
		return true;
	}

	// Find next/previous tabbable control
	FindTabInfo info1 = { 0 };
	info1.pFocus = m_pFocus;
	info1.bForward = bForward;
	CControlUI* pControl = m_pRootControl->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
	if( pControl == NULL )
	{
		if( bForward )
		{
			// Wrap around
			FindTabInfo info2 = { 0 };
			info2.pFocus = bForward ? NULL : info1.pLast;
			info2.bForward = bForward;
			pControl = m_pRootControl->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
		}
		else
		{
			pControl = info1.pLast;
		}
	}
	if( pControl != NULL )
		SetFocus(pControl);
	m_bFocusNeeded = false;
	return true;
}

void CWindowUI::SendNotify(TNotifyUI *pMsg, bool bAsync /*= false*/)
{
#ifdef _DEBUG
	TestUICrossThread();
#endif // _DEBUG

	pMsg->ptMouse = m_ptLastMousePos;
	pMsg->dwTimestamp = ::GetTickCount();
	if( !bAsync )
	{   // 同步事件，立刻发送
		SendNotifyEvent(pMsg);
	}
	else
	{   // 异步事件，加入队列
		TNotifyUI *pAsyncMsg = new TNotifyUI;
		pAsyncMsg->pSender = pMsg->pSender;
		pAsyncMsg->dwType = pMsg->dwType;
		pAsyncMsg->wParam = pMsg->wParam;
		pAsyncMsg->lParam = pMsg->lParam;
		pAsyncMsg->ptMouse = pMsg->ptMouse;
		pAsyncMsg->dwTimestamp = pMsg->dwTimestamp;
		m_arrayAsyncNotify.Add(pAsyncMsg);
		// 人工给消息队列增加一条触发异步事件的处理
		this->PostMessage(WM_DIRECTUI_MESSAGE);
	}
}

void CWindowUI::SendNotify(CControlUI* pControl, UINOTIFY dwType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	TNotifyUI Msg;
	Msg.pSender = pControl;
	Msg.dwType = dwType;
	Msg.wParam = wParam;
	Msg.lParam = lParam;
	SendNotify(&Msg, bAsync);
}

void CWindowUI::SetCapture()
{
	::SetCapture(m_hWnd);
	m_bMouseCapture = true;
}

void CWindowUI::ReleaseCapture()
{
	::ReleaseCapture();
	m_bMouseCapture = false;
}

bool CWindowUI::IsCaptured()
{
	return m_bMouseCapture;
}

CControlUI* CALLBACK CWindowUI::__FindControlFromPoint(CControlUI* pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	return ::PtInRect(&pThis->GetPosition(), *pPoint) ? pThis : NULL;
}

void CWindowUI::NeedUpdate()
{
	m_bUpdateNeeded = true;
}

CControlUI* CWindowUI::GetFocus() const
{
	return m_pFocus;
}

void CWindowUI::SetFocus(CControlUI* pControl)
{
	HWND hFocusWnd = ::GetFocus();
	if ( hFocusWnd != m_hWnd && pControl != m_pFocus)
		::SetFocus(m_hWnd);

	if ( pControl == m_pFocus )
	{   // Focus Control not change
		return;
	}
	if ( m_pFocus != NULL)
	{
		TEventUI event;
		event.dwType = UIEVENT_KILLFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->EventHandler(event);
		SendNotify(m_pFocus, UINOTIFY_KILLFOCUS);
		m_pFocus = NULL;
	}

	if ( pControl == NULL)
		return;

	if ( pControl != NULL
		&& pControl->GetManager() == this
		&& pControl->IsVisible()
		&& pControl->IsEnabled() )
	{
		m_pFocus = pControl;
		TEventUI event;
		event.dwType = UIEVENT_SETFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->EventHandler(event);
		SendNotify(m_pFocus, UINOTIFY_SETFOCUS);
	}
}

void CWindowUI::SetFocusNeeded(CControlUI* pControl)
{
	::SetFocus(m_hWnd);
	if( pControl == NULL ) return;
	if( m_pFocus != NULL )
	{
		TEventUI event;
		event.dwType = UIEVENT_KILLFOCUS;
		event.pSender = pControl;
		event.dwTimestamp = ::GetTickCount();
		m_pFocus->EventHandler(event);
		SendNotify(m_pFocus, UINOTIFY_KILLFOCUS);
		m_pFocus = NULL;
	}
	FindTabInfo info = { 0 };
	info.pFocus = pControl;
	info.bForward = false;
	m_pFocus = m_pRootControl->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
	m_bFocusNeeded = true;
	if( m_pRootControl != NULL )
		m_pRootControl->NeedUpdate();
}

void CWindowUI::Invalidate(RECT& rcItem)
{
#ifdef _DEBUG
	TestUICrossThread();
#endif // _DEBUG

	::InvalidateRect(m_hWnd, &rcItem, FALSE);
}

CControlUI* CALLBACK CWindowUI::__FindControlFromTab(CControlUI* pThis, LPVOID pData)
{
	FindTabInfo* pInfo = static_cast<FindTabInfo*>(pData);
	if( pInfo->pFocus == pThis )
	{
		if( pInfo->bForward )
			pInfo->bNextIsIt = true;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	if( (pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0 )
		return NULL;
	pInfo->pLast = pThis;
	if( pInfo->bNextIsIt )
		return pThis;
	if( pInfo->pFocus == NULL )
		return pThis;
	return NULL;  // Examine all controls
}

CControlUI* CALLBACK CWindowUI::__FindControlFromUpdate(CControlUI* pThis, LPVOID pData)
{
	return pThis->IsUpdateNeeded() ? pThis : NULL;
}

void CWindowUI::ReapObjects(CControlUI* pControl)
{
	if( pControl == m_pEventKey )
		m_pEventKey = NULL;
	if( pControl == m_pEventHover )
		m_pEventHover = NULL;
	if( pControl == m_pEventClick )
		m_pEventClick = NULL;
	if( pControl == m_pFocus )
		m_pFocus = NULL;
	KillTimer(pControl);

	CDuiString sName = pControl->GetName();
	if( !sName.empty() )
	{
		if( pControl == FindControl(sName.c_str()) )
			m_mapNameHash.Remove(sName.c_str());
	}
	for( int i = 0; i < m_arrayAsyncNotify.GetSize(); i++ )
	{
		TNotifyUI* pMsg = static_cast<TNotifyUI*>(m_arrayAsyncNotify[i]);
		if( pMsg->pSender == pControl )
			pMsg->pSender = NULL;
	}    
}

bool CWindowUI::SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse)
{
	ASSERT(pControl!=NULL);
	ASSERT(uElapse>0);
	for( int i = 0; i< m_arrayTimers.GetSize(); i++ )
	{
		TimerInfo* pTimer = static_cast<TimerInfo*>(m_arrayTimers[i]);
		if( pTimer->pSender == pControl
			&& pTimer->hWnd == m_hWnd
			&& pTimer->nLocalID == nTimerID )
		{
			if( pTimer->bKilled == true )
			{
				if( ::SetTimer(m_hWnd, pTimer->uWinTimer, uElapse, NULL) )
				{
					pTimer->bKilled = false;
					return true;
				}
				return false;
			}
			return false;
		}
	}

	m_uTimerID = (++m_uTimerID) % 0xFF;
	if( !::SetTimer(m_hWnd, m_uTimerID, uElapse, NULL) ) return FALSE;
	TimerInfo* pTimer = new TimerInfo;
	if( pTimer == NULL ) return FALSE;
	pTimer->hWnd = m_hWnd;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = m_uTimerID;
	pTimer->bKilled = false;
	return m_arrayTimers.Add(pTimer);
}

bool CWindowUI::KillTimer(CControlUI* pControl, UINT nTimerID)
{
	ASSERT(pControl!=NULL);
	for( int i = 0; i< m_arrayTimers.GetSize(); i++ )
	{
		TimerInfo* pTimer = static_cast<TimerInfo*>(m_arrayTimers[i]);
		if( pTimer->pSender == pControl
			&& pTimer->hWnd == m_hWnd
			&& pTimer->nLocalID == nTimerID )
		{
			if( pTimer->bKilled == false )
			{
				if( ::IsWindow(m_hWnd) )
					::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
				pTimer->bKilled = true;
				return true;
			}
		}
	}
	return false;
}

void CWindowUI::KillTimer(CControlUI* pControl)
{
	ASSERT(pControl!=NULL);
	int count = m_arrayTimers.GetSize();
	for( int i = 0, j = 0; i < count; i++ )
	{
		TimerInfo* pTimer = static_cast<TimerInfo*>(m_arrayTimers[i - j]);
		if( pTimer->pSender == pControl && pTimer->hWnd == m_hWnd )
		{
			if( pTimer->bKilled == false ) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
			delete pTimer;
			m_arrayTimers.Remove(i - j);
			j++;
		}
	}
}

void CWindowUI::RemoveAllTimers()
{
	for( int i = 0; i < m_arrayTimers.GetSize(); i++ )
	{
		TimerInfo* pTimer = static_cast<TimerInfo*>(m_arrayTimers[i]);
		if( pTimer->hWnd == m_hWnd )
		{
			if( pTimer->bKilled == false )
			{
				if( ::IsWindow(m_hWnd) )
					::KillTimer(m_hWnd, pTimer->uWinTimer);
			}
			delete pTimer;
		}
	}

	m_arrayTimers.Empty();
}

CControlUI* CALLBACK CWindowUI::__FindControlFromNameHash(CControlUI* pThis, LPVOID pData)
{
	CWindowUI* pManager = static_cast<CWindowUI*>(pData);
	CDuiString strName = pThis->GetName();
	if( strName.empty() )
		return NULL;
	// Add this control to the hash list
	pManager->m_mapNameHash.Set(strName.c_str(), pThis);
	return NULL; // Attempt to add all controls
}

void CWindowUI::AddDelayedCleanup(CControlUI* pControl)
{
	pControl->SetManager(this, NULL);
	m_arrayDelayedCleanup.Add(pControl);
	this->PostMessage(WM_DIRECTUI_MESSAGE, 0, 0L);
}

bool CWindowUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
{
	if( pControl == NULL )
		return false;
	pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent(),true);
	pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
	return true;
}
#ifdef _DEBUG
void CWindowUI::TestUICrossThread()
{
	// 警告：你在跨线程操作UI对象！
	// 轻则运行效果看运气（正确的说是看CPU心情）
	// 重则各种野指针漫天飞舞
	// 如果你选择注释掉下面这行代码，崩溃不要怪Duilib不给力，这是基础不过关
	ASSERT(m_dwRunningThread ==::GetCurrentThreadId());
}

CControlUI* CALLBACK CWindowUI::__FindControlFromName(CControlUI* pThis, LPVOID pData)
{
	LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
	const CDuiString sName = pThis->GetName();
	if( sName.empty() )
		return NULL;
	return (_tcsicmp(sName.c_str(), pstrName) == 0) ? pThis : NULL;
}

void CWindowUI::SendNotifyEvent(TNotifyUI *pMsg)
{
	do 
	{
		// 投递到控件注册的监听回调中处理
		INotifyUI *pNotifyFilter = pMsg->pSender->GetNotifyFilter();
		if( pNotifyFilter != NULL && pNotifyFilter->Notify(pMsg) )
		{
			// 过滤UI事件处理器返回已经处理了
			// 则不再投递到窗口全局通知接口
			break;
		}

		// 投递到窗口全局事件监听回调中
		int nCount = m_arrayNotifyFilters.GetSize();
		for( int j = 0; j < nCount; j++ )
		{  
			pNotifyFilter = static_cast<INotifyUI*>(m_arrayNotifyFilters[j]);
			if ( pNotifyFilter->Notify(pMsg) )
				break;
		}
	} while (false);
}

int CWindowUI::GetPostPaintCount() const
{
	return m_arrayPostPaintControls.GetSize();
}

bool CWindowUI::AddPostPaint(CControlUI* pControl)
{
	ASSERT(m_arrayPostPaintControls.Find(pControl) < 0);
	return m_arrayPostPaintControls.Add(pControl);
}

bool CWindowUI::RemovePostPaint(CControlUI* pControl)
{
	int nCount = m_arrayPostPaintControls.GetSize();
	for( int i = 0; i < nCount; i++ )
	{
		if( static_cast<CControlUI*>(m_arrayPostPaintControls[i]) == pControl )
		{
			return m_arrayPostPaintControls.Remove(i);
		}
	}
	return false;
}

bool CWindowUI::SetPostPaintIndex(CControlUI* pControl, int iIndex)
{
	RemovePostPaint(pControl);
	return m_arrayPostPaintControls.InsertAt(iIndex, pControl);
}

CControlUI* CALLBACK CWindowUI::__FindControlFromShortcut(CControlUI* pThis, LPVOID pData)
{
	if( !pThis->IsVisible() )
		return NULL; 
	FindShortCut* pFS = static_cast<FindShortCut*>(pData);
	if( pFS->ch == toupper(pThis->GetShortcut()) )
		pFS->bPickNext = true;
	if( _tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL )
		return NULL;   // Labels never get focus!
	return pFS->bPickNext ? pThis : NULL;
}

void CWindowUI::SetDefaultFont(LPCTSTR lpszFaceName,int nSize /*= 12*/, bool bBold /*= false*/, bool bUnderline/*= false*/, bool bItalic/*= false */,bool bStrikeout/*= false*/)
{
	if ( m_pDefaultFont != NULL )
	{
		delete m_pDefaultFont;
		m_pDefaultFont = new FontObject;
	}

	m_pDefaultFont->m_FaceName = lpszFaceName;
	m_pDefaultFont->m_nSize = nSize;
	m_pDefaultFont->m_bBold = bBold;
	m_pDefaultFont->m_bUnderline = bUnderline;
	m_pDefaultFont->m_bItalic = bItalic;
	m_pDefaultFont->m_bStrikeout = bStrikeout;
}

FontObject* CWindowUI::GetDefaultFont(void)
{
	if ( m_pDefaultFont != NULL)
		return m_pDefaultFont;

	return CResourceManager::GetInstance()->GetDefaultFont();
}

TEXTMETRIC CWindowUI::GetTM(HFONT hFont)
{
	TEXTMETRIC tm = { 0 };
	HFONT hOldFont = (HFONT) ::SelectObject(m_hPaintDC, hFont);
	::GetTextMetrics(m_hPaintDC, &tm);
	::SelectObject(m_hPaintDC, hOldFont);
	return tm;
}

#endif
