#ifndef WindowUI_h__
#define WindowUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	WindowUI.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-11-08 16:11:01
// 说明		:	DirectUI Window
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Base/WindowWnd.h"
#include "Define/IUIRender.h"

class DIRECTUI_API CWindowUI
	: public CObjectUI
	, public CWindowWnd
{
public:
	CWindowUI(void);
	virtual ~CWindowUI(void);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	CControlUI *GetRoot() const;
	CControlUI *GetItem(LPCTSTR lpszItemPath) const;

	CControlUI* FindControl(POINT pt) const;
	CControlUI* FindControl(LPCTSTR pstrName) const;

	HWND CreateDuiWindow(HWND hwndParent, LPCTSTR lpszWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);

	void ShowWindow(int nCmdShow = SW_SHOW);
	void CloseWindow(UINT nRet = IDOK);

	UINT ShowModal();
	void EndModal(UINT nRet = IDOK);
	bool IsModal();

	// 最大化、最小化、还原窗口
	void MaximizeWindow();
	void MinimizeWindow();	
	void RestoreWindow();
	bool IsMaximized();
	bool IsMinimized();

	SIZE GetInitSize();
	void SetInitSize(int cx, int cy);
	RECT GetSizeBox();
	void SetSizeBox(RECT& rcSizeBox);
	RECT GetCaptionRect();
	void SetCaptionRect(RECT& rcCaption);

	CControlUI* GetFocus() const;
	void SetFocus(CControlUI* pControl);
	void SetFocusNeeded(CControlUI* pControl);

	// 设置和获取窗口Capture
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();

	 void NeedUpdate();
	 void Invalidate(RECT& rcItem);

	void SetWindowTitle(LPCTSTR lpszWindowTitle);
	LPCTSTR	GetWindowTitle();

	bool SetNextTabControl(bool bForward = true);

	// Contorl Event Filter
	void AddNotify(INotifyUI *pNotify);
	void RemoveNotify(INotifyUI *pNotify);

	void SendNotify(TNotifyUI *pMsg, bool bAsync = false);
	void SendNotify(CControlUI* pControl, DWORD dwType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
	void AddDelayedCleanup(CControlUI* pControl);
	void ReapObjects(CControlUI* pControl);

	bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
	bool KillTimer(CControlUI* pControl, UINT nTimerID);
	void KillTimer(CControlUI* pControl);
	void RemoveAllTimers();

public:
	// Window Message Filter
	void AddMessageFilter(IMessageFilterUI* pFilter);
	void RemoveMessageFilter(IMessageFilterUI* pFilter);

public:
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	// 优先过滤消息
	virtual bool PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);
	// 处理加速键消息
	virtual bool TranslateAccelerator(MSG *pMsg);

	LRESULT ReflectNotifications(
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Inout_ bool& bHandled);

private:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UINT GetClassStyle() const;

private:
	HINSTANCE m_hInstance;
	CControlUI *m_pRootControl;
	CControlUI *m_pEventClick;
	CControlUI *m_pEventHover;
	CControlUI *m_pFocus;
	CControlUI *m_pEventKey;

	bool m_bIsModal;
	bool m_bIsMaximized;
	bool m_bIsMinimized;
	CDuiRect m_rcRestore;

	CDuiSize m_szRoundCorner;		
	SIZE m_szMinWindow;				// 窗口最小
	SIZE m_szMaxWindow;				// 窗口最大
	CDuiSize m_szInitWindowSize;	// 初始化窗口大小
	CDuiRect m_rcSizeBox;				// 可调整边框边距
	CDuiRect m_rcCaption;				// 响应标题栏拖动

	bool m_bShowUpdateRect;
	BYTE m_nAlpha;
	bool m_bLayedWindow;

	CDuiString m_strWindowTitle;


	bool m_bFirstLayout;
	bool m_bUpdateNeeded;
	bool m_bFocusNeeded;
	CMemDC m_OffscreenDC;
	IUIRender *m_pRenderEngine;

	// Tooltip
	HWND m_hWndTooltip;
	TOOLINFO m_ToolTip;

	// Events
	POINT m_ptLastMousePos;
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	CStdPtrArray m_arrayDelayedCleanup;
	CStdPtrArray m_arrayAsyncNotify;
	CStdPtrArray m_arrayNotifyFilters;
	CStdPtrArray m_arrayPreMessageFilters;
	CStdPtrArray m_arrayTranslateAccelerators;
	CStdPtrArray m_arrayTimers;
	UINT m_uTimerID;

	CStdStringPtrMap m_mapNameHash;

private:
	static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromUpdate(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
	static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);
};

#endif // WindowUI_h__
