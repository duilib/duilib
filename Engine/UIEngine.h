#ifndef UIEngine_h__
#define UIEngine_h__
#pragma once

namespace DuiLib
{

class CWindowUI;
class CWindowWnd;
//////////////////////////////////////////////////////////////////////////
// CUIEngine
class UILIB_API CUIEngine
	: public IUIEngine
{
public:
	CUIEngine();
	~CUIEngine();
	static CUIEngine* s_pUIEngine;

public:
	static CUIEngine* GetInstance();

public:
	virtual DWORD GetEngineVersion();

	// 1、初始化界面库
	virtual void Init();
	// 2、创建窗口，
	//new CWindowUI()
	// 3、消息循环
	virtual int  MessageLoop();
	// 4、销毁界面库
	virtual void Uninit();

	// 设置进程资源句柄
	virtual void SetInstanceHandle(HINSTANCE hInstance);
	virtual HINSTANCE GetInstanceHandle();

	virtual HWND GetPumpWnd();

	// 消息泵接口
	virtual bool TranslateMessage(const LPMSG pMsg);
	virtual bool PumpMessage();

	// 注册/反注册自绘窗口
	void SkinWindow(CWindowUI* pWindow);
	void UnSkinWindow(CWindowUI* pWindow);

	// 空闲处理接口
	virtual void AddIdleFilter(IIdleFilter* pFilter);
	virtual void RemoveIdleFilter(IIdleFilter* pFilter);

	// 自绘窗口查找
	virtual int GetWindowCount();
	// 根据窗口句柄查找自绘窗口
	virtual CWindowUI* GetWindow(HWND hWnd);
	// 根据窗口名称查找自绘窗口
	virtual CWindowUI* GetWindow(LPCTSTR lpszName);
	// 根据窗口索引查找自绘窗口
	virtual CWindowUI* GetWindow(UINT nIndex);
	// 获取当前焦点窗口
	virtual CWindowUI* GetKeyWindow();
	// 设置焦点窗口
	virtual CWindowUI* SetKeyWindow(CWindowUI* pKeyWindow);

	// 控件注册与创建接口
	virtual void RegisterControl(LPCTSTR lpszType, PROCCONTROLCREATE pFn_ControlCreate);
	virtual void UnregisterControl(LPCTSTR lpszType);
	virtual CControlUI* CreateControl(LPCTSTR lpszType);

	virtual LPCTSTR GetAppDir();
	virtual LPCTSTR GetWorkDir();
	virtual BOOL SetWorkDir(LPCTSTR lpszWorkDir);

private:
	BOOL IsIdleMessage( MSG* pMsg );

private:
	HINSTANCE m_hInstance;
	typedef std::map<CDuiString, PROCCONTROLCREATE> ProcControlCreateMap;
	ProcControlCreateMap m_ControlCreateMap;
	CStdPtrArray m_arrayDirectUI;
	CStdPtrArray m_arrayIdleFilters;
	CDuiString m_strAppDir;
	CWindowUI* m_pKeyWindow;
	CBufferPtr m_strWorkDir;
	CWindowWnd* m_pPumpWnd;
};

}


#endif // UIEngine_h__
