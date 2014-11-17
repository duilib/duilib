#include "stdafx.h"
#include "ButtonUI.h"

UI_IMPLEMENT_DYNCREATE(CButtonUI);

CButtonUI::CButtonUI(void)
{
}


CButtonUI::~CButtonUI(void)
{
}

LPCTSTR CButtonUI::GetClass() const
{
	return _T("Button");
}

LPVOID CButtonUI::GetInterface(LPCTSTR lpszClass)
{
	if ( _tcscmp(lpszClass,_T("Button")))
		return this;
	else
		return CControlUI::GetInterface(lpszClass);
}
