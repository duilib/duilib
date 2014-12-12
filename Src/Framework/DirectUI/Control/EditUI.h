#ifndef EditUI_h__
#define EditUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	EditUI.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-12-11 12:19:43
// 说明			:	
/////////////////////////////////////////////////////////////*/
#pragma once

class CEditWnd;
class DIRECTUI_API CEditUI
	: public CLabelUI
{
	friend class CEditWnd;
public:
	CEditUI(void);
	virtual ~CEditUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetEnabled(bool bEnable = true);
	void SetText(LPCTSTR pstrText);
	void SetMaxChar(UINT uMax);
	UINT GetMaxChar();
	void SetReadOnly(bool bReadOnly);
	bool IsReadOnly() const;
	void SetPasswordMode(bool bPasswordMode);
	bool IsPasswordMode() const;
	void SetPasswordChar(TCHAR cPasswordChar);
	TCHAR GetPasswordChar() const;
	void SetNumberOnly(bool bNumberOnly);
	bool IsNumberOnly() const;
	int GetWindowStyls() const;

	void SetNativeEditBkColor(DWORD dwBkColor);
	DWORD GetNativeEditBkColor() const;

	void SetSel(long nStartChar, long nEndChar);
	void SetSelAll();
	void SetReplaceSel(LPCTSTR lpszReplace);

	virtual void SetPosition(LPCRECT rc);
	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true);
	virtual SIZE EstimateSize(SIZE szAvailable);
	virtual bool EventHandler(TEventUI& event);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	virtual void Init();

protected:
	void ShowDefaultText(BOOL bShow);
	virtual void SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState);
	virtual void Render(IUIRender* pRender, LPCRECT pRcPaint);



protected:
	CEditWnd* m_pWindow;

	UINT m_uMaxChar;
	bool m_bReadOnly;
	bool m_bPasswordMode;
	TCHAR m_cPasswordChar;

	DWORD m_dwEditbkColor;
	int m_iWindowStyls;

	ImageObject *m_pHoverImage;
	ImageObject *m_pFocusedImage;
	ImageObject *m_pDisabledImage;

	CDuiString m_sDefaultText;
	BOOL m_bShowDefault;
	DWORD m_clrDefaultText;
	DWORD m_backupTextClr;
	TCHAR m_backupPasswordChar;
	bool m_backupPasswordMode;

};

#endif // EditUI_h__
