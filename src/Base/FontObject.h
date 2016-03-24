#ifndef FontObject_h__
#define FontObject_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	FontObject.h
// ������		: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-09 16:55:32
// ˵��			:	Font Object
// �޸�ʱ�䣺	2014-11-23 shaoyuan1943@gmail.com
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API FontObject
{
public:
	FontObject(void);
	~FontObject();
	
	CDuiString m_IndexName;	
	CDuiString m_FaceName;	
	int m_nSize;	
	bool m_bBold;	
	bool m_bItalic;	

	bool m_bUnderline;	
	bool m_bStrikeout;

	HFONT GetFont();
	TEXTMETRIC& GetTextMetric();

private:
	HFONT m_hFont;
	TEXTMETRIC m_TextMetric;
};

#endif // FontObject_h__
