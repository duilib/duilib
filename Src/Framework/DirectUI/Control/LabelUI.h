#ifndef LabelUI_h__
#define LabelUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	LabelUI.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-27 22:18:43
// 说明			:	
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
