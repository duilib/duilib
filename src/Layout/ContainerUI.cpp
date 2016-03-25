#include "stdafx.h"
#include "ContainerUI.h"

static int ZOrderCompare(LPVOID p1,LPVOID p2)
{
	CControlUI* w1 = (CControlUI*)p1;
	CControlUI* w2 = (CControlUI *)p2;
	if( w1->GetZOrder() > w2->GetZOrder() )
		return 1;
	if( w1->GetZOrder() < w2->GetZOrder() )
		return -1;
	return 0;
}

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

	if ( m_pNotifyFilter != NULL)
	{
		pControl->SetNotifyFilter(m_pNotifyFilter);
	}

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

void CContainerUI::Render(IUIRender* pRender,LPCRECT pRcPaint)
{
	CDuiRect rcTemp;
	// ˢ�������Ƿ���ؼ������ص�
	if ( !rcTemp.IntersectRect(pRcPaint, &m_rcControl))
		return;

	// �ص����������ƾ���
	CRenderClip clip;
	CRenderClip::GenerateClip(pRender->GetPaintDC(), rcTemp, clip);
	// �ص����Ȼ����ֿؼ�
	CControlUI::Render(pRender, pRcPaint);

	if( m_items.GetSize() > 0 )
	{   // �����ӿؼ�����Ҫ�ݹ����ͼ
		
		// �ӿؼ������м����ڱ߿�
		CDuiRect rcContent = m_rcControl;
		rcContent.left += m_rcInset.left;
		rcContent.top += m_rcInset.top;
		rcContent.right -= m_rcInset.right;
		rcContent.bottom -= m_rcInset.bottom;

		// �����Ҫ��ʾ������������������������Ŀ�߶�
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
			rcContent.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
			rcContent.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		// �������Ƿ��ڻ�ͼ������
		if( !rcTemp.IntersectRect(pRcPaint, &rcContent) )
		{   //����������ˢ�������ص�
			
			for( int it = 0; it < m_items.GetSize(); it++ )
			{
				// �ݹ��ӿؼ�������״̬�Ϳؼ��������ͼ�����ص��ģ�����
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( !pControl->IsVisible() )
					continue;
				if( !rcTemp.IntersectRect( pRcPaint, &pControl->GetPosition()) )
					continue;

				// �ӿؼ��Ǹ�������
				if( pControl ->IsFloat() )
				{
					// ��鸡���ؼ��Ƿ񳬳��˲��ֿؼ�����
					if( !rcTemp.IntersectRect(&m_rcControl, &pControl->GetPosition()) )
						continue;
					pControl->Render(pRender, pRcPaint);
				}
			}
		}
		else
		{   // ��������ˢ�������ص�

			// Ϊ�ص����ִ�����������
			CRenderClip childClip;
			CRenderClip::GenerateClip(pRender->GetPaintDC(), rcTemp, childClip);
			for( int it = 0; it < m_items.GetSize(); it++ )
			{   // �ݹ��ӿؼ�������״̬�Ϳؼ��������ͼ�����ص��ģ�����
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( !pControl->IsVisible() )
					continue;
				if( !rcTemp.IntersectRect(pRcPaint, &pControl->GetPosition()) )
					continue;

				// �ӿؼ��Ǹ�������
				if( pControl ->IsFloat() )
				{
					// ��鸡���ؼ��Ƿ񳬳��˲��ֿؼ�����
					if( !rcTemp.IntersectRect(&m_rcControl, &pControl->GetPosition()) )
						continue;

					// ���ƿռ����ص�������ˢ��
					CRenderClip::UseOldClipBegin(pRender->GetPaintDC(), childClip);
					pControl->Render(pRender, pRcPaint);
					CRenderClip::UseOldClipEnd(pRender->GetPaintDC(), childClip);
				}
				else
				{
					// ���Ǹ����ؼ�����󲻴��ڸ����ڣ��ؼ�������ˢ�����ص��ͻ�ͼ
					if( !rcTemp.IntersectRect( &rcContent, &pControl->GetPosition()) )
						continue;
					pControl->Render(pRender, pRcPaint);
				}
			}
		}
	}

	// ��󻭹�����
	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() )
	{
		if( rcTemp.IntersectRect( pRcPaint, &m_pVerticalScrollBar->GetPosition()) )
		{
			m_pVerticalScrollBar->Render(pRender, pRcPaint);
		}
	}

	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() )
	{
		if( rcTemp.IntersectRect( pRcPaint, &m_pHorizontalScrollBar->GetPosition()) )
		{
			m_pHorizontalScrollBar->Render(pRender, pRcPaint);
		}
	}
}

