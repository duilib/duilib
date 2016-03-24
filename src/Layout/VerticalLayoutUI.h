#ifndef VerticalLayoutUI_h__
#define VerticalLayoutUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	VerticalLayoutUI.h
// ������		: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-26 15:15:58
// ˵��			:	���ֿؼ�
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API CVerticalLayoutUI
	: public CContainerUI
{
public:
	CVerticalLayoutUI(void);
	virtual ~CVerticalLayoutUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetSepHeight(int iHeight);
	int GetSepHeight() const;
	void SetSepImmMode(bool bImmediately);
	bool IsSepImmMode() const;

	virtual void PostRender(IUIRender* pRender,LPCRECT pRcPaint);
	virtual void SetPosition(LPCRECT rc);
	virtual bool EventHandler(TEventUI& event);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	RECT GetThumbRect(bool bUseNew = false) const;
private:
	int m_iSepHeight;
	bool m_bImmMode;
	CDuiPoint m_ptLastMouse;
	CDuiRect m_rcNewPos;
};

#endif // VerticalLayoutUI_h__
