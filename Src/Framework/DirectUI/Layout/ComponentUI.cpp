#include "stdafx.h"
#include "ComponentUI.h"

UI_IMPLEMENT_DYNCREATE(CComponentUI);

CComponentUI::CComponentUI(void)
{
}


CComponentUI::~CComponentUI(void)
{
}

LPCTSTR CComponentUI::GetClass() const
{
	return _T("Component");
}

LPVOID CComponentUI::GetInterface(LPCTSTR lpszClass)
{
	if( _tcscmp(lpszClass, _T("Component")) == 0 )
		return this;
	else
		return CObjectUI::GetInterface(lpszClass);
}

bool CComponentUI::Notify(TNotifyUI *pMsg)
{
	INotifyUI *pNotifyFilter = NULL;
	int iCount = m_arrayNotifyFilters.GetSize();
	for ( int i=0; i<iCount;++i)
	{
		pNotifyFilter = static_cast<INotifyUI*>(m_arrayNotifyFilters.GetAt(i));
		if ( pNotifyFilter && pNotifyFilter->Notify(pMsg))
			return true;
	}
	return false;
}

void CComponentUI::AddNotify(INotifyUI *pNotify)
{
	if ( m_arrayNotifyFilters.Find(pNotify) == -1)
	{
		m_arrayNotifyFilters.Add(pNotify);
	}
}

void CComponentUI::RemoveNotify(INotifyUI *pNotify)
{
	int nIndex = m_arrayNotifyFilters.Find(pNotify);
	if ( nIndex != -1 )
	{
		m_arrayNotifyFilters.Remove(nIndex);
	}
}

