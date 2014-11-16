#ifndef FontObject_h__
#define FontObject_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	FontObject.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-09 16:55:32
// 说明			:	Font Object
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API FontObject
{
public:
	FontObject(void);
	~FontObject(void);

private:
	HFONT m_hFont;
	CDuiString sFontName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;
	TEXTMETRIC tm;
};

#endif // FontObject_h__
