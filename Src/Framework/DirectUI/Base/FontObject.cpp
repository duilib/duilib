#include "stdafx.h"
#include "FontObject.h"


FontObject::FontObject(void)
{
}


FontObject::~FontObject(void)
{
}

HFONT FontObject::GetFont()
{
	return m_hFont;
}

LOGFONT& FontObject::GetLogFont()
{
	return m_lfFont;
}
