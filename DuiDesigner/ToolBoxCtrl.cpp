// ToolBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "ToolBoxCtrl.h"

#define AFX_ID_SCROLL_VERT 2

#define visualManager CMFCVisualManager::GetInstance()

//////////////////////////////////////////////////////////////////////////
//CToolElement

IMPLEMENT_DYNAMIC(CToolElement, CObject)

CToolElement::CToolElement(const CString& strTabName):m_strName(strTabName)
{
	m_bTab=TRUE;
	m_bExpanded=TRUE;

	Init();
}

CToolElement::CToolElement(const CString& strName,int nClass,UINT nIDIcon):m_strName(strName),
m_nClass(nClass)
{
	m_bTab=FALSE;
	m_bExpanded=FALSE;
	m_hIcon=(HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIDIcon), IMAGE_ICON,16, 16, 0);

	Init();
}

CToolElement::~CToolElement()
{
	while (!m_lstSubTools.IsEmpty())
	{
		delete m_lstSubTools.RemoveHead();
	}
}

void CToolElement::Init()
{
	m_pWndList = NULL;
	m_bEnabled = TRUE;
	m_pParent = NULL;
	m_bNameIsTruncated = FALSE;

	m_Rect.SetRectEmpty();
	m_bIsVisible = TRUE;
}

BOOL CToolElement::AddSubTool(CToolElement* pTool)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pTool);

	if (!IsToolTab())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (pTool->m_pWndList != NULL)
	{
		for (POSITION pos = pTool->m_pWndList->m_lstToolTabs.GetHeadPosition(); pos != NULL;)
		{
			CToolElement* pLstToolTab = pTool->m_pWndList->m_lstToolTabs.GetNext(pos);
			ASSERT_VALID(pLstToolTab);

			if (pLstToolTab == pTool || pLstToolTab->IsSubTool(pTool))
			{
				// Can't ad the same tool twice
				ASSERT(FALSE);
				return FALSE;
			}
		}
	}

	pTool->m_pParent = this;

	m_lstSubTools.AddTail(pTool);
	pTool->m_pWndList = m_pWndList;

	return TRUE;
}

