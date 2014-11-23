#ifndef FontObject_h__
#define FontObject_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	FontObject.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-09 16:55:32
// 说明			:	Font Object
// 修改时间：	2014-11-23 shaoyuan1943@gmail.com
/////////////////////////////////////////////////////////////*/
#pragma once

typedef struct DIRECTUI_API _tsFontObject
{
	_tsFontObject(void);

	CDuiString m_IndexName;	
	CDuiString m_FaceName;	
	int m_nSize;	
	bool m_bBold;	
	bool m_bItalic;	

	bool m_bUnderline;	
	bool m_bStrikeout;

	HFONT GetFont();
private:
	HFONT m_hFont;
}FontObject;

#endif // FontObject_h__
