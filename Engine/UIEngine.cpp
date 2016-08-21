#include "stdafx.h"
#include "UIEngine.h"
#include "Core/WindowWnd.h"

#ifndef WM_SYSTIMER
#define WM_SYSTIMER 0x0118   //(caret blink)
#endif//WM_SYSTIMER

namespace DuiLib
{

EXTERN_C
{
    IUIEngine * GetUIEngine()
    {
		if ( CUIEngine::s_pUIEngine == NULL)
		{
			CUIEngine::s_pUIEngine = new CUIEngine();
		}
        return CUIEngine::s_pUIEngine;
    }

	UILIB_API CControlUI* CreateControl(LPCTSTR lpszControlName)
	{
		return GetUIEngine()->CreateControl(lpszControlName);
	}

};

//////////////////////////////////////////////////////////////////////////
// CUIEngineEx

CUIEngine::CUIEngine()
	: m_hInstance(NULL)
	, m_pKeyWindow(NULL)
	, m_strWorkDir(MAX_PATH)
	, m_pPumpWnd(NULL)
{

}

CUIEngine::~CUIEngine()
{

}

CUIEngine* CUIEngine::s_pUIEngine =NULL;

DWORD CUIEngine::GetEngineVersion()
{
	return 0x0100;		// 01 00
}

void CUIEngine::Init()
{
	::InitCommonControls();						// 注册系统控件
	::LoadLibrary(_T("msimg32.dll"));       // 依赖AlphaBlend

	// 启动Duilib公用的消息窗口
	m_pPumpWnd = new CWindowWnd;
	m_pPumpWnd->SetAutoDelete(true);
	m_pPumpWnd->Create(HWND_MESSAGE,_T("PumpWnd"),WS_POPUP,0);

	// 初始化资源管理器
	AddIdleFilter(CResEngine::GetInstance());

	// 动画管理器初始化
	CActionManager::GetInstance();

	// 注册Duilib基础控件
	// 基础控件
	UI_REGISTER_DYNCREATE(_T("Control"),CControlUI);
	UI_REGISTER_DYNCREATE(_T("Container"),CContainerUI);
	UI_REGISTER_DYNCREATE(_T("ScrollBar"),CScrollBarUI);
	UI_REGISTER_DYNCREATE(_T("Slider"),CSliderUI);

	// 布局控件
	UI_REGISTER_DYNCREATE(_T("HorizontalLayout"),CHorizontalLayoutUI);
	UI_REGISTER_DYNCREATE(_T("VerticalLayout"),CVerticalLayoutUI);
	UI_REGISTER_DYNCREATE(_T("TabLayout"),CTabLayoutUI);
	UI_REGISTER_DYNCREATE(_T("TileLayout"),CTileLayoutUI);

	UI_REGISTER_DYNCREATE(_T("ChildLayout"),CChildLayoutUI);
	UI_REGISTER_DYNCREATE(_T("CompositeLayout"),CCompositeLayoutUI);

	// 基本控件
	UI_REGISTER_DYNCREATE(_T("Label"),CLabelUI);
	UI_REGISTER_DYNCREATE(_T("Button"),CButtonUI);
	UI_REGISTER_DYNCREATE(_T("Edit"),CEditUI);
	UI_REGISTER_DYNCREATE(_T("Option"),COptionUI);
	UI_REGISTER_DYNCREATE(_T("Progress"),CProgressUI);
	UI_REGISTER_DYNCREATE(_T("List"),CListUI);
	UI_REGISTER_DYNCREATE(_T("Combo"),CComboUI);

	UI_REGISTER_DYNCREATE(_T("RichEdit"),CRichEditUI);
	UI_REGISTER_DYNCREATE(_T("IPAddress"),CIPAddressUI);
	UI_REGISTER_DYNCREATE(_T("ActiveX"),CActiveXUI);

	UI_REGISTER_DYNCREATE(_T("CheckBox"),CCheckBoxUI);

	// List扩展控件
	UI_REGISTER_DYNCREATE(_T("ListHeader"),CListHeaderUI);
	UI_REGISTER_DYNCREATE(_T("ListHeaderItem"),CListHeaderItemUI);
	UI_REGISTER_DYNCREATE(_T("ListLabelElement"),CListLabelElementUI);
	UI_REGISTER_DYNCREATE(_T("ListTextElement"),CListTextElementUI);
	UI_REGISTER_DYNCREATE(_T("ListContainerElement"),CListContainerElementUI);

	UI_REGISTER_DYNCREATE(_T("TreeView"),CTreeViewUI);
	UI_REGISTER_DYNCREATE(_T("TreeNode"),CTreeNodeUI);

	// ActiveX扩展控件
	UI_REGISTER_DYNCREATE(_T("WebBrowser"),CWebBrowserUI);
	UI_REGISTER_DYNCREATE(_T("Flash"),CFlashUI);

	// 拓展TabLayout，切换动画
	UI_REGISTER_DYNCREATE(_T("SliderTabLayout"),CSliderTabLayoutUI);
	// 扩展动画空间，播放序列帧
	UI_REGISTER_DYNCREATE(_T("AnimationControl"),CAnimationControlUI);

	SetWorkDir(GetAppDir());
}

int CUIEngine::MessageLoop()
{
	BOOL bDoIdle = TRUE;
	int nIdleCount = 0;
	BOOL bRet;
	int nFiltersCount =0;
	MSG msg = { 0 };

	for(;;)
	{
		while(bDoIdle && !::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			++nIdleCount;
			nFiltersCount=m_arrayIdleFilters.GetSize();
			for(int i = 0; i < nFiltersCount; ++i)
			{
				IIdleFilter* pIdleHandler = static_cast<IIdleFilter*>(m_arrayIdleFilters.GetAt(i));
				if(pIdleHandler != NULL)
					pIdleHandler->OnIdle();
			}
		}

		bRet = ::GetMessage(&msg, NULL, 0, 0);

		if(bRet == -1)
		{
			DUI__Trace(_T("::GetMessage returned -1 (error)"));
			continue;   // error, don't process
		}
		else if(!bRet)
		{
			DUI__Trace(_T("SMessageLoop::Run - exiting"));
			break;   // WM_QUIT, exit message loop
		}

		if(!this->TranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if(IsIdleMessage(&msg))
		{
			bDoIdle = TRUE;
			nIdleCount = 0;
		}
	}

	return (int)msg.wParam;
}

void CUIEngine::Uninit()
{
	// 检查自绘窗口列表，如果还存在窗口，则需要全部删除
	std::vector<HWND> vecWindows;
	// 因为SkinWindows接口会在销毁窗口时变化，所以先压入所有HWND
	// 然后通过HWND实例获得窗口指针销毁窗口
	int nWindowCount = GetUIEngine()->GetWindowCount();
	for ( int i=0;i<nWindowCount;++i)
	{
		CWindowUI* pWindow = GetUIEngine()->GetWindow(i);
		vecWindows.push_back(pWindow->GetHWND());
	}

	for ( int i=0;i<nWindowCount;++i)
	{
		CWindowUI* pWindow = GetUIEngine()->GetWindow(vecWindows[i]);
		if ( pWindow && pWindow->IsWindow())
		{
			pWindow->DestroyWindow();
		}
	}

	CActionManager::ReleaseInstance();
	CResEngine::ReleaseInstance();
	m_pPumpWnd->DestroyWindow();
	m_pPumpWnd = NULL;

	if ( CUIEngine::s_pUIEngine != NULL)
	{
		delete CUIEngine::s_pUIEngine;
		CUIEngine::s_pUIEngine = NULL;
	}
}

void CUIEngine::SetInstanceHandle(HINSTANCE hInstance)
{
	m_hInstance = m_hInstance;
}

HINSTANCE CUIEngine::GetInstanceHandle()
{
	return m_hInstance;
}

HWND CUIEngine::GetPumpWnd()
{
	return m_pPumpWnd->GetHWND();
}

bool CUIEngine::TranslateMessage(const LPMSG pMsg)
{
	if(!::IsWindow(pMsg->hwnd))
	{
		return false;
	}

	// pMsg->hwnd 如果在 m_arrayDirectUI 中
	// 说明这个窗口是自绘窗口，分发消息处理
	int nCount = m_arrayDirectUI.GetSize();
	LRESULT lRes = S_OK;
	UINT uStyle = GetWindowStyle(pMsg->hwnd);
	bool bIsChildWindow = (uStyle & WS_CHILD) != 0;

	if(bIsChildWindow)
	{
		HWND hWndParent = ::GetParent(pMsg->hwnd);

		for(int i = m_arrayDirectUI.GetSize() - 1 ; i >= 0 ; --i)
		{
			CWindowUI *pT = static_cast<CWindowUI *>(m_arrayDirectUI[i]);
			HWND hTempParent = hWndParent;

			while(hTempParent)
			{
				if(pMsg->hwnd == pT->GetHWND() || hTempParent == pT->GetHWND())
				{
					if(pT->TranslateAccelerator(pMsg))
					{
						return true;
					}

					pT->PreMessageHandler(pMsg, lRes);
				}

				hTempParent = GetParent(hTempParent);
			}
		}

		return false;
	}

	// 将消息交给窗口实例，使窗口实例有机会过滤消息或处理加速键

	for(int i = 0; i < nCount; ++i)
	{
		CWindowUI *pWindow = static_cast<CWindowUI *>(m_arrayDirectUI.GetAt(i));

		if(pMsg->hwnd != pWindow->GetHWND())
		{
			continue;
		}

		if(pWindow->TranslateAccelerator(pMsg))
		{
			return true;
		}

		if(pWindow->PreMessageHandler(pMsg, lRes))
		{
			return true;
		}
	}

	return false;
}

bool CUIEngine::PumpMessage()
{
	MSG msg = { 0 };

	if(::GetMessage(&msg, 0, 0, 0))
	{
		if(!this->TranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		return true;
	}

	return false;
}

void CUIEngine::SkinWindow(CWindowUI* pWindow)
{
	m_arrayDirectUI.Add(pWindow);
}

void CUIEngine::UnSkinWindow(CWindowUI* pWindow)
{
	m_arrayDirectUI.Remove(m_arrayDirectUI.Find(pWindow));
}

void CUIEngine::AddIdleFilter(IIdleFilter* pFilter)
{
	m_arrayIdleFilters.Add(pFilter);
}

void CUIEngine::RemoveIdleFilter(IIdleFilter* pFilter)
{
	m_arrayIdleFilters.Remove(m_arrayIdleFilters.Find(pFilter));
}

int CUIEngine::GetWindowCount()
{
	return m_arrayDirectUI.GetSize();
}

CWindowUI* CUIEngine::GetWindow(HWND hWnd)
{
	int count = m_arrayDirectUI.GetSize();

	for(int i = 0; i < count; ++i)
	{
		CWindowUI *pWindow = static_cast<CWindowUI *>(m_arrayDirectUI.GetAt(i));
		HWND hWindow = pWindow->GetHWND();

		if(::IsWindow(hWindow) && hWnd == hWindow)
		{
			return pWindow;
		}
	}

	return NULL;
}

CWindowUI* CUIEngine::GetWindow(LPCTSTR lpszName)
{
	int count = m_arrayDirectUI.GetSize();

	for(int i = 0; i < count; ++i)
	{
		CWindowUI *pWindow = static_cast<CWindowUI *>(m_arrayDirectUI.GetAt(i));

		if(::IsWindow(pWindow->GetHWND()))
		{
			if(_tcscmp(pWindow->GetName(), lpszName) == 0)
			{
				return pWindow;
			}
		}
	}

	return NULL;
}

CWindowUI* CUIEngine::GetWindow(UINT nIndex)
{
	CWindowUI *pWindow = static_cast<CWindowUI *>(m_arrayDirectUI.GetAt(nIndex));
	return pWindow;
}

CWindowUI* CUIEngine::GetKeyWindow()
{
	return m_pKeyWindow;
}

CWindowUI* CUIEngine::SetKeyWindow(CWindowUI* pKeyWindow)
{
	CWindowUI* pPrevKeyWindow = m_pKeyWindow;
	m_pKeyWindow = pKeyWindow;
	return pPrevKeyWindow;
}

void CUIEngine::RegisterControl(LPCTSTR lpszType, PROCCONTROLCREATE pFn_ControlCreate)
{
	if ( pFn_ControlCreate == NULL)
		return ;

	ProcControlCreateMap::iterator iter = m_ControlCreateMap.find(lpszType);
	if ( iter == m_ControlCreateMap.end() )
	{
		m_ControlCreateMap[lpszType] = pFn_ControlCreate;
	}
#ifdef _DEBUG
	else
	{
		// Exist Control Type Name

		DUI__Trace(_T("该控件已存在：%s"),iter->first.c_str());
	}
#endif
}

void CUIEngine::UnregisterControl(LPCTSTR lpszType)
{
	ProcControlCreateMap::iterator iter = m_ControlCreateMap.find(lpszType);
	if ( iter != m_ControlCreateMap.end())
	{
		m_ControlCreateMap.erase(iter);
	}
}

CControlUI* CUIEngine::CreateControl(LPCTSTR lpszType)
{
	ProcControlCreateMap::iterator iter = m_ControlCreateMap.find(lpszType);
	if ( iter != m_ControlCreateMap.end())
	{
		PROCCONTROLCREATE pFn_ControlCreate = iter->second;
		return pFn_ControlCreate();
	}
	return NULL;
}

LPCTSTR CUIEngine::GetAppDir()
{
	if ( m_strAppDir.empty() )
	{
		TCHAR szFullPath[MAX_PATH] = {0};
		::GetModuleFileName(NULL,szFullPath,MAX_PATH);
		(_tcsrchr(szFullPath, _T('\\')))[0] = 0;
		m_strAppDir = szFullPath;
	}

	return m_strAppDir.c_str();
}

LPCTSTR CUIEngine::GetWorkDir()
{
	DWORD dwLength = ::GetCurrentDirectory(0,NULL);
	dwLength = ::GetCurrentDirectory(m_strWorkDir.Size(),(TCHAR*)m_strWorkDir.Ptr());
	return (LPCTSTR)m_strWorkDir.Ptr();
}

BOOL CUIEngine::SetWorkDir(LPCTSTR lpszWorkDir)
{
	return ::SetCurrentDirectory(lpszWorkDir);
}

BOOL CUIEngine::IsIdleMessage(MSG* pMsg)
{
	// These messages should NOT cause idle processing
	switch(pMsg->message)
	{
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
	case WM_PAINT:
	case WM_SYSTIMER:	// WM_SYSTIMER (caret blink)
		return FALSE;
	}

	return TRUE;
}

}
