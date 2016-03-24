#ifndef LabelUI_h__
#define LabelUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	LabelUI.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-11-27 22:18:43
// ˵��			:	
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API CLabelUI
	: public CControlUI
{
public:
	CLabelUI(void);
	virtual ~CLabelUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);

	virtual bool EventHandler(TEventUI& event);
	virtual void Render(IUIRender* pRender, LPCRECT pRcPaint);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	RECT GetTextPadding() const;
	void SetTextPadding(LPCRECT lpRect);
	bool IsShowHtml();
	void SetShowHtml(bool bShowHtml = true);

	SIZE EstimateSize(SIZE szAvailable);

protected:
	CDuiRect m_rcTextPadding;
	bool m_bShowHtml;

};

#endif // LabelUI_h__
