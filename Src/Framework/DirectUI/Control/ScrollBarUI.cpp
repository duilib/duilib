#include "stdafx.h"
#include "ScrollBarUI.h"

UI_IMPLEMENT_DYNCREATE(CScrollBarUI);

CScrollBarUI::CScrollBarUI(void)
{
}


CScrollBarUI::~CScrollBarUI(void)
{
}

LPCTSTR CScrollBarUI::GetClass() const
{
	return _T("ScrollBar");
}

LPVOID CScrollBarUI::GetInterface(LPCTSTR lpszClass)
{
	if ( _tcscmp(lpszClass,_T("ScrollBar")))
		return this;
	else
		return CControlUI::GetInterface(lpszClass);
}
