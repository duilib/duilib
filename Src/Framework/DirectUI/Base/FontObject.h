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

	HFONT GetFont();
	LOGFONT& GetLogFont();
	int GetHeight();
	void SetFaceName(LPCTSTR lpszFace);
	void SetFontSize(int nSize);
	void SetBold(bool bBold);
	void SetUnderline(bool bUnderline);
	void SetItalic(bool bItalic);
	void SetStrikeout(bool bStrikeout);

protected:
	LOGFONT m_lfFont;
	HFONT m_hFont;
};

#endif // FontObject_h__
