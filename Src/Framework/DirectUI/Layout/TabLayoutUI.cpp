#include "stdafx.h"
#include "TabLayoutUI.h"

UI_IMPLEMENT_DYNCREATE(CTabLayoutUI);

CTabLayoutUI::CTabLayoutUI(void)
	: m_iCurSel(-1)
{
}


CTabLayoutUI::~CTabLayoutUI(void)
{
}

LPCTSTR CTabLayoutUI::GetClass() const
{
	return _T("TabLayout");
}

LPVOID CTabLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("TabLayout")) == 0 )
		return static_cast<CTabLayoutUI*>(this);
	return
		CContainerUI::GetInterface(pstrName);
}

bool CTabLayoutUI::Add(CControlUI* pControl)
{
	bool ret = CContainerUI::Add(pControl);
	if( !ret ) return ret;

	if(m_iCurSel == -1 && pControl->IsVisible())
	{
		m_iCurSel = GetItemIndex(pControl);
	}
	else
	{
		pControl->SetVisible(false);
	}

	return ret;
}

bool CTabLayoutUI::AddAt(CControlUI* pControl, int iIndex)
{
	bool ret = CContainerUI::AddAt(pControl, iIndex);
	if( !ret ) return ret;

	if(m_iCurSel == -1 && pControl->IsVisible())
	{
		m_iCurSel = GetItemIndex(pControl);
	}
	else if( m_iCurSel != -1 && iIndex <= m_iCurSel )
	{
		m_iCurSel += 1;
	}
	else
	{
		pControl->SetVisible(false);
	}

	return ret;
}

bool CTabLayoutUI::Remove(CControlUI* pControl)
{
	if( pControl == NULL) return false;

	int index = GetItemIndex(pControl);
	bool ret = CContainerUI::Remove(pControl);
	if( !ret ) return false;

	if( m_iCurSel == index)
	{
		if( GetCount() > 0 )
		{
			m_iCurSel=0;
			GetItemAt(m_iCurSel)->SetVisible(true);
		}
		else
			m_iCurSel=-1;
		NeedParentUpdate();
	}
	else if( m_iCurSel > index )
	{
		m_iCurSel -= 1;
	}

	return ret;
}

void CTabLayoutUI::RemoveAll()
{
	m_iCurSel = -1;
	CContainerUI::RemoveAll();
	NeedParentUpdate();
}

void CTabLayoutUI::SetPosition(LPCRECT rc)
{
	CControlUI::SetPosition(rc);
	CDuiRect rcTemp(m_rcControl);

	// Adjust for inset
	rcTemp.left += m_rcInset.left;
	rcTemp.top += m_rcInset.top;
	rcTemp.right -= m_rcInset.right;
	rcTemp.bottom -= m_rcInset.bottom;

	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		if( !pControl->IsVisible() )
			continue;
		if( pControl->IsFloat() )
		{
			SetFloatPos(it);
			continue;
		}

		if( it != m_iCurSel )
			continue;

		RECT rcPadding = pControl->GetPadding();
		rcTemp.left += rcPadding.left;
		rcTemp.top += rcPadding.top;
		rcTemp.right -= rcPadding.right;
		rcTemp.bottom -= rcPadding.bottom;

		SIZE szAvailable = { rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top };

		SIZE sz = pControl->EstimateSize(szAvailable);
		if( sz.cx == 0 )
		{
			sz.cx = max(0, szAvailable.cx);
		}
		if( sz.cx < pControl->GetMinWidth() )
			sz.cx = pControl->GetMinWidth();
		if( sz.cx > pControl->GetMaxWidth() )
			sz.cx = pControl->GetMaxWidth();

		if(sz.cy == 0)
		{
			sz.cy = max(0, szAvailable.cy);
		}
		if( sz.cy < pControl->GetMinHeight() )
			sz.cy = pControl->GetMinHeight();
		if( sz.cy > pControl->GetMaxHeight() )
			sz.cy = pControl->GetMaxHeight();

		RECT rcCtrl = { rcTemp.left, rcTemp.top, rcTemp.left + sz.cx, rcTemp.top + sz.cy};
		pControl->SetPosition(&rcCtrl);
	}
}

void CTabLayoutUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcscmp(lpszName, _T("selectedid")) == 0 )
		SelectItem(_ttoi(lpszValue));
	return
		CContainerUI::SetAttribute(lpszName, lpszValue);
}

int CTabLayoutUI::GetCurSel() const
{
		return m_iCurSel;
}

bool CTabLayoutUI::SelectItem(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_items.GetSize() )
		return false;
	if( iIndex == m_iCurSel )
		return true;

	int iOldSel = m_iCurSel;
	m_iCurSel = iIndex;
	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		if( it == iIndex )
		{
			GetItemAt(it)->SetVisible(true);
			GetItemAt(it)->SetFocus();
			SetPosition(&m_rcControl);
		}
		else
			GetItemAt(it)->SetVisible(false);
	}
	NeedParentUpdate();

	if( m_pManager != NULL )
	{
		m_pManager->SetNextTabControl();
		m_pManager->SendNotify(this, UINOTIFY_TAB_SELECTED, m_iCurSel, iOldSel);
	}
	return true;
}

bool CTabLayoutUI::SelectItem(CControlUI* pControl)
{
	int iIndex=GetItemIndex(pControl);
	if (iIndex==-1)
		return false;
	else
		return SelectItem(iIndex);
}
