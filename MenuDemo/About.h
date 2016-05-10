#pragma once

class CAbout : public WindowImplBase,DuiLib::CSkinSetting
{
public:
	bool m_bModal;
public:
	CAbout(HWND hWndParent,const RECT rc);
	~CAbout(void);

	DWORD DoModal();

	LPCTSTR GetWindowClassName() const;

	virtual void OnFinalMessage(HWND hWnd);

	void InitWindow();

	DWORD GetBkColor();

	void SetBkImage(LPCTSTR szImageName);

	void SetBkColor(DWORD dwBackColor);

	virtual CDuiString GetSkinFile();

	virtual CDuiString GetSkinFolder();

	virtual UILIB_RESOURCETYPE GetResourceType() const;

	virtual LPCTSTR GetResourceID() const;

	CDuiString GetZIPFileName() const;

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:

	//RECT parent_window_rect_;

protected:

	void Notify(TNotifyUI& msg);

	void OnPrepare(TNotifyUI& msg);
};

