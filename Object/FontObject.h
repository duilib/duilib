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

namespace DuiLib
{

class UILIB_API FontObject
{
public:
	FontObject(void);
	~FontObject();
	
	CDuiString strFontID;	
	CDuiString strFaceName;

	int nSize;	
	bool bBold;	
	bool bItalic;	
	bool bUnderline;	
	bool bStrikeout;

	HFONT GetFont();
	TEXTMETRIC& GetTextMetric();

private:
	HFONT m_hFont;
	TEXTMETRIC m_TextMetric;
};
}
#endif // FontObject_h__
