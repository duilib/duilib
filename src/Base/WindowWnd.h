#ifndef WindowWnd_h__
#define WindowWnd_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	WindowWnd.h
// ������	: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-07 22:23:50
// ˵��		:	Windows���ڰ�װ��
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API CWindowWnd
{
public:
	CWindowWnd(void);
	virtual ~CWindowWnd(void);
	void SetAutoDelete(bool bAutoDelete = true);
public:

	HWND Create(HWND hwndParent, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT);

	LONG ModifyStyle(DWORD dwStyleAdd,DWORD dwStyleRemove = 0);
	LONG ModifyExStyle(DWORD dwExStyleAdd,DWORD dwExStyleRemove = 0);

	HWND Subclass(HWND hWnd);
	void Unsubclass();

	HWND GetHWND() const;
	operator HWND() const;

	HDC GetPaintDC() const;
	operator HDC() const;

	virtual void ShowWindow(int nCmdShow = SW_SHOW);
	virtual void CloseWindow(UINT nRet = IDOK);

	virtual UINT DoModal();
	virtual void EndModal(UINT nRet = IDOK);

	virtual BOOL MoveWindow(int x,int y,int width = -1, int height = -1,BOOL bRepaint= FALSE);

	virtual BOOL DestroyWindow();

	void CenterWindow();	// ���У�֧����չ��Ļ
	void SetSmallIcon(HICON hSmallIcon);
	void SetLargeIcon(HICON hLargeIcon);

	LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	void ResizeClient(int cx = -1, int cy = -1);

protected:
	bool RegisterWindowClass();		// ����Window
	bool RegisterSuperclass();			// ����ϵͳ��׼�ؼ�

	virtual LPCTSTR GetWindowClassName() const;		// ����Window
	virtual LPCTSTR GetSuperClassName() const;		// ����ϵͳ�ؼ�
	virtual UINT GetClassStyle() const;
	virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnFinalMessage(HWND hWnd);

	// ע��Ĵ�����Ϣѭ��
	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// ע��Ŀؼ���Ϣѭ��
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	HWND m_hWnd;
	HDC m_hPaintDC;
	WNDPROC m_OldWndProc;
	bool m_bSubclassed;
	bool m_bIsAutoDelete;
	bool m_bIsDoModal;
#ifdef _DEBUG
    DWORD m_dwRunningThread;
#endif // _DEBUG
};

#endif // WindowWnd_h__
