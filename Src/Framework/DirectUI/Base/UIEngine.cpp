#include "stdafx.h"
#include "UIEngine.h"
#include "Base/ResourceManager.h"

CUIEngine* CUIEngine::m_pUIEngineInstance = NULL;

CUIEngine::CUIEngine(void)
	: m_hInstace(NULL)
	, m_bInitedCOM(false)
	, m_bInitedOLE(false)
	, m_pResourceManager(NULL)
{
}

CUIEngine::CUIEngine(const CUIEngine* rhs)
{

}


CUIEngine::~CUIEngine(void)
{
}

CUIEngine* CUIEngine::GetInstance()
{
	if ( m_pUIEngineInstance == NULL)
	{
		m_pUIEngineInstance = new CUIEngine;
	}
	return m_pUIEngineInstance;
}

void CUIEngine::ReleaseInstance()
{
	if ( m_pUIEngineInstance != NULL)
	{
		delete m_pUIEngineInstance;
		m_pUIEngineInstance = NULL;
	}
}

void CUIEngine::Init()
{
	// 注册所有系统基础控件
	::InitCommonControls();
	//::LoadLibrary(_T("msimg32.dll"));
	// COM和OLE
	this->InitCOM();
	this->InitOLE();

	// 其他单例启动，销毁Engine时删除，确保生命周期可控
	m_pResourceManager = CResourceManager::GetInstance();
	m_pResourceManager->SetDefaultFont(_T("微软雅黑"));
	CUIPaint::GetInstance();

	// 注册提供的基础控件
	// bIsActive为true，控件区域不响应窗口拖动事件
	UI_REGISTER_DYNCREATE(_T("Control"),CControlUI,false);
	UI_REGISTER_DYNCREATE(_T("Container"),CContainerUI,false);
	UI_REGISTER_DYNCREATE(_T("ScrollBar"),CScrollBarUI,true);
	UI_REGISTER_DYNCREATE(_T("Button"),CButtonUI,true);
	UI_REGISTER_DYNCREATE(_T("HorizontalLayout"),CHorizontalLayoutUI,false);
	UI_REGISTER_DYNCREATE(_T("VerticalLayout"),CVerticalLayoutUI,false);
	UI_REGISTER_DYNCREATE(_T("ChildLayout"),CChildLayoutUI,false);
	UI_REGISTER_DYNCREATE(_T("TabLayout"),CTabLayoutUI,false);

	UI_REGISTER_DYNCREATE(_T("Label"),CLabelUI,false);
	UI_REGISTER_DYNCREATE(_T("Edit"),CEditUI,true);
	UI_REGISTER_DYNCREATE(_T("RichEdit"),CRichEditUI,true);

}

void CUIEngine::Uninit()
{
	CUIPaint::ReleaseInstance();
	m_pResourceManager = NULL;
	CResourceManager::ReleaseInstance();

	m_ControlCreateMap.clear();
	m_arrayDirectUI.Empty();

	if ( m_bInitedCOM )
	{
		::CoUninitialize();
		m_bInitedCOM = false;
	}
	if ( m_bInitedOLE)
	{
		::OleUninitialize();
		m_bInitedOLE = false;
	}
}

void CUIEngine::SetInstanceHandler(HINSTANCE hInstance)
{
	m_hInstace = hInstance;
}

HINSTANCE CUIEngine::GetInstanceHandler()
{
	return m_hInstace;
}