BOOL CToolElement::RemoveSubTool(CToolElement*& pTool, BOOL bDelete/* = TRUE*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pTool);

	for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
	{
		POSITION posSaved = pos;

		CToolElement* pSubTool = m_lstSubTools.GetNext(pos);
		ASSERT_VALID(pSubTool);

		if (pSubTool == pTool)
		{
			m_lstSubTools.RemoveAt(posSaved);

			if (m_pWndList != NULL && m_pWndList->m_pSel == pTool)
			{
				m_pWndList->m_pSel = NULL;
			}

			if (bDelete)
			{
				delete pTool;
				pTool = NULL;
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CToolElement::Expand(BOOL bExpand)
{
	ASSERT_VALID(this);
	ASSERT(IsToolTab());

	if (m_bExpanded == bExpand || m_lstSubTools.IsEmpty())
	{
		return;
	}

	m_bExpanded = bExpand;

	if (m_pWndList != NULL && m_pWndList->GetSafeHwnd() != NULL)
	{
		ASSERT_VALID(m_pWndList);
		m_pWndList->AdjustLayout();

		CRect rectRedraw = m_pWndList->m_rectList;
		rectRedraw.top = m_Rect.top;

		m_pWndList->RedrawWindow(rectRedraw);
	}
}

CToolElement* CToolElement::GetTool(int nClass) const
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pTool = m_lstSubTools.GetNext(pos);
		ASSERT_VALID(pTool);

		if (pTool->GetClass()==nClass)
		{
			return pTool;
		}
	}

	return NULL;
}

BOOL CToolElement::IsSubTool(CToolElement* pSubTool) const
{
	ASSERT_VALID(this);
	ASSERT_VALID(pSubTool);

	for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pTool = m_lstSubTools.GetNext(pos);
		ASSERT_VALID(pTool);

		if (pSubTool == pTool)
		{
			return TRUE;
		}
	}

	return FALSE;
}

int CToolElement::GetExpandedSubTools(BOOL bIncludeHidden) const
{
	ASSERT_VALID(this);

	if (!m_bExpanded)
	{
		return 0;
	}

	int nCount = 0;
	if(bIncludeHidden)
		nCount=m_lstSubTools.GetCount();
	else
	{
		for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
		{
			CToolElement* pTool = m_lstSubTools.GetNext(pos);
			ASSERT_VALID(pTool);

			if (pTool->IsVisible())
			{
				nCount ++;
			}
		}
	}
	return nCount;
}

CToolElement* CToolElement::HitTest(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	if (m_Rect.PtInRect(point))
	{
		return this;
	}

	if(!IsToolTab()||!IsExpanded())
		return NULL;

	for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pTool = m_lstSubTools.GetNext(pos);
		ASSERT_VALID(pTool);

		CToolElement* pHit = pTool->HitTest(point);
		if (pHit != NULL)
		{
			return pHit;
		}
	}

	return NULL;
}

BOOL CToolElement::IsParentExpanded() const
{
	ASSERT_VALID(this);
	if(m_pParent==NULL)
		return TRUE;

	if (!m_pParent->IsExpanded())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CToolElement::IsSelected() const
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	return m_pWndList->m_pSel == this;
}

BOOL CToolElement::IsHovered() const
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	return m_pWndList->m_pHover==this;
}

void CToolElement::SetOwnerList(CToolBoxCtrl* pWndList)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWndList);

	m_pWndList = pWndList;

	for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pTool = m_lstSubTools.GetNext(pos);
		ASSERT_VALID(pTool);

		pTool->SetOwnerList(m_pWndList);
	}
}

void CToolElement::Redraw()
{
	ASSERT_VALID(this);

	if (m_pWndList != NULL)
	{
		ASSERT_VALID(m_pWndList);
		m_pWndList->InvalidateRect(m_Rect);

		m_pWndList->UpdateWindow();
	}
}

void CToolElement::OnDrawExpandBox(CDC* pDC, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(m_pWndList);
	ASSERT(IsToolTab());

	CPoint ptCenter = rect.CenterPoint();

	int nBoxSize = min(9, rect.Width());

	rect = CRect(ptCenter, CSize(1, 1));
	rect.InflateRect(nBoxSize / 2, nBoxSize / 2);

	COLORREF clrText = afxGlobalData.clrBarText;

	visualManager->OnDrawExpandingBox(pDC, rect, m_bExpanded && !m_lstSubTools.IsEmpty(), clrText);
}

void CToolElement::OnDrawIcon(CDC* pDC, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(m_pWndList);

	if (IsToolTab()||m_hIcon==NULL)
	{
		return;
	}

 	rect.OffsetRect(3, 2);
	DrawIconEx(pDC->GetSafeHdc(),rect.left,rect.top,m_hIcon,16,16,0,NULL,DI_NORMAL);
}

void CToolElement::OnDrawName(CDC* pDC, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(m_pWndList);

	rect.DeflateRect(AFX_TEXT_MARGIN, 0);

	pDC->DrawText(m_strName, rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);

	m_bNameIsTruncated = pDC->GetTextExtent(m_strName).cx > rect.Width();
}

void CToolElement::Reposition(int& y)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	CRect rectOld = m_Rect;

	BOOL bShowProperty = m_bIsVisible &&IsParentExpanded();

	if (bShowProperty)
	{
		int nHeight=IsToolTab() ? m_pWndList->m_nTabHeight:m_pWndList->m_nToolHeight;

		m_Rect = CRect(m_pWndList->m_rectList.left+3, y, m_pWndList->m_rectList.right-3, y + nHeight);
		y += nHeight;
	}
	else
	{
		m_Rect.SetRectEmpty();
	}

	for (POSITION pos = m_lstSubTools.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pTool = m_lstSubTools.GetNext(pos);
		ASSERT_VALID(pTool);

		pTool->Reposition(y);
	}
}

///////////////////////////////////////////////////////////////
// CToolBoxCtrl

IMPLEMENT_DYNAMIC(CToolBoxCtrl, CWnd)

CToolBoxCtrl::CToolBoxCtrl()
{
	m_hFont = NULL;
	m_rectList.SetRectEmpty();
	m_nTabHeight = 16;
	m_nToolHeight = 20;
	m_nVertScrollOffset = 0;
	m_nVertScrollTotal = 0;
	m_nVertScrollPage = 0;
	m_pSel = NULL;
	m_pHover=NULL;

	m_clrBackground=(COLORREF)-1;
	m_clrSelected=RGB(225,230,232);
	m_clrHover=RGB(193,210,238);
	m_clrSelectedBorder=RGB(49,106,197);
	m_clrToolTab=RGB(217,215,198);
}

CToolBoxCtrl::~CToolBoxCtrl()
{
}


BEGIN_MESSAGE_MAP(CToolBoxCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSELEAVE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CToolBoxCtrl message handlers
BOOL CToolBoxCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(afxGlobalData.RegisterWindowClass(_T("Afx:ToolBox")), _T(""), dwStyle, rect, pParentWnd, nID, NULL);
}

int CToolBoxCtrl::AddToolTab(CToolElement* pToolTab, BOOL bRedraw, BOOL bAdjustLayout)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pToolTab);

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pListTool = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pListTool);

		if (pListTool == pToolTab)
		{
			// Can't add the same ToolTab twice
			ASSERT(FALSE);
			return -1;
		}
	}

	pToolTab->SetOwnerList(this);

	m_lstToolTabs.AddTail(pToolTab);
	int nIndex = (int) m_lstToolTabs.GetCount() - 1;

	if (bAdjustLayout)
	{
		AdjustLayout();

		if (bRedraw && GetSafeHwnd() != NULL)
		{
			pToolTab->Redraw();
		}
	}

	return nIndex;
}

BOOL CToolBoxCtrl::DeleteToolTab(CToolElement*& pTool, BOOL bRedraw/* = TRUE*/, BOOL bAdjustLayout/* = TRUE*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pTool);

	BOOL bFound = FALSE;

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		POSITION posSaved = pos;

		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		if (pToolTab == pTool) // ToolTab
		{
			bFound = TRUE;

			m_lstToolTabs.RemoveAt(posSaved);
			break;
		}

		if (pToolTab->RemoveSubTool(pTool, FALSE))
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		return FALSE;
	}

	if (m_pSel == pTool)
	{
		m_pSel = NULL;
	}

	delete pTool;
	pTool = NULL;

	if (bAdjustLayout)
	{
		AdjustLayout();
		return TRUE;
	}

	if (bRedraw && GetSafeHwnd() != NULL)
	{
		RedrawWindow();
	}

	return TRUE;
}

void CToolBoxCtrl::RemoveAll()
{
	ASSERT_VALID(this);

	while (!m_lstToolTabs.IsEmpty())
	{
		delete m_lstToolTabs.RemoveHead();
	}

	m_pSel = NULL;
}

void CToolBoxCtrl::SetCurSel(CToolElement* pTool, BOOL bRedraw/* = TRUE*/)
{
	if(pTool==NULL)
		return;

	m_pSel=pTool;
	if(bRedraw)
	m_pSel->Redraw();
}

