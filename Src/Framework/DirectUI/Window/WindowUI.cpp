#include "stdafx.h"
#include "WindowUI.h"
#include "Render/GDIRender.h"
#include "Control/ControlUI.h"
#include "Base/TemplateObject.h"
#include "Define/UIDefine.h"

#pragma comment(lib,"comctl32.lib")

CWindowUI::CWindowUI(void)
	: m_bIsModal(false)
	, m_bIsMaximized(false)
	, m_bIsMinimized(false)
	, m_pRenderEngine(NULL)
	, m_pRootControl(NULL)
	, m_pEventClick(NULL)
	, m_pFocus(NULL)
	, m_pEventKey(NULL)
	, m_pEventHover(NULL)
	, m_hWndTooltip(NULL)
	, m_uTimerID(0x1000)
	, m_hInstance(NULL)
	, m_bMouseTracking(false)
	, m_bLayedWindow(false)
{
	m_ptLastMousePos.x = -1;
	m_ptLastMousePos.y = -1;
	ZeroMemory(&m_ToolTip,sizeof(TOOLINFO));
}


CWindowUI::~CWindowUI(void)
{
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
	return NULL;
}

CControlUI* CWindowUI::FindControl(POINT pt) const
{
	ASSERT(m_pRootControl);
	return m_pRootControl->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI* CWindowUI::FindControl(LPCTSTR lpszName) const
{
	ASSERT(m_pRootControl);
	return static_cast<CControlUI*>(m_mapNameHash.Find(lpszName));
}

void CWindowUI::ShowWindow(int nCmdShow /*= SW_SHOW*/)
{
	if ( ::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd,nCmdShow);
	}
}

void CWindowUI::CloseWindow(UINT nRet /*= IDOK*/)
{

}

UINT CWindowUI::ShowModal()
{
	m_bIsModal = true;

	return 0;
}

void CWindowUI::EndModal(UINT nRet /*= IDOK*/)
{
	if ( m_bIsModal == false)
	{
		// 警告，不应该用这个函数关闭窗口
		return;
	}
}

bool CWindowUI::IsModal()
{
	return m_bIsModal;
}

void CWindowUI::MaximizeWindow()
{

}

void CWindowUI::MinimizeWindow()
{

}

void CWindowUI::RestoreWindow()
{

}

bool CWindowUI::IsMaximized()
{
	return m_bIsMaximized;
}

bool CWindowUI::IsMinimized()
{
	return m_bIsMinimized;
}

HWND CWindowUI::CreateDuiWindow(HWND hwndParent, LPCTSTR lpszWindowName,DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/)
{
#ifdef _DEBUG
	m_dwRunningThread = ::GetCurrentThreadId();
#endif // _DEBUG
	return CWindowWnd::Create(hwndParent,lpszWindowName,dwStyle,dwExStyle);
}

LRESULT CWindowUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			return CWindowWnd::HandleMessage(uMsg,wParam,lParam);
		}
	}

	return S_FALSE;
}

