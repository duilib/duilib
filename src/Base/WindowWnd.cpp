#include "stdafx.h"
#include "WindowWnd.h"
#include <WindowsX.h>

CWindowWnd::CWindowWnd(void)
	: m_bSubclassed(false)
	, m_bIsAutoDelete(false)
	, m_OldWndProc(::DefWindowProc)
	, m_hWnd(NULL)
	, m_hPaintDC(NULL)
	, m_bIsDoModal(false)
{
}


CWindowWnd::~CWindowWnd(void)
{
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle, int x /*= CW_USEDEFAULT*/, int y /*= CW_USEDEFAULT*/, int cx /*= CW_USEDEFAULT*/, int cy /*= CW_USEDEFAULT*/)
{
	if ( GetSuperClassName() != NULL && !RegisterSuperclass() )
		return NULL;
	if ( GetSuperClassName() == NULL && !RegisterWindowClass())
		return NULL;

#ifdef _DEBUG
    m_dwRunningThread = ::GetCurrentThreadId();
#endif // _DEBUG

	m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), lpszWindowName, dwStyle, x, y, cx, cy,
		hwndParent, NULL, CUIEngine::GetInstance()->GetInstanceHandler(), this);
	DWORD dwLastError = ::GetLastError();
	ASSERT(m_hWnd!=NULL);
	return m_hWnd;
}

bool CWindowWnd::RegisterWindowClass()
{
	WNDCLASS wc = { 0 };
	wc.style = GetClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = CWindowWnd::__WndProc;
	wc.hInstance = CUIEngine::GetInstance()->GetInstanceHandler();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = GetWindowClassName();
	ATOM ret = ::RegisterClass(&wc);
	ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LPCTSTR CWindowWnd::GetWindowClassName() const
{
	return _T("WindowWnd");
}

UINT CWindowWnd::GetClassStyle() const
{
	return NULL;
}

LRESULT CWindowWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::CallWindowProc(m_OldWndProc,m_hWnd,uMsg,wParam,lParam);
}

void CWindowWnd::OnFinalMessage(HWND hWnd)
{

}

LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// ����Ĭ����Ϣ������
	CWindowWnd* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		// ���ô���ʵ��ָ�뵽hWnd������
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		pThis->m_hPaintDC = ::GetDC(hWnd);
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	} 
	else
	{
		// �� WM_NCCREATE ����hWnd�ж�ȡ����ʵ��ָ��
		pThis = reinterpret_cast<CWindowWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
			// �����ǰ��Ϣ�����ٴ���
			::ReleaseDC(hWnd,pThis->m_hPaintDC);
			::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);

			// ��ǰ���������໯�ģ��ָ�ԭ���ڹ���
			if( pThis->m_bSubclassed )
				pThis->Unsubclass();
			pThis->m_hWnd = NULL;

			// ����������ɣ��û����԰�ȫ���ٴ���ʵ��
			pThis->OnFinalMessage(hWnd);
			if ( !pThis->m_bIsDoModal && pThis->m_bIsAutoDelete )
				delete pThis;
			return lRes;
		}
	}

	if( pThis != NULL )
	{
		// ������Ϣ���͵�����ʵ���м�������
		// CWindowWnd::HandleMessage Ϊ�麯��
		// CWindowUI::HandleMessage��дʵ������Ϣ����
		return pThis->WindowProc(uMsg, wParam, lParam);
	} 
	else
	{
		// û���ҵ�����ʵ��ָ�룬����Ĭ�ϴ���
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

HWND CWindowWnd::Subclass(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	ASSERT(m_hWnd==NULL);
	m_OldWndProc = SubclassWindow(hWnd, CWindowWnd::__WndProc);
	if( m_OldWndProc == NULL )
	{
		// ���໯ʧ��
		return NULL;
	}

	m_bSubclassed = true;
	m_hWnd = hWnd;
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
	return m_hWnd;
}

void CWindowWnd::Unsubclass()
{
	ASSERT(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) )
		return;
	if( !m_bSubclassed )
		return;
	SubclassWindow(m_hWnd, m_OldWndProc);
	m_OldWndProc = ::DefWindowProc;
	m_bSubclassed = false;
}

void CWindowWnd::CenterWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWnd=m_hWnd;
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	if (hWndCenter!=NULL)
		hWnd=hWndCenter;

	// �������ʾ��ģʽ����Ļ����
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	rcArea = oMonitor.rcWork;

	if( hWndCenter == NULL )
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if( xLeft < rcArea.left )
		xLeft = rcArea.left;
	else if( xLeft + DlgWidth > rcArea.right )
		xLeft = rcArea.right - DlgWidth;
	if( yTop < rcArea.top )
		yTop = rcArea.top;
	else if( yTop + DlgHeight > rcArea.bottom )
		yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

HWND CWindowWnd::GetHWND() const
{
	return m_hWnd;
}

CWindowWnd::operator HWND() const
{
	return m_hWnd;
}

void CWindowWnd::SetAutoDelete(bool bAutoDelete /*= true*/)
{
	m_bIsAutoDelete = bAutoDelete;
}

void CWindowWnd::ShowWindow(int nCmdShow /*= SW_SHOW*/)
{
	ASSERT(::IsWindow(m_hWnd));
	if ( ::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd,nCmdShow);
	}
}

