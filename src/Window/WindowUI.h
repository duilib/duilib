#ifndef WindowUI_h__
#define WindowUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	WindowUI.h
// ������	: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-08 16:11:01
// ˵��		:	DirectUI Window
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Base/WindowWnd.h"
#include "Define/IUIRender.h"

class DIRECTUI_API CWindowUI
	: public CObjectUI
	, public CWindowWnd
	, public INotifyUI
{
	friend class CDialogBuilder;
public:
	CWindowUI(void);
	virtual ~CWindowUI(void);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	CControlUI *GetRoot() const;
	CControlUI *GetItem(LPCTSTR lpszItemPath) const;

	CControlUI* FindControl(POINT pt) const;
	CControlUI* FindControl(LPCTSTR pstrName) const;
	CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;

	virtual bool Notify(TNotifyUI *pMsg);

	HWND CreateDuiWindow(HWND hwndParent, LPCTSTR lpszWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);

	virtual UINT DoModal();
	bool IsModal();

	// ��󻯡���С������ԭ����
	void MaximizeWindow();
	void MinimizeWindow();	
	void RestoreWindow();
	bool IsMaximized();
	bool IsMinimized();

	POINT GetMousePos() const;

	// �������Խӿ�
	SIZE GetInitSize();
	void SetInitSize(int cx, int cy);
	RECT GetSizeBox();
	void SetSizeBox(RECT& rcSizeBox);
	RECT GetCaptionRect();
	void SetCaptionRect(RECT& rcCaption);
	void SetShadowEnable(bool bEnable = true);

	// ����
	CControlUI* GetFocus() const;
	void SetFocus(CControlUI* pControl);
	void SetFocusNeeded(CControlUI* pControl);

	// ���úͻ�ȡ����Capture
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();

	// ˢ��
	void NeedUpdate();
	void Invalidate(RECT& rcItem);

	// title
	void SetWindowTitle(LPCTSTR lpszWindowTitle);
	LPCTSTR	GetWindowTitle();

	bool SetNextTabControl(bool bForward = true);

	// ���ڿؼ���Ϣ֪ͨ�ӿ�
	void AddNotify(INotifyUI *pNotify);
	void RemoveNotify(INotifyUI *pNotify);

	void SendNotify(TNotifyUI *pMsg, bool bAsync = false);
	void SendNotify(CControlUI* pControl, UINOTIFY dwType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	// ����Ĭ������
	void SetDefaultFont(LPCTSTR lpszFaceName,int nSize = 12, bool bBold = false, bool bUnderline= false, bool bItalic= false ,bool bStrikeout= false);
	FontObject* GetDefaultFont(void);

	// �ӿؼ��������
	bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
	void AddDelayedCleanup(CControlUI* pControl);
	void ReapObjects(CControlUI* pControl);

	bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
	bool KillTimer(CControlUI* pControl, UINT nTimerID);
	void KillTimer(CControlUI* pControl);
	void RemoveAllTimers();

	HDC GetPaintDC();

	int GetPostPaintCount() const;
	bool AddPostPaint(CControlUI* pControl);
	bool RemovePostPaint(CControlUI* pControl);
	bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

	bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
	void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	void RemoveAllOptionGroups();

	TEXTMETRIC GetTM(HFONT hFont);

public:
	// ��ǰ���ڵ���Ϣ����
	void AddWindowMessageFilter(IMessageFilterUI* pFilter);
	void RemoveWindowMessageFilter(IMessageFilterUI* pFilter);

	// ȫ����Ϣ�Ĺ���
	void AddPreMessageFilter(IMessageFilterUI* pFilter);
	void RemovePreMessageFilter(IMessageFilterUI* pFilter);

	// ���ټ���ϢԤ����
	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);

public:
	// ʵ���Ի洰�ڵĴ�����Ϣ����
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	// ��д��CWindowWnd������Ϣѭ��
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual LRESULT CustomMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual void SendNotifyEvent(TNotifyUI *pMsg);

public:
	// ���ȹ�����Ϣ
	virtual bool PreMessageHandler(const LPMSG pMsg, LRESULT& lRes);
	// ������ټ���Ϣ
	virtual bool TranslateAccelerator(MSG *pMsg);

	LRESULT ReflectNotifications(
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Inout_ bool& bHandled);

private:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UINT GetClassStyle() const;

protected:
	// �ؼ���ʵ��
	HINSTANCE m_hInstance;
	CControlUI *m_pRootControl;
	CControlUI *m_pEventClick;
	CControlUI *m_pEventHover;
	CControlUI *m_pFocus;
	CControlUI *m_pEventKey;

	// �������
	CDuiRect m_rcRestore;
	CDuiSize m_szRoundCorner;		
	CDuiSize m_szMinWindow;				// ������С
	CDuiSize m_szMaxWindow;				// �������
	CDuiSize m_szInitWindowSize;	// ��ʼ�����ڴ�С
	CDuiRect m_rcSizeBox;				// �ɵ����߿�߾�
	CDuiRect m_rcCaption;				// ��Ӧ�������϶�

	// ˢ��
	bool m_bShowUpdateRect;
	BYTE m_nAlpha;
	bool m_bLayedWindow;

	CDuiString m_strWindowTitle;

	// ��������Ⱦ
	bool m_bFirstLayout;
	bool m_bUpdateNeeded;
	bool m_bFocusNeeded;
	HDC m_hPaintDC;
	CMemDC m_OffscreenDC;
	IUIRender *m_pRenderEngine;
	FontObject *m_pDefaultFont;

	// Tooltip
	HWND m_hWndTooltip;
	TOOLINFO m_ToolTip;

	// Events
	CDuiPoint m_ptLastMousePos;
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	CStdPtrArray m_arrayDelayedCleanup;
	CStdPtrArray m_arrayPostPaintControls;
	CStdPtrArray m_arrayAsyncNotify;
	CStdPtrArray m_arrayNotifyFilters;
	CStdPtrArray m_arrayWindowMessageFilters;
	CStdPtrArray m_arrayPreMessageFilters;
	CStdPtrArray m_arrayTranslateAccelerators;
	CStdPtrArray m_arrayTimers;
	UINT m_uTimerID;

	CStdStringPtrMap m_mapNameHash;
	CStdStringPtrMap m_mapOptionGroup;

#ifdef _DEBUG
	void TestUICrossThread();
#endif // _DEBUG

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
