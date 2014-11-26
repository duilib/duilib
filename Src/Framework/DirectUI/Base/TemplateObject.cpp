#include "stdafx.h"
#include "TemplateObject.h"


TemplateObject::TemplateObject(void)
{
}


TemplateObject::~TemplateObject(void)
{
	int nCount = m_arrayChild.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		TemplateObject* pTemplateObj = static_cast<TemplateObject*>(m_arrayChild.GetAt(i));
		delete pTemplateObj;
	}
}

void TemplateObject::SetType(LPCTSTR lpszType)
{
	m_strType = lpszType;
}

LPCTSTR TemplateObject::GetType(void)
{
	return m_strType.c_str();
}

void TemplateObject::SetAttribute(LPCTSTR lpszKey,LPCTSTR lpszValue)
{
	m_mapAttribute[lpszKey] = lpszValue;
}

LPCTSTR TemplateObject::GetAttribute(LPCTSTR lpszKey)
{
	AttributeMap::iterator iter = m_mapAttribute.find(lpszKey);
	if (iter != m_mapAttribute.end())
		return iter->second.c_str();

	return NULL;
}

int TemplateObject::GetChildCount(void)
{
	return m_arrayChild.GetSize();
}

TemplateObject* TemplateObject::GetChild(int nIndex)
{
	if ( nIndex >= 0 && nIndex < m_arrayChild.GetSize())
	{
		return static_cast<TemplateObject*>(m_arrayChild.GetAt(nIndex));
	}

	return NULL;
}

void TemplateObject::InsertChild(TemplateObject* pChild,int nAtIndex/*=-1*/)
{
	if ( nAtIndex == -1 )
		m_arrayChild.Add(pChild);
	else
		m_arrayChild.InsertAt(nAtIndex,pChild);
}