void CToolBoxCtrl::SetCurSel(int nClass,BOOL bRedraw/*=TRUE*/)
{
	ASSERT_VALID(this);

	CToolElement* pTool=NULL;
	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		pTool=pToolTab->GetTool(nClass);
		if(pTool)
		{
			CToolElement* pOldSel=m_pSel;
			m_pSel=pTool;
			if(bRedraw)
			{
				if(pOldSel)
					pOldSel->Redraw();
				m_pSel->Redraw();
			}

			return;
		}
	}
}

void CToolBoxCtrl::ExpandAll(BOOL bExpand/* = TRUE*/)
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		pToolTab->Expand(bExpand);
	}
}

void CToolBoxCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClient;
	GetClientRect(rectClient);
	OnFillBackground(pDC, rectClient);

	HFONT hfontOld = SetCurrFont(pDC);
	pDC->SetBkMode(TRANSPARENT);
	OnDrawList(pDC);
	::SelectObject(pDC->GetSafeHdc(), hfontOld);
}

void CToolBoxCtrl::OnFillBackground(CDC* pDC, CRect rectClient)
{
	ASSERT_VALID(pDC);

	if (m_brBackground.GetSafeHandle() == NULL)
	{
		pDC->FillRect(rectClient, &afxGlobalData.brBtnFace);
	}
	else
	{
		pDC->FillRect(rectClient, &m_brBackground);
	}
}