void CContainerUI::SetPosition(LPCRECT rc)
{
	CControlUI::SetPosition(rc);
	if( m_items.IsEmpty() )
		return;

	// ��������������
	CDuiRect rcTemp(rc);
	rcTemp.left += m_rcInset.left;
	rcTemp.top += m_rcInset.top;
	rcTemp.right -= m_rcInset.right;
	rcTemp.bottom -= m_rcInset.bottom;

	for( int it = 0; it < m_items.GetSize(); it++ )
	{   // �ݹ��ӿؼ��������꣬���ص�����
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
		if( !pControl->IsVisible() )
			continue;
		if( pControl->IsFloat() )
		{
			SetFloatPos(it);
		}
		else
		{
			pControl->SetPosition(&rcTemp); // ���з�float�ӿؼ��Ŵ������ͻ���
		}
	}
}

void CContainerUI::SetFloatPos(int iIndex)
{
	// ��ΪCControlUI::SetPos��float�Ĳ���Ӱ�죬���ﲻ�ܶ�float�����ӹ�������Ӱ��
	if( iIndex < 0 || iIndex >= m_items.GetSize() )
		return;

	CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);

	if( !pControl->IsVisible() )
		return;
	if( !pControl->IsFloat() )
		return;

	SIZE szXY = pControl->GetFixedXY();
	SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
	RECT rcCtrl = { 0 };
	if( szXY.cx >= 0 )
	{
		rcCtrl.left = m_rcControl.left + szXY.cx;
		rcCtrl.right = m_rcControl.left + szXY.cx + sz.cx;
	}
	else
	{
		rcCtrl.left = m_rcControl.right + szXY.cx - sz.cx;
		rcCtrl.right = m_rcControl.right + szXY.cx;
	}
	if( szXY.cy >= 0 )
	{
		rcCtrl.top = m_rcControl.top + szXY.cy;
		rcCtrl.bottom = m_rcControl.top + szXY.cy + sz.cy;
	}
	else
	{
		rcCtrl.top = m_rcControl.bottom + szXY.cy - sz.cy;
		rcCtrl.bottom = m_rcControl.bottom + szXY.cy;
	}
	//if( pControl->IsRelativePos() )
	//{
	//	TRelativePosUI tRelativePos = pControl->GetRelativePos();
	//	SIZE szParent = {m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top};
	//	if(tRelativePos.szParent.cx != 0)
	//	{
	//		int nIncrementX = szParent.cx-tRelativePos.szParent.cx;
	//		int nIncrementY = szParent.cy-tRelativePos.szParent.cy;
	//		rcCtrl.left += (nIncrementX*tRelativePos.nMoveXPercent/100);
	//		rcCtrl.top += (nIncrementY*tRelativePos.nMoveYPercent/100);
	//		rcCtrl.right = rcCtrl.left+sz.cx+(nIncrementX*tRelativePos.nZoomXPercent/100);
	//		rcCtrl.bottom = rcCtrl.top+sz.cy+(nIncrementY*tRelativePos.nZoomYPercent/100);
	//	}
	//	pControl->SetRelativeParentSize(szParent);
	//}
	pControl->SetPosition(&rcCtrl);
}

CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	// �����ӿؼ�

	// ��������������ʾ������״̬�������ǰ���ֿؼ��������أ�����״̬������Ӧ
	if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() )
		return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() )
		return NULL;

	// ��������������������
	if( (uFlags & UIFIND_HITTEST) != 0 )
	{
		// ���е㲻�ڲ��ֿؼ������ڣ�����
		if( !m_rcControl.PtInRect(*(static_cast<LPPOINT>(pData))) )
			return NULL;

		// �������ӿؼ��������������¼���������
		if( !m_bMouseChildEnabled )
		{
			CControlUI* pResult = NULL;
			if( m_pVerticalScrollBar != NULL )
				pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
			if( pResult == NULL && m_pHorizontalScrollBar != NULL )
				pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
			if( pResult == NULL )
				pResult = CControlUI::FindControl(Proc, pData, uFlags);
			return pResult;
		}
	}

	CControlUI* pResult = NULL;
	if( m_pVerticalScrollBar != NULL )
		pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
	if( pResult == NULL && m_pHorizontalScrollBar != NULL )
		pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
	if( pResult != NULL )
		return pResult;

	if( (uFlags & UIFIND_ME_FIRST) != 0 )
	{
		CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
		if( pControl != NULL )
			return pControl;
	}

	// ���㲼�ֿؼ���������������
	CDuiRect rc = m_rcControl;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	// ���������������ʾ״̬��������Ӧ�Ŀ�߶�
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
		rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	//m_items.Sort(ZOrderCompare);

	if( (uFlags & UIFIND_TOP_FIRST) != 0 )
	{
		for( int it = m_items.GetSize() - 1; it >= 0; it-- )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
			if( pControl != NULL )
			{
				
				if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !rc.PtInRect(*(static_cast<LPPOINT>(pData))) )
					continue;
				else 
					return pControl;
			}            
		}
	}
	else
	{
		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
			if( pControl != NULL )
			{
				if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !rc.PtInRect(*(static_cast<LPPOINT>(pData))) )
					continue;
				else 
					return pControl;
			} 
		}
	}

	if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 )
		pResult = CControlUI::FindControl(Proc, pData, uFlags);
	return pResult;
}

CControlUI* CContainerUI::FindSubControl(LPCTSTR pstrSubControlName)
{
	CControlUI* pSubControl=NULL;
	pSubControl=static_cast<CControlUI*>(m_pManager->FindSubControlByName(this,pstrSubControlName));
	return pSubControl;
}

bool CContainerUI::EventHandler(TEventUI& event)
{
	if( !IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND )
	{
		bool bRet = false;
		if( m_pParent != NULL )
			bRet = m_pParent->EventHandler(event);
		else
			bRet = CControlUI::EventHandler(event);
		return bRet;
	}

	if( event.dwType == UIEVENT_SETFOCUS ) 
	{
		m_bIsFocused = true;
		return true;
	}
	if( event.dwType == UIEVENT_KILLFOCUS ) 
	{
		m_bIsFocused = false;
		return true;
	}
	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled() )
	{
		if( event.dwType == UIEVENT_KEYDOWN ) 
		{
			switch( event.chKey )
			{
			case VK_DOWN:
				LineDown();
				return true;
			case VK_UP:
				LineUp();
				return true;
			case VK_NEXT:
				PageDown();
				return true;
			case VK_PRIOR:
				PageUp();
				return true;
			case VK_HOME:
				HomeUp();
				return true;
			case VK_END:
				EndDown();
				return true;
			}
		}
		else if( event.dwType == UIEVENT_SCROLLWHEEL )
		{
			switch( LOWORD(event.wParam) )
			{
			case SB_LINEUP:
				LineUp();
				return true;
			case SB_LINEDOWN:
				LineDown();
				return true;
			}
		}
	}
	else if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled() )
	{
		if( event.dwType == UIEVENT_KEYDOWN ) 
		{
			switch( event.chKey ) {
			case VK_DOWN:
				LineRight();
				return true;
			case VK_UP:
				LineLeft();
				return true;
			case VK_NEXT:
				PageRight();
				return true;
			case VK_PRIOR:
				PageLeft();
				return true;
			case VK_HOME:
				HomeLeft();
				return true;
			case VK_END:
				EndRight();
				return true;
			}
		}
		else if( event.dwType == UIEVENT_SCROLLWHEEL )
		{
			switch( LOWORD(event.wParam) )
			{
			case SB_LINEUP:
				LineLeft();
				return true;
			case SB_LINEDOWN:
				LineRight();
				return true;
			}
		}
	}
	return CControlUI::EventHandler(event);
}

