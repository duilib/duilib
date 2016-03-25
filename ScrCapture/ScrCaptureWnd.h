#pragma once

class CScrCaptureWnd : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
private:
	CScrCaptureWnd();
	~CScrCaptureWnd();

public:
	static CScrCaptureWnd* Instance();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

	void Init();
	void OnPrepare();
	void Notify(TNotifyUI& msg);

	RECT GetWindowRect() const;
	RECT GetClipPadding() const;
	void SetClipPadding(RECT rc);
	RECT GetClipRect() const;
	RECT GetCanvasContainerRect() const;

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static CScrCaptureWnd* m_pInstance;
	CPaintManagerUI m_pm;
	HMENU m_hEditMenu;

	CControlUI* m_pDesktopImage;
	CControlUI* m_pDesktopMask;
	CControlUI* m_pDesktopMaskBorder;
	CContainerUI* m_pDesktopCanvasContainer;

	RECT m_rcWindow;
	RECT m_rcClipPadding;
	POINT m_ptClipBasePoint; // 调整基准点
	bool m_bClipChoiced; // 是否已选择截图区域
	TCHAR m_sClipDrawStringBuf[MAX_PATH];
};
