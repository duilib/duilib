#include "stdafx.h"
#include "ScrollBarUI.h"

UI_IMPLEMENT_DYNCREATE(CScrollBarUI);

CScrollBarUI::CScrollBarUI(void)
	: m_pOwner(NULL)
	, m_nRange(100)
	, m_nScrollPos(0)
	, m_nLineSize(8)
	, m_nLastScrollPos(0)
	, m_nLastScrollOffset(0)
	, m_nScrollRepeatDelay(0)
	, m_bHorizontal(false)
	, m_bShowButton1(true)
	, m_bShowButton2(true)
	, m_uButton1State(UISTATE_Normal)
	, m_uButton2State(UISTATE_Normal)
	, m_uThumbState(UISTATE_Normal)
{
	m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
	m_ptLastMouse.x = m_ptLastMouse.y = 0;
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

bool CScrollBarUI::IsClientArea()
{
	return true;
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

void CScrollBarUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	CControlUI::SetAttribute(lpszName,lpszValue);
}

void CScrollBarUI::SetPosition(LPCRECT lpRc)
{
	CControlUI::SetPosition(lpRc);
	CDuiRect rc = m_rcControl;

	if( m_bHorizontal )
	{
		int cx = rc.right - rc.left;
		if( m_bShowButton1 ) cx -= m_cxyFixed.cy;
		if( m_bShowButton2 ) cx -= m_cxyFixed.cy;
		if( cx > m_cxyFixed.cy ) {
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 ) {
				m_rcButton1.right = rc.left + m_cxyFixed.cy;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
			}
			else {
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 ) {
				m_rcButton2.left = rc.right - m_cxyFixed.cy;
				m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
			}
			else {
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			m_rcThumb.top = rc.top;
			m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
			if( m_nRange > 0 )
			{
				int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
				if( cxThumb < m_cxyFixed.cy )
					cxThumb = m_cxyFixed.cy;

				m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
				m_rcThumb.right = m_rcThumb.left + cxThumb;
				if( m_rcThumb.right > m_rcButton2.left )
				{
					m_rcThumb.left = m_rcButton2.left - cxThumb;
					m_rcThumb.right = m_rcButton2.left;
				}
			}
			else
			{
				m_rcThumb.left = m_rcButton1.right;
				m_rcThumb.right = m_rcButton2.left;
			}
		}
		else
		{
			int cxButton = (rc.right - rc.left) / 2;
			if( cxButton > m_cxyFixed.cy )
				cxButton = m_cxyFixed.cy;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 )
			{
				m_rcButton1.right = rc.left + cxButton;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
			}
			else
			{
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.top = rc.top;
			m_rcButton2.right = rc.right;
			if( m_bShowButton2 )
			{
				m_rcButton2.left = rc.right - cxButton;
				m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
			}
			else
			{
				m_rcButton2.left = m_rcButton2.right;
				m_rcButton2.bottom = m_rcButton2.top;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
	else
	{
		int cy = rc.bottom - rc.top;
		if( m_bShowButton1 )
			cy -= m_cxyFixed.cx;
		if( m_bShowButton2 )
			cy -= m_cxyFixed.cx;
		if( cy > m_cxyFixed.cx )
		{
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 )
			{
				m_rcButton1.right = rc.left + m_cxyFixed.cx;
				m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
			}
			else
			{
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 )
			{
				m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
				m_rcButton2.right = rc.left + m_cxyFixed.cx;
			}
			else
			{
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			m_rcThumb.left = rc.left;
			m_rcThumb.right = rc.left + m_cxyFixed.cx;
			if( m_nRange > 0 )
			{
				int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
				if( cyThumb < m_cxyFixed.cx )
					cyThumb = m_cxyFixed.cx;

				m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
				m_rcThumb.bottom = m_rcThumb.top + cyThumb;
				if( m_rcThumb.bottom > m_rcButton2.top )
				{
					m_rcThumb.top = m_rcButton2.top - cyThumb;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			}
			else
			{
				m_rcThumb.top = m_rcButton1.bottom;
				m_rcThumb.bottom = m_rcButton2.top;
			}
		}
		else
		{
			int cyButton = (rc.bottom - rc.top) / 2;
			if( cyButton > m_cxyFixed.cx )
				cyButton = m_cxyFixed.cx;
			m_rcButton1.left = rc.left;
			m_rcButton1.top = rc.top;
			if( m_bShowButton1 )
			{
				m_rcButton1.right = rc.left + m_cxyFixed.cx;
				m_rcButton1.bottom = rc.top + cyButton;
			}
			else
			{
				m_rcButton1.right = m_rcButton1.left;
				m_rcButton1.bottom = m_rcButton1.top;
			}

			m_rcButton2.left = rc.left;
			m_rcButton2.bottom = rc.bottom;
			if( m_bShowButton2 )
			{
				m_rcButton2.top = rc.bottom - cyButton;
				m_rcButton2.right = rc.left + m_cxyFixed.cx;
			}
			else
			{
				m_rcButton2.top = m_rcButton2.bottom;
				m_rcButton2.right = m_rcButton2.left;
			}

			::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		}
	}
}

void CScrollBarUI::Render(IUIRender* pRender,LPCRECT pRcPaint)
{
	CUIPaint::GetInstance()->DrawScrollBar(pRender,this,pRcPaint);
}

void CScrollBarUI::SetVisible(bool bVisible /*= true*/)
{
	if( m_bIsVisible == bVisible )
		return;

	bool v = IsVisible();
	m_bIsVisible = bVisible;
	if( m_bIsFocused )
		m_bIsFocused = false;
}

void CScrollBarUI::SetEnabled(bool bEnable /*= true*/)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() )
	{
		m_uButton1State = 0;
		m_uButton2State = 0;
		m_uThumbState = 0;
	}
}

void CScrollBarUI::SetFocus()
{
	if( m_pOwner != NULL )
		m_pOwner->SetFocus();
	else
		CControlUI::SetFocus();
}
