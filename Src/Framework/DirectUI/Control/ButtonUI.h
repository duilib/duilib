#ifndef ButtonUI_h__
#define ButtonUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ButtonUI.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-16 23:46:28
// 说明			:	Button
/////////////////////////////////////////////////////////////*/
#pragma once

class CButtonUI
	: public CControlUI
{
public:
	CButtonUI(void);
	virtual ~CButtonUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);
};

#endif // ButtonUI_h__
