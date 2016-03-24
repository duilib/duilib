#ifndef UIEngine_h__
#define UIEngine_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	DirectUIEngine.h
// ������	: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-08 13:56:14
// ˵��		:	DirectUI����Ĺ�������
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

	HFONT GetFont(LPCTSTR lpszFontName);
	FontObject *GetFontObject(LPCTSTR lpszFontName);

public:
	CResourceManager* GetResourceManager();

	HINSTANCE GetInstanceHandler();
	void SetInstanceHandler(HINSTANCE hInstance);

	int MessageLoop();
	bool TranslateMessage(const LPMSG pMsg);
	bool PumpMessage();

	// ע��/��ע���Ի洰��
	void SkinWindow(CWindowUI* pWindow);
	void UnSkinWindow(CWindowUI* pWindow);

	// ���ݴ��ھ�������Ի洰��
	CWindowUI* GetWindow(HWND hWnd);
	// ���ݴ������Ʋ����Ի洰��
	CWindowUI* GetWindow(LPCTSTR lpszName);

public:
	virtual void RegisterControl(LPCTSTR lpszType, PROCCONTROLCREATE pFn_ControlCreate);
	CControlUI* CreateControl(LPCTSTR lpszType);
	void UnregisterControl(LPCTSTR lpszType);

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
