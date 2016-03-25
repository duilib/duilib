#ifndef HorizontalLayoutUI_h__
#define HorizontalLayoutUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	HorizontalLayoutUI.h
// ������		: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-26 14:39:01
// ˵��			:	���ֿؼ�
/////////////////////////////////////////////////////////////*/
#pragma once
class DIRECTUI_API CHorizontalLayoutUI
	: public CContainerUI
{
public:
	CHorizontalLayoutUI(void);
	virtual ~CHorizontalLayoutUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetSepWidth(int iWidth);
	int GetSepWidth() const;
	void SetSepImmMode(bool bImmediately);
	bool IsSepImmMode() const;

	virtual void PostRender(IUIRender* pRender,LPCRECT pRcPaint);
	virtual void SetPosition(LPCRECT rc);
	virtual bool EventHandler(TEventUI& event);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	RECT GetThumbRect(bool bUseNew = false) const;
private:
	int m_iSepWidth;
	bool m_bImmMode;
	CDuiPoint m_ptLastMouse;
	CDuiRect m_rcNewPos;
};

#endif // HorizontalLayoutUI_h__