void CToolBoxCtrl::OnDrawList(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		if (!OnDrawTool(pDC, pToolTab))
		{
			break;
		}
	}
}

BOOL CToolBoxCtrl::OnDrawTool(CDC* pDC, CToolElement* pTool) const
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(pTool);

	if (!pTool->m_Rect.IsRectEmpty())
	{
		if (pTool->m_Rect.top >= m_rectList.bottom)
		{
			return FALSE;
		}

		CRect rectName = pTool->m_Rect;
		BOOL bFlag=FALSE;
		if(pTool->IsSelected())
		{
			bFlag=TRUE;
			CBrush bsh(m_clrSelected);
			CBrush* oldBsh=pDC->SelectObject(&bsh);
			CPen pen(PS_SOLID,1,m_clrSelectedBorder);
			CPen* oldPen=pDC->SelectObject(&pen);
			pDC->Rectangle(&rectName);
			pDC->SelectObject(oldBsh);
			pDC->SelectObject(oldPen);
		}
		if(!bFlag&&pTool->IsHovered())
		{
			bFlag=TRUE;
			CBrush bsh(m_clrHover);
			CBrush* oldBsh=pDC->SelectObject(&bsh);
			CPen pen(PS_SOLID,1,m_clrSelectedBorder);
			CPen* oldPen=pDC->SelectObject(&pen);
			pDC->Rectangle(&rectName);
			pDC->SelectObject(oldBsh);
			pDC->SelectObject(oldPen);
		}
		

		int nHeight=m_nToolHeight;
		if (pTool->IsToolTab())
		{
			CRect rectExpand = rectName;
			nHeight=m_nTabHeight;
			rectExpand.right = nHeight;

			if(!bFlag)
			{
				CRect rectFill=rectName;
				rectFill.DeflateRect(1,1);
				CBrush bsh(m_clrToolTab);
				pDC->FillRect(rectName, &bsh);
			}

			CRgn rgnClipExpand;
			CRect rectExpandClip = rectExpand;
			rectExpandClip.bottom = min(rectExpandClip.bottom, m_rectList.bottom);

			rgnClipExpand.CreateRectRgnIndirect(&rectExpandClip);
			pDC->SelectClipRgn(&rgnClipExpand);

			pTool->OnDrawExpandBox(pDC, rectExpand);
		}

		COLORREF clrTextOld = (COLORREF)-1;
		if (!pTool->IsEnabled())
		{
			clrTextOld = pDC->SetTextColor(afxGlobalData.clrGrayedText);
		}

		CRect rectIcon = pTool->m_Rect;
		rectIcon.right = rectIcon.left + nHeight;

		CRgn rgnClipIcon;
		CRect rectIconClip = rectIcon;
		rectIconClip.bottom = min(rectIconClip.bottom, m_rectList.bottom);

		rgnClipIcon.CreateRectRgnIndirect(&rectIconClip);
		pDC->SelectClipRgn(&rgnClipIcon);

		pTool->OnDrawIcon(pDC, rectIcon);

		CRgn rgnClipName;
		rectName.left += rectIcon.right;
		CRect rectNameClip = rectName;
		rectNameClip.bottom = min(rectNameClip.bottom, m_rectList.bottom);

		rgnClipName.CreateRectRgnIndirect(&rectNameClip);
		pDC->SelectClipRgn(&rgnClipName);

		HFONT hOldFont = NULL;
		if (pTool->IsToolTab())
		{
			hOldFont = (HFONT) ::SelectObject(pDC->GetSafeHdc(), m_fontToolTab.GetSafeHandle());
		}

		pTool->OnDrawName(pDC, rectName);

		if (hOldFont != NULL)
		{
			::SelectObject(pDC->GetSafeHdc(), hOldFont);
		}

		pDC->SelectClipRgn(NULL);

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor(clrTextOld);
		}
	}

	if (pTool->IsExpanded())
	{
		for (POSITION pos = pTool->m_lstSubTools.GetHeadPosition(); pos != NULL;)
		{
			if (!OnDrawTool(pDC, pTool->m_lstSubTools.GetNext(pos)))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void CToolBoxCtrl::SetCustomColors(COLORREF clrBackground, COLORREF clrSelected, COLORREF clrHover, COLORREF clrSelectedBorder,
					 COLORREF clrToolTab)
{
	m_clrBackground=clrBackground;
	m_clrSelected=clrSelected;
	m_clrHover=clrHover;
	m_clrSelectedBorder=clrSelectedBorder;
	m_clrToolTab=clrToolTab;

	m_brBackground.DeleteObject();
	if (m_clrBackground != (COLORREF)-1)
		m_brBackground.CreateSolidBrush(m_clrBackground);
}

void CToolBoxCtrl::GetCustomColors(COLORREF& clrBackground, COLORREF& clrSelected, COLORREF& clrHover, COLORREF& clrSelectedBorder,
					 COLORREF& clrToolTab)
{
	clrBackground=m_clrBackground;
	clrSelected=m_clrSelected;
	clrHover=m_clrHover;
	clrSelectedBorder=m_clrSelectedBorder;
	clrToolTab=m_clrToolTab;
}

HFONT CToolBoxCtrl::SetCurrFont(CDC* pDC)
{
	ASSERT_VALID(pDC);

	return(HFONT) ::SelectObject(pDC->GetSafeHdc(), m_hFont != NULL ? m_hFont : ::GetStockObject(DEFAULT_GUI_FONT));
}

CToolElement* CToolBoxCtrl::HitTest(CPoint pt) const
{
	ASSERT_VALID(this);

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		CToolElement* pHit = pToolTab->HitTest(pt);
		if (pHit != NULL)
		{
			return pHit;
		}
	}

	return NULL;
}

void CToolBoxCtrl::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_rectList = rectClient;

	SetScrollSizes();

	ReposTools();

	RedrawWindow();
}

