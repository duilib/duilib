#include "stdafx.h"
#include "ContainerUI.h"

UI_IMPLEMENT_DYNCREATE(CContainerUI);

CContainerUI::CContainerUI(void)
	: m_iChildPadding(0)
	, m_bAutoDestroy(true)
	, m_bDelayedDestroy(true)
	, m_bMouseChildEnabled(true)
	, m_bScrollProcess(false)
	, m_pVerticalScrollBar(NULL)
	, m_pHorizontalScrollBar(NULL)
{
}


CContainerUI::~CContainerUI(void)
{
	m_bDelayedDestroy = false;
	RemoveAll();
	if( m_pVerticalScrollBar )
		delete m_pVerticalScrollBar;
	if( m_pHorizontalScrollBar )
		delete m_pHorizontalScrollBar;
}

CControlUI* CContainerUI::GetItemAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_items.GetSize() )
		return NULL;
	return static_cast<CControlUI*>(m_items[iIndex]);
}

int CContainerUI::GetItemIndex(CControlUI* pControl) const
{
	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		if( static_cast<CControlUI*>(m_items[it]) == pControl )
		{
			return it;
		}
	}

	return -1;
}

bool CContainerUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		if( static_cast<CControlUI*>(m_items[it]) == pControl )
		{
			NeedUpdate();            
			m_items.Remove(it);
			return m_items.InsertAt(iIndex, pControl);
		}
	}

	return false;
}

int CContainerUI::GetCount() const
{
	return m_items.GetSize();
}

bool CContainerUI::Add(CControlUI* pControl)
{
	if( pControl == NULL)
		return false;

	if( m_pManager != NULL )
		m_pManager->InitControls(pControl, this);
	if( IsVisible() ) 
		NeedUpdate();
	else 
		pControl->SetInternVisible(false);
	return m_items.Add(pControl);   
}

bool CContainerUI::AddAt(CControlUI* pControl, int iIndex)
{
	if( pControl == NULL) return false;

	if( m_pManager != NULL )
		m_pManager->InitControls(pControl, this);
	if( IsVisible() )
		NeedUpdate();
	else
		pControl->SetInternVisible(false);
	return m_items.InsertAt(iIndex, pControl);
}

bool CContainerUI::Remove(CControlUI* pControl)
{
	if( pControl == NULL) return false;

	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		if( static_cast<CControlUI*>(m_items[it]) == pControl )
		{
			NeedUpdate();
			if( m_bAutoDestroy )
			{
				if( m_bDelayedDestroy && m_pManager )
					m_pManager->AddDelayedCleanup(pControl);             
				else delete pControl;
			}
			return m_items.Remove(it);
		}
	}
	return false;
}

bool CContainerUI::RemoveAt(int iIndex)
{
	CControlUI* pControl = GetItemAt(iIndex);
	if (pControl != NULL)
	{
		return CContainerUI::Remove(pControl);
	}

	return false;
}

void CContainerUI::RemoveAll()
{
	for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ )
	{
		if( m_bDelayedDestroy && m_pManager )
			m_pManager->AddDelayedCleanup(static_cast<CControlUI*>(m_items[it]));             
		else
			delete static_cast<CControlUI*>(m_items[it]);
	}
	m_items.Empty();
	NeedUpdate();
}

LPCTSTR CContainerUI::GetClass() const
{
	return _T("Container");
}

LPVOID CContainerUI::GetInterface(LPCTSTR lpszClass)
{

	if ( _tcscmp(lpszClass, _T("IContainer")) == 0 )
		return static_cast<IContainerUI*>(this);
	else if( _tcscmp(lpszClass, _T("Container")) == 0 )
		return this;
	else
		return CControlUI::GetInterface(lpszClass);
}

void CContainerUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		static_cast<CControlUI*>(m_items[it])->SetManager(pManager, this);
	}

	if( m_pVerticalScrollBar != NULL )
		m_pVerticalScrollBar->SetManager(pManager, this);
	if( m_pHorizontalScrollBar != NULL )
		m_pHorizontalScrollBar->SetManager(pManager, this);
	CControlUI::SetManager(pManager, pParent);
}
