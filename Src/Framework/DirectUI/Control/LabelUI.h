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

};

#endif // LabelUI_h__
