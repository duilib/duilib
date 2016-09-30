#pragma once

class CMainWnd : public CWindowWnd, public INotifyUI,public IDialogBuilderCallback
{
public:
	CMainWnd();
	~CMainWnd();

	LPCTSTR GetWindowClassName() const { return _T("CMainWnd"); }
	UINT GetClassStyle() const { return CS_DBLCLKS; };	//²Î¿¼360DEMO
	void OnFinalMessage(HWND /*hWnd*/) { /*delete this;*/ };
	void Notify(TNotifyUI& msg);

	CControlUI* CreateControl(LPCTSTR pstrClass);

public:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
public:
	void InitDlg();

public:
	CPaintManagerUI m_PM;
	
	CButtonUI* m_pTestBtn;
};