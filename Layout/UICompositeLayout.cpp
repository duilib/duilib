#include "stdafx.h"
#include "UICompositeLayout.h"


namespace DuiLib
{
	
UI_IMPLEMENT_DYNCREATE(CCompositeLayoutUI);

CCompositeLayoutUI::CCompositeLayoutUI(void)
{
}


CCompositeLayoutUI::~CCompositeLayoutUI(void)
{
	m_pManager->RemoveNotify(this);
}

LPVOID CCompositeLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("CompositeLayout")) == 0 )
		return static_cast<CCompositeLayoutUI*>(this);
	return CChildLayoutUI::GetInterface(pstrName);
}

LPCTSTR CCompositeLayoutUI::GetClass() const
{
	return _T("CompositeLayoutUI");
}

void CCompositeLayoutUI::AddNotifyFilter(INotifyUI *pFilter)
{
	if ( m_arrayNotifyFilters.Find(pFilter) == -1)
	{
		m_arrayNotifyFilters.Add(pFilter);
	}
}

void CCompositeLayoutUI::RemoveNotifyFilter(INotifyUI *pFilter)
{
	int nIndex = m_arrayNotifyFilters.Find(pFilter);
	if ( nIndex != -1 )
	{
		m_arrayNotifyFilters.Remove(nIndex);
	}
}

bool CCompositeLayoutUI::Notify(TNotifyUI *pMsg)
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

void CCompositeLayoutUI::DoInit()
{
	this->SetNotifyFilter(this);
	m_pManager->AddNotify(this);
}

}