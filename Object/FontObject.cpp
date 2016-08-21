#include "stdafx.h"
#include "FontObject.h"

namespace DuiLib
{

FontObject::FontObject()
	:nSize(0),
	bBold(false),
	bItalic(false),
	bUnderline(false),
	bStrikeout(false),
	strFontID(L""),
	strFaceName(L""),
	m_hFont(NULL)
{
	::ZeroMemory(&m_TextMetric,sizeof(TEXTMETRIC));
}

HFONT FontObject::GetFont()
{
	if (strFaceName.empty())
	{
		return 0;
	}

	if (m_hFont)
	{
		return m_hFont;
	}

	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcscpy(lf.lfFaceName, strFaceName.c_str());
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -nSize;
	if (bBold)
	{
		lf.lfWeight += FW_BOLD;
	}

	if (bUnderline)
	{
		lf.lfUnderline = TRUE;
	}

	if (bItalic)
	{
		lf.lfItalic = TRUE;
	}

	if (bStrikeout)
	{
		lf.lfStrikeOut = TRUE;
	}

	m_hFont = ::CreateFontIndirect(&lf);
	return m_hFont;
}

FontObject::~FontObject()
{
	if ( m_hFont != NULL)
	{
		::DeleteObject(m_hFont);
	}
}

TEXTMETRIC& FontObject::GetTextMetric()
{
	return m_TextMetric;
}
}