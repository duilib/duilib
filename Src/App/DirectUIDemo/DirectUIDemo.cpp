// DirectUIDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DirectUIDemo.h"
#include "MainWnd.h"

void InitResource();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	CUIEngine * pDirectUIEngine = NULL;
	pDirectUIEngine = CUIEngine::GetInstance();

	pDirectUIEngine->SetInstanceHandler(hInstance);
	pDirectUIEngine->Init();

	InitResource();

	CMainWnd* pMainWnd = new CMainWnd;
	pMainWnd->SetAutoDelete();
	pMainWnd->Init();

	int nRet = pDirectUIEngine->MessageLoop();

	pDirectUIEngine->Uninit();
	pDirectUIEngine = NULL;
	CUIEngine::ReleaseInstance();

	return nRet;
}

void InitResource()
{
	CString strModulePath;
	{
		TCHAR szPath[MAX_PATH] ;
		DWORD dwRet = ::GetModuleFileName(NULL,szPath,MAX_PATH);
		if ( dwRet !=0 && dwRet <MAX_PATH)
		{
			TCHAR *pLastPath = NULL;
			if (pLastPath = _tcsrchr(szPath, '\\'))
				*(pLastPath + 1) = '\0';
			strModulePath = szPath;
		}
	}

	CResourceManager::GetInstance()->InitResouceDir(strModulePath + _T("skin"),true);

	//CResourceManager::GetInstance()->InitResouceDir(strModulePath + _T("Resource\\XXXModule"));
	//CResourceManager::GetInstance()->InitResouceDir(strModulePath + _T("Resource\\YYYModule"));
}

