#pragma once

#include "UIBase.h"

namespace DuiLib {


class UILIB_API CWindowUI
	: public CWindowWnd
{
public:
	CWindowUI(void);
	~CWindowUI(void);

public:
	void Init(HWND hWnd);
	void NeedUpdate();
	void Invalidate(RECT& rcItem);

	HDC GetPaintDC() const;
	HWND GetPaintWindow() const;
	HWND GetTooltipWindow() const;

	POINT GetMousePos() const;
	SIZE GetClientSize() const;
	SIZE GetInitSize();
	void SetInitSize(int cx, int cy);
	RECT& GetSizeBox();
	void SetSizeBox(RECT& rcSizeBox);
	RECT& GetCaptionRect();
	void SetCaptionRect(RECT& rcCaption);
	SIZE GetRoundCorner() const;
	void SetRoundCorner(int cx, int cy);
	SIZE GetMinInfo() const;
	void SetMinInfo(int cx, int cy);
	SIZE GetMaxInfo() const;
	void SetMaxInfo(int cx, int cy);
	int GetTransparent() const;
	void SetTransparent(int nOpacity);
	void SetBackgroundTransparent(bool bTrans);
	bool IsShowUpdateRect() const;
	void SetShowUpdateRect(bool show);

	bool AttachDialog(CControlUI* pControl);
	bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
	void ReapObjects(CControlUI* pControl);

	bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
	void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
	void RemoveAllOptionGroups();

	CControlUI* GetFocus() const;
	void SetFocus(CControlUI* pControl);
	void SetFocusNeeded(CControlUI* pControl);

	bool SetNextTabControl(bool bForward = true);

	bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
	bool KillTimer(CControlUI* pControl, UINT nTimerID);
	void KillTimer(CControlUI* pControl);
	void RemoveAllTimers();

	// 获取释放焦点
	void SetCapture();
	void ReleaseCapture();
	bool IsCaptured();

	bool AddNotifier(INotifyUI* pControl);
	bool RemoveNotifier(INotifyUI* pControl);   
	void SendNotify(TNotifyUI& Msg, bool bAsync = false);
	void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	bool AddPreMessageFilter(IMessageFilterUI* pFilter);
	bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

	bool AddMessageFilter(IMessageFilterUI* pFilter);
	bool RemoveMessageFilter(IMessageFilterUI* pFilter);

	int GetPostPaintCount() const;
	bool AddPostPaint(CControlUI* pControl);
	bool RemovePostPaint(CControlUI* pControl);
	bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

	void AddDelayedCleanup(CControlUI* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);

	bool TranslateAccelerator(LPMSG pMsg);

	CControlUI* GetRoot() const;
	CControlUI* FindControl(POINT pt) const;
	CControlUI* FindControl(LPCTSTR pstrName) const;
	CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
	CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
	CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
	CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);
	CStdPtrArray* GetSubControlsByClass();

	static void MessageLoop();
	static bool TranslateMessage(const LPMSG pMsg);
	static void Term();

	bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

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

private:
	HWND m_hWndPaint;
	int m_nOpacity;
	HDC m_hDcPaint;
	HDC m_hDcOffscreen;
	HDC m_hDcBackground;
	HBITMAP m_hbmpOffscreen;
	HBITMAP m_hbmpBackground;
	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;
	bool m_bShowUpdateRect;
	//
	CControlUI* m_pRoot;
	CControlUI* m_pFocus;
	CControlUI* m_pEventHover;
	CControlUI* m_pEventClick;
	CControlUI* m_pEventKey;

	CStdPtrArray m_aMessageFilters;

	//
	POINT m_ptLastMousePos;
	SIZE m_szMinWindow;
	SIZE m_szMaxWindow;
	SIZE m_szInitWindowSize;
	RECT m_rcSizeBox;
	SIZE m_szRoundCorner;
	RECT m_rcCaption;
	UINT m_uTimerID;
	bool m_bFirstLayout;
	bool m_bUpdateNeeded;
	bool m_bFocusNeeded;
	bool m_bOffscreenPaint;
	bool m_bAlphaBackground;
	bool m_bMouseTracking;
	bool m_bMouseCapture;
	//
	CStdPtrArray m_aNotifiers;
	CStdPtrArray m_aTimers;
	CStdPtrArray m_aPreMessageFilters;
	
	CStdPtrArray m_aPostPaintControls;
	CStdPtrArray m_aDelayedCleanup;
	CStdPtrArray m_aAsyncNotify;
	CStdPtrArray m_aFoundControls;
	CStdStringPtrMap m_mNameHash;
	CStdStringPtrMap m_mOptionGroup;

	static short m_H;
	static short m_S;
	static short m_L;
	static CStdPtrArray m_aPreMessages;
	static CStdPtrArray m_aPlugins;

public:
	CStdPtrArray m_aTranslateAccelerator;
};

}
