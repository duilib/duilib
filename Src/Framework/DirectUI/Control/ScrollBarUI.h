#ifndef ScrollBarUI_h__
#define ScrollBarUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ScrollBarUI.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-14 9:20:00
// 说明			:	滚动条
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Control/ControlUI.h"

class DIRECTUI_API CScrollBarUI
	: public CControlUI
{
public:
	CScrollBarUI(void);
	virtual ~CScrollBarUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);

};

#endif // ScrollBarUI_h__