void CContainerUI::EnableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
{
	if( bEnableVertical && !m_pVerticalScrollBar )
	{
		m_pVerticalScrollBar = new CScrollBarUI;
		m_pVerticalScrollBar->SetOwner(this);
		m_pVerticalScrollBar->SetManager(m_pManager, NULL);
		if ( m_pManager )
		{
			//LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
			//if( pDefaultAttributes )
			//{
			//	m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
			//}
		}
	}
	else if( !bEnableVertical && m_pVerticalScrollBar )
	{
		delete m_pVerticalScrollBar;
		m_pVerticalScrollBar = NULL;
	}

	if( bEnableHorizontal && !m_pHorizontalScrollBar )
	{
		m_pHorizontalScrollBar = new CScrollBarUI;
		m_pHorizontalScrollBar->SetHorizontal(true);
		m_pHorizontalScrollBar->SetOwner(this);
		m_pHorizontalScrollBar->SetManager(m_pManager, NULL);
#pragma TODO(ʵ�ֹ�������Ƥ����ʼ��)
		/*if ( m_pManager )
		{
			LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
			if( pDefaultAttributes ) {
				m_pHorizontalScrollBar->ApplyAttributeList(pDefaultAttributes);
			}
		}*/
	}
	else if( !bEnableHorizontal && m_pHorizontalScrollBar )
	{
		delete m_pHorizontalScrollBar;
		m_pHorizontalScrollBar = NULL;
	}

	NeedUpdate();
}

SIZE CContainerUI::GetScrollPos() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
		sz.cy = m_pVerticalScrollBar->GetScrollPos();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		sz.cx = m_pHorizontalScrollBar->GetScrollPos();
	return sz;
}

SIZE CContainerUI::GetScrollRange() const
{
	SIZE sz = {0, 0};
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
		sz.cy = m_pVerticalScrollBar->GetScrollRange();
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		sz.cx = m_pHorizontalScrollBar->GetScrollRange();
	return sz;
}

void CContainerUI::SetScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
		int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
		m_pVerticalScrollBar->SetScrollPos(szPos.cy);
		cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
	}

	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
		int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
		m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
		cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
	}

	if( cx == 0 && cy == 0 ) return;

	RECT rcPos;
	for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;

		rcPos = pControl->GetPosition();
		rcPos.left -= cx;
		rcPos.right -= cx;
		rcPos.top -= cy;
		rcPos.bottom -= cy;
		pControl->SetPosition(&rcPos);
	}

	Invalidate();
}

void CContainerUI::LineUp()
{
	int cyLine = 8;
#pragma TODO(������Ҫ��������)
	//if( m_pManager )
	//	cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

	SIZE sz = GetScrollPos();
	sz.cy -= cyLine;
	SetScrollPos(sz);
}

void CContainerUI::LineDown()
{
	int cyLine = 8;
	//if( m_pManager )
	//	cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

	SIZE sz = GetScrollPos();
	sz.cy += cyLine;
	SetScrollPos(sz);
}