void CToolBoxCtrl::SetScrollSizes()
{
	ASSERT_VALID(this);

	if (m_wndScrollVert.GetSafeHwnd() == NULL)
	{
		return;
	}

	if (m_nToolHeight == 0)
	{
		m_nVertScrollPage = 0;
		m_nVertScrollTotal = 0;
		m_nVertScrollOffset = 0;
	}
	else
	{
		m_nVertScrollPage = m_rectList.Height() / m_nToolHeight + 1;
		m_nVertScrollTotal = GetTotalItems(FALSE /* Visible only */);

		if (m_nVertScrollTotal < m_nVertScrollPage)
		{
			m_nVertScrollPage = 0;
			m_nVertScrollTotal = 0;
		}

		m_nVertScrollOffset = min(m_nVertScrollOffset, m_nVertScrollTotal);
	}

	SCROLLINFO si;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);

	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	si.nMin = 0;
	si.nMax = m_nVertScrollTotal;
	si.nPage = m_nVertScrollPage;
	si.nPos = m_nVertScrollOffset;

	SetScrollInfo(SB_VERT, &si, TRUE);
	m_wndScrollVert.EnableScrollBar(m_nVertScrollTotal > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
	m_wndScrollVert.EnableWindow();
}

int CToolBoxCtrl::GetTotalItems(BOOL bIncludeHidden) const
{
	ASSERT_VALID(this);

	int nCount = 0;

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		nCount += pToolTab->GetExpandedSubTools(bIncludeHidden) + 1;
	}

	return nCount;
}

