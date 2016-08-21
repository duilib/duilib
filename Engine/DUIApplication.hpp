#ifndef DUIApplication_h__
#define DUIApplication_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	DUIApplication.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2015-05-06 14:38:32
// 说明			:	DirectUI应用程序核心，简化初始化
/////////////////////////////////////////////////////////////*/
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#pragma comment(lib,"gdiplus")
#include "UIDefine.h"

namespace DuiLib
{
#define APPENV_OLE			0x00000001		// OLE			1
#define APPENV_COM		0x00000002		// COM		2
#define APPENV_SOCK2		0x00000004		// Socket		4
#define APPENV_GDIPLUS		0x00000008		// GDI+	8

class CDUIApplication
{
public:
	CDUIApplication(HINSTANCE hInstance,DWORD dwInitEnv)
		: m_dwEnv(dwInitEnv)
		, m_pUIEngine(NULL)
		, m_dwGDIPlusToken(0)
		, m_hMutex(NULL)
	{
		m_pUIEngine = GetUIEngine();
		m_pUIEngine->Init();
		m_pUIEngine->SetInstanceHandle(hInstance);
	}

	~CDUIApplication(void)
	{
		m_pUIEngine = NULL;
		GetUIEngine()->Uninit();
		if ( m_hMutex != NULL)
		{
			::CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}

		UninitEnv();
	}

	// 返回值为true,说明lpszMutexName所指向的Mutex已经被某个进程打开
	// 说明已经有至少一个进程在运行
	bool IsRunning(LPCTSTR lpszMutexName )
	{
		m_hMutex   =   ::OpenMutex(MUTEX_ALL_ACCESS,TRUE,lpszMutexName); 	
		if(m_hMutex)	
		{
			return true; 		
		}
		return false;
	}

	// 返回值为true，说明指定的窗口已经设置为前台焦点窗口
	// false则为失败或未找到
	bool ShowTargetWindow(LPCTSTR lpszWindowName)
	{
		HWND pWndPrev,pWndChild;
		if (pWndPrev= ::FindWindow(NULL,lpszWindowName))
		{		
			pWndChild = ::GetLastActivePopup(pWndPrev);
			if (::IsIconic(pWndPrev))
				::ShowWindow(pWndPrev,SW_RESTORE);

			::ShowWindow(pWndPrev,SW_SHOW);
			::SetForegroundWindow(pWndChild);
			return true;
		}	
		return false;
	}

	void SetInstanceHandle(HINSTANCE hInstance)
	{
		m_pUIEngine->SetInstanceHandle(hInstance);
	}

	HINSTANCE GetInstanceHandle()
	{
		return m_pUIEngine->GetInstanceHandle();
	}

	int Run(CWindowUI* pMainWindow)
	{
		InitEnv();

		m_pUIEngine->SetKeyWindow(pMainWindow);
		int nRet = m_pUIEngine->MessageLoop();
		m_pUIEngine->SetKeyWindow(NULL);
		return nRet;
	}

	LPCTSTR GetAppDir()
	{
		return m_pUIEngine->GetAppDir();
	}

	LPCTSTR GetWorkDir()
	{
		return m_pUIEngine->GetWorkDir();
	}

	BOOL SetWorkDir(LPCTSTR lpszWorkDir)
	{
		return m_pUIEngine->SetWorkDir(lpszWorkDir);
	}

private:
	void InitEnv()
	{
		if (( m_dwEnv & APPENV_OLE) !=0 )
		{
			::OleInitialize(NULL);
		}
		if (( m_dwEnv & APPENV_COM) !=0 )
		{
			::CoInitialize(NULL);
		}
		if (( m_dwEnv & APPENV_SOCK2) !=0 )
		{
			WSADATA ws={0};
			::WSAStartup(MAKEWORD(2,2),&ws);
		}
		if (( m_dwEnv & APPENV_GDIPLUS) !=0 )
		{
			// 加载GDI接口
			 Gdiplus::GdiplusStartup(&m_dwGDIPlusToken, &m_GDOPlusStartupInput, NULL);
		}
	}

	void UninitEnv()
	{
		if (( m_dwEnv & APPENV_OLE) !=0 )
		{
			::OleUninitialize();
		}
		if (( m_dwEnv & APPENV_COM) !=0 )
		{
			::CoUninitialize();
		}
		if (( m_dwEnv & APPENV_SOCK2) !=0 )
		{
			::WSACleanup();
		}
		if (( m_dwEnv & APPENV_GDIPLUS) !=0 )
		{
			//  卸载GDI接口
			Gdiplus::GdiplusShutdown(m_dwGDIPlusToken);
			m_dwGDIPlusToken = 0;
		}
	}

private:
	DWORD m_dwEnv;
	IUIEngine* m_pUIEngine;
	HANDLE m_hMutex;

	ULONG_PTR								m_dwGDIPlusToken;
	Gdiplus::GdiplusStartupInput		m_GDOPlusStartupInput;
};

}
#endif // DUIApplication_h__
