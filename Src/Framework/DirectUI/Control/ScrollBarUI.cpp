#include "stdafx.h"
#include "ScrollBarUI.h"

UI_IMPLEMENT_DYNCREATE(CScrollBarUI);

CScrollBarUI::CScrollBarUI(void)
	: m_pOwner(NULL)
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

CContainerUI* CScrollBarUI::GetOwner() const
{
	return m_pOwner;
}

void CScrollBarUI::SetOwner(CContainerUI* pOwner)
{
	m_pOwner = pOwner;
}

bool CScrollBarUI::IsHorizontal()
{
	return m_bHorizontal;
}

void CScrollBarUI::SetHorizontal(bool bHorizontal /*= true*/)
{
	m_bHorizontal = bHorizontal;
}

int CScrollBarUI::GetScrollRange() const
{
	return m_nRange;
}

void CScrollBarUI::SetScrollRange(int nRange)
{
	if( m_nRange == nRange ) return;

	m_nRange = nRange;
	if( m_nRange < 0 )
		m_nRange = 0;
	if( m_nScrollPos > m_nRange )
		m_nScrollPos = m_nRange;
	SetPosition(&m_rcControl);
}

int CScrollBarUI::GetScrollPos() const
{
	return m_nScrollPos;
}

void CScrollBarUI::SetScrollPos(int nPos)
{
	if( m_nScrollPos == nPos )
		return;

	m_nScrollPos = nPos;
	if( m_nScrollPos < 0 )
		m_nScrollPos = 0;
	if( m_nScrollPos > m_nRange )
		m_nScrollPos = m_nRange;
	SetPosition(&m_rcControl);
}

int CScrollBarUI::GetLineSize() const
{
	return m_nLineSize;
}

void CScrollBarUI::SetLineSize(int nSize)
{
	m_nLineSize = nSize;
}