int CUIEngine::MessageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) )
	{
		if( !this->TranslateMessage(&msg) )
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return msg.lParam;
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
	bool bIsChildWindow = (uStyle & WS_CHILD ) != 0;
	if ( bIsChildWindow )
	{
		HWND hWndParent = ::GetParent(pMsg->hwnd);

		for( int i = m_arrayDirectUI.GetSize() -1 ; i >=0 ; --i ) 
		{
			CWindowUI* pT = static_cast<CWindowUI*>(m_arrayDirectUI[i]);        
			HWND hTempParent = hWndParent;
			while(hTempParent)
			{
				if(pMsg->hwnd == pT->GetHWND() || hTempParent == pT->GetHWND())
				{
					if (pT->TranslateAccelerator(pMsg))
						return true;
					// by Redrain WebBrowser Tab
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
	if ( ::GetMessage(&msg,0,0,0) )
	{
		if ( !this->TranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		return true;
	}
	return false;
}

void CUIEngine::InitCOM()
{
	if ( !m_bInitedCOM )
	{
		::CoInitialize(NULL);
		m_bInitedCOM = true;
	}
}

void CUIEngine::InitCOMEx(DWORD dwCoInit)
{
	if ( !m_bInitedCOM )
	{
		::CoInitializeEx(NULL,dwCoInit);
		m_bInitedCOM = true;
	}
}

void CUIEngine::InitOLE()
{
	if ( !m_bInitedOLE )
	{
		::OleInitialize(NULL);
		m_bInitedOLE = true;
	}
}

void CUIEngine::RegisterControl(LPCTSTR lpszType, PROCCONTROLCREATE pFn_ControlCreate,bool bActive/*=false*/)
{
	if ( pFn_ControlCreate == NULL)
		return ;

	ProcControlCreateMap::iterator iter = m_ControlCreateMap.find(lpszType);
	if ( iter == m_ControlCreateMap.end() )
	{
		m_ControlCreateMap[lpszType] = pFn_ControlCreate;
		if ( bActive )
		m_vecActiveControl.push_back(lpszType);
	}
	else
	{
		// Exist Control Type Name
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

void CUIEngine::UnregisterControl(LPCTSTR lpszType)
{
	{
		ProcControlCreateMap::iterator iter = m_ControlCreateMap.find(lpszType);
		if ( iter != m_ControlCreateMap.end())
		{
			m_ControlCreateMap.erase(iter);
		}
	}

	{
		VecString::iterator iter = m_vecActiveControl.begin();
		VecString::iterator end = m_vecActiveControl.end();
		while (iter != end)
		{
			if ( CDuiStringOperation::compareNoCase(lpszType,iter->c_str()) == 0 )
			{
				m_vecActiveControl.erase(iter);
				break;
			}
			++iter;
		}
	}
}

void CUIEngine::SkinWindow(CWindowUI* pWindow)
{
	m_arrayDirectUI.Add(pWindow);
}

void CUIEngine::UnSkinWindow(CWindowUI* pWindow)
{
	m_arrayDirectUI.Remove(m_arrayDirectUI.Find(pWindow));
}

CResourceManager* CUIEngine::GetResourceManager()
{
	return m_pResourceManager;
}

CWindowUI* CUIEngine::GetWindow(HWND hWnd)
{
	int count = m_arrayDirectUI.GetSize();
	for (int i = 0; i < count; ++i)
	{
		CWindowUI* pWindow = static_cast<CWindowUI*>(m_arrayDirectUI.GetAt(i));
		HWND hWindow = pWindow->GetHWND();
		if ( ::IsWindow(hWindow) && hWnd == hWindow)
		{
			return pWindow;
		}
	}
	return NULL;
}

CWindowUI* CUIEngine::GetWindow(LPCTSTR lpszName)
{
	int count = m_arrayDirectUI.GetSize();
	for (int i = 0; i < count; ++i)
	{
		CWindowUI* pWindow = static_cast<CWindowUI*>(m_arrayDirectUI.GetAt(i));
		if ( ::IsWindow(pWindow->GetHWND()) )
		{
			if ( _tcscmp(pWindow->GetName(),lpszName) == 0)
				return pWindow;
		}
	}
	return NULL;
}

bool CUIEngine::IsActiveControl(LPCTSTR lpszClass)
{
	VecString::iterator iter = m_vecActiveControl.begin();
	VecString::iterator end = m_vecActiveControl.end();
	while (iter != end)
	{
		if ( CDuiStringOperation::compareNoCase(lpszClass,iter->c_str()) == 0 )
		{
			return true;
		}
		++iter;
	}
	return false;
}

HFONT CUIEngine::GetFont(LPCTSTR lpszFontName)
{
	FontObject *pFontObject = m_pResourceManager->GetFont(lpszFontName);
	return pFontObject->GetFont();
}

FontObject * CUIEngine::GetFontObject(LPCTSTR lpszFontName)
{
	return m_pResourceManager->GetFont(lpszFontName);
}

