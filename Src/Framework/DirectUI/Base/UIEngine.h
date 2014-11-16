#ifndef UIEngine_h__
#define UIEngine_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	DirectUIEngine.h
// 创建人	: 	daviyang35@gmail.com
// 创建时间	:	2014-11-08 13:56:14
// 说明		:	DirectUI库核心管理引擎
DirectUI Engine Core Manager
/////////////////////////////////////////////////////////////*/
#pragma once
#include <windows.h>

class CWindowUI;
class DIRECTUI_API CUIEngine
{
public:
	// Singleton instace 
	// not thread safety
	static CUIEngine* GetInstance();
	static void ReleaseInstance();

	// Init Engine and Global Resource
	void Init();
	// Free Engine and Global Resource
	void Uninit();

	void InitCOM();
	void InitCOMEx(DWORD dwCoInit);
	void InitOLE();

public:
	CResourceManager* GetResourceManager();

	HINSTANCE GetInstanceHandler();
	void SetInstanceHandler(HINSTANCE hInstance);

	int MessageLoop();
	bool TranslateMessage(const LPMSG pMsg);
	bool PumpMessage();

	void SkinWindow(CWindowUI* pWindow);
	void UnSkinWindow(CWindowUI* pWindow);
	void UnSkinWindow(HWND hWnd);

public:
	virtual void RegisterControl(LPCTSTR lpszType, PROCCONTROLCREATE pFn_ControlCreate);
	CControlUI* CreateControl(LPCTSTR lpszType);

private:
	CUIEngine(void);
	~CUIEngine(void);
	CUIEngine(const CUIEngine* rhs);
	static CUIEngine* m_pUIEngineInstance;

private:
	HINSTANCE m_hInstace;
	bool m_bInitedCOM;
	bool m_bInitedOLE;
	CResourceManager * m_pResourceManager;
	typedef std::map<CDuiString, PROCCONTROLCREATE> ProcControlCreateMap;
	ProcControlCreateMap m_ControlCreateMap;

	CStdPtrArray m_arrayDirectUI;
};

#endif // UIEngine_h__