LRESULT CWindowUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{   // 自绘窗口真实处理所有窗口消息
	
	// 优先处理异步事件队列
	TNotifyUI* pMsg = NULL;
	while( pMsg = static_cast<TNotifyUI*>(m_arrayAsyncNotify.GetAt(0)) )
	{
		// Pop出一个内部事件
		m_arrayAsyncNotify.Remove(0);

		// 投递到控件注册的监听回调中处理
		if( pMsg->pSender != NULL )
		{
			if( pMsg->pSender->OnNotify )
				pMsg->pSender->OnNotify(pMsg);
		}

		// 投递到已注册的窗口全局事件监听回调中
		int nCount = m_arrayNotifyFilters.GetSize();
		for( int j = 0; j < nCount; j++ )
		{  
			static_cast<INotifyUI*>(m_arrayNotifyFilters[j])->Notify(pMsg);
		}
		delete pMsg;
	}

	LRESULT lResult = S_OK;

	int nCount = m_arrayPreMessageFilters.GetSize();
	for( int i = 0; i < nCount; i++ ) 
	{   // Windows Message Filters
		// 给注册了的接口发送消息，使其有机会过滤消息
		lResult = static_cast<IMessageFilterUI*>(m_arrayPreMessageFilters[i])->MessageFilter(uMsg, wParam, lParam, bHandled);
		if( bHandled )
			return lResult;
	}

	// 延迟控件删除
	if ( uMsg == WM_DIRECTUI_MESSAGE )
	{
		bHandled = true;
		for( int i = 0; i < m_arrayDelayedCleanup.GetSize(); i++ ) 
			delete static_cast<CControlUI*>(m_arrayDelayedCleanup[i]);
		m_arrayDelayedCleanup.Empty();
		return S_OK;
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

			// 去掉标题栏
			LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
			styleValue &= ~( WS_CAPTION );
			::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

			// 等同于发送WM_NCCALCSIZE消息
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
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

				//int nCount = pWindowTemplate->GetChildCount();
				//for (int i=0;i<nCount;++i)
				//{

				//}
				TemplateObject* pControl = pWindowTemplate->GetChild(0);
				m_pRootControl = pResourceManager->CreateControlFromTemplate(pControl,this);
			}
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
			if( ::IsIconic(*this) )
				bHandled = false;
			else
				bHandled = true;
			return (wParam == 0) ? TRUE : FALSE;
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
					if( pControl
						&& _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0
						&& _tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0
						&&_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0
						&&_tcsicmp(pControl->GetClass(), _T("EditUI")) != 0 )
						return HTCAPTION;
			}

			return HTCLIENT;
		}
		break;
	case WM_ERASEBKGND:
		{   // 屏蔽背景擦除，防闪烁
			lResult = S_FALSE;
			bHandled = true;
		}
		break;
	case WM_NCPAINT:
		{
			// ???
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
				return true;
			if( m_pRootControl == NULL )
			{
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWnd, &ps);
				::EndPaint(m_hWnd, &ps);
				return true;
			}

			//////////////////////////////////////////////////////////////////////////
			// Window size changed
			if ( m_bUpdateNeeded )
			{
				m_bUpdateNeeded = false;
				CDuiRect rcClient ;
				::GetClientRect(m_hWnd, &rcClient);
				if( !::IsRectEmpty(&rcClient) )
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
						m_OffscreenDC.Create(::GetDC(m_hWnd),rcClient.GetWidth(),rcClient.GetHeight());
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
			{
				m_pRenderEngine->SetDevice(&m_OffscreenDC);
				m_pRenderEngine->SetInvalidateRect(ps.rcPaint);
				m_pRootControl->Render(m_pRenderEngine,&ps.rcPaint);
				m_pRenderEngine->DrawImage(NULL,0,0,0);
			}

			if ( !m_bLayedWindow )
			{
				::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top, m_OffscreenDC.GetSafeHdc(), ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
			}
			else
			{

			}

			::EndPaint(m_hWnd,&ps	);
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
	case WM_MOUSEMOVE:
		{
			bHandled = true;

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
				return true;

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
		}
		break;
	case WM_MOUSELEAVE:
		{
			bHandled = true;

			// 鼠标悬停消息触发，关闭跟踪标志
			m_bMouseTracking = false;

			if( m_hWndTooltip != NULL )
				::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			if( m_bMouseTracking )
				::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM) -1);
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
				return true;

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

void CWindowUI::AddMessageFilter(IMessageFilterUI* pFilter)
{
	if ( m_arrayPreMessageFilters.Find(pFilter) == -1)
	{
		m_arrayPreMessageFilters.Add(pFilter);
	}
}

void CWindowUI::RemoveMessageFilter(IMessageFilterUI* pFilter)
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
	{   // send sync message immediately
		if( pMsg->pSender != NULL )
		{   // Send to control
			if( pMsg->pSender->OnNotify )
				pMsg->pSender->OnNotify(pMsg);
		}
		for( int i = 0; i < m_arrayNotifyFilters.GetSize(); i++ )
		{   // Send to all listeners
			static_cast<INotifyUI*>(m_arrayNotifyFilters[i])->Notify(pMsg);
		}
	}
	else
	{   // add async message to queue
		TNotifyUI *pMsg = new TNotifyUI;
		pMsg->pSender = pMsg->pSender;
		pMsg->dwType = pMsg->dwType;
		pMsg->wParam = pMsg->wParam;
		pMsg->lParam = pMsg->lParam;
		pMsg->ptMouse = pMsg->ptMouse;
		pMsg->dwTimestamp = pMsg->dwTimestamp;
		m_arrayAsyncNotify.Add(pMsg);
	}
}

void CWindowUI::SendNotify(CControlUI* pControl, DWORD dwType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
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
		&& pControl->GetManager() != this
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
	::PostMessage(m_hWnd, WM_DIRECTUI_MESSAGE, 0, 0L);
}

bool CWindowUI::InitControls(CControlUI* pControl, CControlUI* pParent /*= NULL*/)
{
	ASSERT(pControl);
	if( pControl == NULL )
		return false;
	pControl->SetManager(this, pParent != NULL ? pParent : pControl->GetParent());
	pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
	return true;
}
#ifdef _DEBUG
void CWindowUI::TestUICrossThread()
{
	// 警告：你在跨线程操作UI对象！
	// 轻则运行效果看运气（正确的说是看CPU心情）
	// 重则各种野指针漫天飞舞
	// 如果你选择注释掉下面这行代码，崩溃不要怪Duilib不给力，这是能力和水平问题
	ASSERT(m_dwRunningThread ==::GetCurrentThreadId());
}
#endif
