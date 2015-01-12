#include "stdafx.h"
#include "ObjectUI.h"

CObjectUI::CObjectUI(void)
{
}


CObjectUI::~CObjectUI(void)
{
}

void CObjectUI::SetName(LPCTSTR lpszName)
{
	m_strName = lpszName;
}

LPCTSTR CObjectUI::GetName()
{
	return m_strName.c_str();
}

void CObjectUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if ( _tcsicmp(_T("Name"),lpszName) == 0 )
	{
		m_strName = lpszValue;
	}
}

LPCTSTR CObjectUI::GetClass() const
{
	return _T("Object");
}

LPVOID CObjectUI::GetInterface(LPCTSTR lpszClass)
{
	if( _tcscmp(lpszClass, _T("Object")) == 0 )
		return this;
	return NULL;
}

bool CObjectUI::IsClientArea(void)
{
	return false;
}