void CWindowWnd::CloseWindow(UINT nRet /*= IDOK*/)
{
	if (::IsWindow(m_hWnd))
	{
		::PostMessage(m_hWnd,WM_CLOSE,nRet,0);
	}
}

void CWindowWnd::SetSmallIcon(HICON hSmallIcon)
{
	ASSERT(hSmallIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) ICON_SMALL, (LPARAM) hSmallIcon);
}

void CWindowWnd::SetLargeIcon(HICON hLargeIcon)
{
	ASSERT(hLargeIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM) ICON_BIG, (LPARAM) hLargeIcon);
}

UINT CWindowWnd::DoModal()
{
	ASSERT(::IsWindow(m_hWnd));
	UINT nRet = 0;
	HWND hWndParent = GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);
	m_bIsDoModal = true;
	MSG msg = { 0 };
	do 
	{
		if ( !::IsWindow(m_hWnd))
			break;
		if ( ! ::GetMessage(&msg, NULL, 0, 0) )
			break;

		if ( msg.hwnd == m_hWnd && msg.message == WM_CLOSE )
		{
			::EnableWindow(hWndParent,TRUE);		// �ָ�������������Ϣ����
			::SetFocus(hWndParent);
			nRet = msg.wParam;
		}

		if( !::TranslateMessage(&msg) )
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT );

	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);

	if( msg.message == WM_QUIT )
		::PostQuitMessage(msg.wParam);

	if ( m_bIsDoModal && m_bIsAutoDelete)
		delete this;
	return nRet;
}

void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
	ASSERT(::IsWindow(m_hWnd));
	RECT rc = { 0 };
	if( !::GetClientRect(m_hWnd, &rc) ) return;
	if( cx != -1 ) rc.right = cx;
	if( cy != -1 ) rc.bottom = cy;
	if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd),
		(!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)),
		GetWindowExStyle(m_hWnd)) )
		return;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LONG CWindowWnd::ModifyStyle(DWORD dwStyleAdd,DWORD dwStyleRemove /*= 0*/)
{
	LONG dwOldStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	LONG styleValue = dwOldStyle;
	styleValue &= ~( dwStyleRemove );
	styleValue |= (dwStyleAdd);
	LONG lRet = ::SetWindowLong(m_hWnd, GWL_STYLE, styleValue);
	return dwOldStyle;
}

LONG CWindowWnd::ModifyExStyle(DWORD dwExStyleAdd,DWORD dwExStyleRemove /*= 0*/)
{
	LONG dwOldStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	LONG styleValue = dwOldStyle;
	styleValue &= ~( dwExStyleRemove );
	styleValue |= (dwExStyleAdd);
	LONG lRet = ::SetWindowLong(m_hWnd, GWL_EXSTYLE, styleValue);
	return dwOldStyle;
}

void CWindowWnd::EndModal(UINT nRet /*= IDOK*/)
{
	if ( m_bIsDoModal == false)
	{
		// ���棬��ģ̬���ڲ�Ӧ������������رմ���
		ASSERT(false);
		return;
	}

	if ( ::IsWindow(m_hWnd) )
	{
		::PostMessage(m_hWnd,WM_CLOSE,nRet,0);
	}
}

LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd,uMsg,wParam,lParam);
}

LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd,uMsg,wParam,lParam);
}

LPCTSTR CWindowWnd::GetSuperClassName() const
{
	return NULL;
}

bool CWindowWnd::RegisterSuperclass()
{
	// Get the class information from an existing
	// window so we can subclass it later on...
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	if( !::GetClassInfoEx(NULL, GetSuperClassName(), &wc) )
	{
		if( !::GetClassInfoEx(CUIEngine::GetInstance()->GetInstanceHandler(), GetSuperClassName(), &wc) )
		{
			ASSERT(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = CWindowWnd::__ControlProc;
	wc.hInstance = CUIEngine::GetInstance()->GetInstanceHandler();
	wc.lpszClassName = GetWindowClassName();
	ATOM ret = ::RegisterClassEx(&wc);
	ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CWindowWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWindowWnd* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
		::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
		pThis->m_hWnd = hWnd;
	} 
	else
	{
		pThis = reinterpret_cast<CWindowWnd*>(::GetProp(hWnd, _T("WndX")));
		if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			if( pThis->m_bSubclassed )
				pThis->Unsubclass();
			::SetProp(hWnd, _T("WndX"), NULL);
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage(hWnd);
			return lRes;
		}
	}
	if( pThis != NULL )
	{
		return pThis->WindowProc(uMsg, wParam, lParam);
	} 
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

HDC CWindowWnd::GetPaintDC() const
{
	return m_hPaintDC;
}

CWindowWnd::operator HDC() const
{
	return m_hPaintDC;
}

BOOL CWindowWnd::DestroyWindow()
{
	if ( ::IsWindow(m_hWnd))
		return ::DestroyWindow(m_hWnd);

	return FALSE;
}

BOOL CWindowWnd::MoveWindow(int x,int y,int width /*= -1*/, int height /*= -1*/,BOOL bRepaint/*= FALSE*/)
{
	if ( !::IsWindow(m_hWnd))
		return FALSE;

	CDuiRect rcClient;
	::GetClientRect(m_hWnd,&rcClient);

	return ::MoveWindow(m_hWnd,
		x,y, 
		width == -1 ? rcClient.GetWidth() : width ,
		height == -1 ? rcClient.GetHeight() : height,
		bRepaint );
}