void CContainerUI::PageUp()
{
	SIZE sz = GetScrollPos();
	int iOffset = m_rcControl.bottom - m_rcControl.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
	sz.cy -= iOffset;
	SetScrollPos(sz);
}

void CContainerUI::PageDown()
{
	SIZE sz = GetScrollPos();
	int iOffset = m_rcControl.bottom - m_rcControl.top - m_rcInset.top - m_rcInset.bottom;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
		iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
	sz.cy += iOffset;
	SetScrollPos(sz);
}

void CContainerUI::HomeUp()
{
	SIZE sz = GetScrollPos();
	sz.cy = 0;
	SetScrollPos(sz);
}

void CContainerUI::EndDown()
{
	SIZE sz = GetScrollPos();
	sz.cy = GetScrollRange().cy;
	SetScrollPos(sz);
}

void CContainerUI::LineLeft()
{
	SIZE sz = GetScrollPos();
	sz.cx -= 8;
	SetScrollPos(sz);
}

void CContainerUI::LineRight()
{
	SIZE sz = GetScrollPos();
	sz.cx += 8;
	SetScrollPos(sz);
}

void CContainerUI::PageLeft()
{
	SIZE sz = GetScrollPos();
	int iOffset = m_rcControl.right - m_rcControl.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() )
		iOffset -= m_pVerticalScrollBar->GetFixedWidth();
	sz.cx -= iOffset;
	SetScrollPos(sz);
}

void CContainerUI::PageRight()
{
	SIZE sz = GetScrollPos();
	int iOffset = m_rcControl.right - m_rcControl.left - m_rcInset.left - m_rcInset.right;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
	sz.cx += iOffset;
	SetScrollPos(sz);
}

void CContainerUI::HomeLeft()
{
	SIZE sz = GetScrollPos();
	sz.cx = 0;
	SetScrollPos(sz);
}

void CContainerUI::EndRight()
{
	SIZE sz = GetScrollPos();
	sz.cx = GetScrollRange().cx;
	SetScrollPos(sz);
}

void CContainerUI::SetNotifyFilter(INotifyUI* pNotifyFilter)
{
	m_pNotifyFilter = pNotifyFilter;
	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		static_cast<CControlUI*>(m_items[it])->SetNotifyFilter(pNotifyFilter);
	}
}

void CContainerUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if( _tcsicmp(lpszName, _T("inset")) == 0 )
	{
		CDuiCodeOperation::StringToRect(lpszValue,&m_rcInset);
		NeedUpdate();
	}
	else if( _tcsicmp(lpszName, _T("mousechild")) == 0 )
		SetMouseChildEnabled(_tcsicmp(lpszValue, _T("true")) == 0);
	else if( _tcsicmp(lpszName, _T("vscrollbar")) == 0 )
	{
		EnableScrollBar(_tcsicmp(lpszValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
	}
	else if( _tcsicmp(lpszName, _T("hscrollbar")) == 0 )
	{
		EnableScrollBar(GetVerticalScrollBar() != NULL, _tcsicmp(lpszValue, _T("true")) == 0);
	}
	else if( _tcsicmp(lpszName, _T("vscrollbarstyle")) == 0 )
	{
		EnableScrollBar(true, GetHorizontalScrollBar() != NULL);
		if( GetVerticalScrollBar() )
			GetVerticalScrollBar()->SetAttribute(lpszName,lpszValue);
	}
	else if( _tcsicmp(lpszName, _T("hscrollbarstyle")) == 0 )
	{
		EnableScrollBar(GetVerticalScrollBar() != NULL, true);
		if( GetHorizontalScrollBar() )
			GetHorizontalScrollBar()->SetAttribute(lpszName,lpszValue);
	}
	else if( _tcsicmp(lpszName, _T("childpadding")) == 0 )
		SetChildPadding(_ttoi(lpszValue));
	else
	CControlUI::SetAttribute(lpszName,lpszValue);
}

RECT CContainerUI::GetInset() const
{
	return m_rcInset;
}

void CContainerUI::SetInset(RECT rcInset)
{
	m_rcInset = rcInset;
	NeedUpdate();
}

int CContainerUI::GetChildPadding() const
{
	return m_iChildPadding;
}

void CContainerUI::SetChildPadding(int iPadding)
{
	m_iChildPadding = iPadding;
	NeedUpdate();
}

bool CContainerUI::IsAutoDestroy() const
{
	return m_bAutoDestroy;
}

void CContainerUI::SetAutoDestroy(bool bAuto)
{
	m_bAutoDestroy = bAuto;
}

bool CContainerUI::IsDelayedDestroy() const
{
	return m_bDelayedDestroy;
}

void CContainerUI::SetDelayedDestroy(bool bDelayed)
{
	m_bDelayedDestroy = bDelayed;
}

bool CContainerUI::IsMouseChildEnabled() const
{
	return m_bMouseChildEnabled;
}

void CContainerUI::SetMouseChildEnabled(bool bEnable /*= true*/)
{
	m_bMouseChildEnabled = bEnable;
}

void CContainerUI::SetVisible(bool bVisible /*= true*/)
{
	if( m_bIsVisible == bVisible )
		return;
	CControlUI::SetVisible(bVisible);
	int nCount = m_items.GetSize();
	for( int it = 0; it < nCount; it++ )
	{
		static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
	}
}

// �߼��ϣ�Container�ؼ��������˷���
// ���ô˷����Ľ���ǣ��ڲ��ӿؼ����أ��ؼ�������Ȼ��ʾ��������Ч������
void CContainerUI::SetInternVisible(bool bVisible /*= true*/)
{
	CControlUI::SetInternVisible(bVisible);
	if( m_items.IsEmpty() )
		return;
	int nCount = m_items.GetSize();
	for( int it = 0; it < nCount; it++ )
	{
		// �����ӿؼ���ʾ״̬
		// InternVisible״̬Ӧ���ӿؼ��Լ�����
		static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
	}
}

CScrollBarUI* CContainerUI::GetVerticalScrollBar() const
{
	return m_pVerticalScrollBar;
}

CScrollBarUI* CContainerUI::GetHorizontalScrollBar() const
{
	return m_pHorizontalScrollBar;
}

void CContainerUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
{
	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() )
	{
		RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
		m_pHorizontalScrollBar->SetPosition(&rcScrollBarPos);
	}

	if( m_pVerticalScrollBar == NULL )
		return;

	if( cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible() )
	{
		m_pVerticalScrollBar->SetVisible(true);
		m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
		m_pVerticalScrollBar->SetScrollPos(0);
		m_bScrollProcess = true;
		SetPosition(&m_rcControl);
		m_bScrollProcess = false;
		return;
	}
	// No scrollbar required
	if( !m_pVerticalScrollBar->IsVisible() )
		return;

	// Scroll not needed anymore?
	int cyScroll = cyRequired - (rc.bottom - rc.top);
	if( cyScroll <= 0 && !m_bScrollProcess)
	{
		m_pVerticalScrollBar->SetVisible(false);
		m_pVerticalScrollBar->SetScrollPos(0);
		m_pVerticalScrollBar->SetScrollRange(0);
		SetPosition(&m_rcControl);
	}
	else
	{
		RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
		m_pVerticalScrollBar->SetPosition(&rcScrollBarPos);

		if( m_pVerticalScrollBar->GetScrollRange() != cyScroll )
		{
			int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));

			if( m_pVerticalScrollBar->GetScrollRange() == 0 )
			{
				m_pVerticalScrollBar->SetVisible(false);
				m_pVerticalScrollBar->SetScrollPos(0);
			}

			if( iScrollPos > m_pVerticalScrollBar->GetScrollPos() )
			{
				SetPosition(&m_rcControl);
			}
		}
	}
}