void CToolBoxCtrl::ReposTools()
{
	ASSERT_VALID(this);

	int y = m_rectList.top - m_nToolHeight * m_nVertScrollOffset;

	for (POSITION pos = m_lstToolTabs.GetHeadPosition(); pos != NULL;)
	{
		CToolElement* pToolTab = m_lstToolTabs.GetNext(pos);
		ASSERT_VALID(pToolTab);

		pToolTab->Reposition(y);
	}
}

void CToolBoxCtrl::CreateToolTabFont()
{
	if (m_fontToolTab.GetSafeHandle() != NULL)
	{
		m_fontToolTab.DeleteObject();
	}

	CFont* pFont = CFont::FromHandle((HFONT) ::GetStockObject(DEFAULT_GUI_FONT));
	ASSERT_VALID(pFont);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	pFont->GetLogFont(&lf);

	lf.lfWeight = FW_BOLD;
	m_fontToolTab.CreateFontIndirect(&lf);
}
void CToolBoxCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	AdjustLayout();
}

int CToolBoxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_wndScrollVert.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, rectDummy, this, AFX_ID_SCROLL_VERT);

	AdjustLayout();
	CreateToolTabFont();

	return 0;
}

void CToolBoxCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pSel != NULL && pScrollBar->GetSafeHwnd() != NULL)
	{
		return;
	}

	int nPrevOffset = m_nVertScrollOffset;

	switch (nSBCode)
	{
	case SB_LINEUP:
		m_nVertScrollOffset--;
		break;

	case SB_LINEDOWN:
		m_nVertScrollOffset++;
		break;

	case SB_TOP:
		m_nVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		m_nVertScrollOffset = m_nVertScrollTotal;
		break;

	case SB_PAGEUP:
		m_nVertScrollOffset -= m_nVertScrollPage;
		break;

	case SB_PAGEDOWN:
		m_nVertScrollOffset += m_nVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_nVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	m_nVertScrollOffset = min(max(0, m_nVertScrollOffset), m_nVertScrollTotal - m_nVertScrollPage + 1);

	if (m_nVertScrollOffset == nPrevOffset)
	{
		return;
	}

	SetScrollPos(SB_VERT, m_nVertScrollOffset);

	ReposTools();

	int dy = m_nToolHeight *(nPrevOffset - m_nVertScrollOffset);
	ScrollWindow(0, dy, m_rectList, m_rectList);

// 	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CToolBoxCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CToolElement* pOldHover=m_pHover;
	m_pHover=HitTest(point);
	if(pOldHover==m_pHover)
		return;
	if(pOldHover)
		pOldHover->Redraw();
	if(m_pHover)
		m_pHover->Redraw();

	// Post message when the mouse pointer leaves the window 
	TRACKMOUSEEVENT   tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = HOVER_DEFAULT;
	::TrackMouseEvent(&tme);

	CWnd::OnMouseMove(nFlags, point);
}

void CToolBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CToolElement* pOldSel=m_pSel;
	m_pSel=HitTest(point);
	if(m_pSel==NULL)
	{
		m_pSel=pOldSel;
		return;
	}
	if(!m_pSel->IsEnabled())
		return;
	if(m_pSel!=pOldSel)
	{
		m_pSel->Redraw();
		if(pOldSel)
			pOldSel->Redraw();
	}

	if(m_pSel->IsToolTab())
		m_pSel->Expand(!m_pSel->IsExpanded());

	CWnd::OnLButtonDown(nFlags, point);
}

void CToolBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CToolBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

// 	return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}

void CToolBoxCtrl::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	if(m_pHover)
	{
		CToolElement* pHover=m_pHover;
		m_pHover=NULL;
		pHover->Redraw();
	}

	CWnd::OnMouseLeave();
}

void CToolBoxCtrl::OnDestroy()
{
	while (!m_lstToolTabs.IsEmpty())
	{
		delete m_lstToolTabs.RemoveHead();
	}

	m_pSel = NULL;

	CWnd::OnDestroy();
}
