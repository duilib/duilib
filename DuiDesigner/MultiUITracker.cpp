#include "StdAfx.h"
#include "MultiUITracker.h"

/////////////////////////////////////////////////////////////////////////////
// CUITracker
// CUITracker global state

// this array describes all 8 handles (clock-wise)
const HandleInfo g_HandleInfo[] =
{
	// corner handles (top-left, top-right, bottom-right, bottom-left
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 0,  0,  0, 1, 3 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 0, -1,  0, 0, 2 },
	{ offsetof(RECT, right), offsetof(RECT, bottom),    0, 0, -1, -1, 3, 1 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     0, 0,  0, -1, 2, 0 },

	// side handles (top, right, bottom, left)
	{ offsetof(RECT, left), offsetof(RECT, top),        1, 0,  0,  0, 4, 6 },
	{ offsetof(RECT, right), offsetof(RECT, top),       0, 1, -1,  0, 7, 5 },
	{ offsetof(RECT, left), offsetof(RECT, bottom),     1, 0,  0, -1, 6, 4 },
	{ offsetof(RECT, left), offsetof(RECT, top),        0, 1,  0,  0, 5, 7 }
};

// this array is indexed by the offset of the RECT member / sizeof(int)
const RectInfo g_RectInfo[] =
{
	{ offsetof(RECT, right), +1 },
	{ offsetof(RECT, bottom), +1 },
	{ offsetof(RECT, left), -1 },
	{ offsetof(RECT, top), -1 },
};

/////////////////////////////////////////////////////////////////////////////
// CUITracker intitialization

HCURSOR CUITracker::m_hCursors[10];
HBRUSH CUITracker::m_hHatchBrush=NULL;

CUITracker::CUITracker()
{
	Init();
}

CUITracker::CUITracker(LPCRECT lpSrcRect, UINT nStyle)
{
	ASSERT(AfxIsValidAddress(lpSrcRect, sizeof(RECT), FALSE));

	Init();

	m_rect.CopyRect(lpSrcRect);
	m_nStyle = nStyle;
}

void CUITracker::Init()
{
	m_clrDottedLine=RGB(0,0,0);
	m_hDottedLinePen=CreatePen(PS_DOT,0,m_clrDottedLine);
	m_clrHandleBorder=RGB(0,0,0);
	m_hHandlePen=CreatePen(PS_SOLID,1,m_clrHandleBorder);
	m_clrHandleBackground=RGB(255,255,255);
	m_hHandleBrush=CreateSolidBrush(m_clrHandleBackground);
	m_hMoveHandleBitmap=(HBITMAP)::LoadImage(::AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_BITMAP_MOVEHANDLE),
		IMAGE_BITMAP,0,0,0);
	BITMAP bm; 
	GetObject(m_hMoveHandleBitmap,sizeof(BITMAP),(LPBYTE)&bm);

	m_nMask=0xFF;
	m_nControlType=typeControl;
	m_nMoveHandleSize=bm.bmWidth;

	Construct();
}

void CUITracker::Construct()
{
	// do one-time initialization if necessary
	static BOOL bInitialized;
	if (!bInitialized)
	{
		// sanity checks for assumptions we make in the code
		ASSERT(sizeof(((RECT*)NULL)->left) == sizeof(int));
		ASSERT(offsetof(RECT, top) > offsetof(RECT, left));
		ASSERT(offsetof(RECT, right) > offsetof(RECT, top));
		ASSERT(offsetof(RECT, bottom) > offsetof(RECT, right));

		if (m_hHatchBrush == NULL)
		{
			// create the hatch pattern + bitmap
			WORD hatchPattern[8];
			WORD wPattern = 0x1111;
			for (int i = 0; i < 4; i++)
			{
				hatchPattern[i] = wPattern;
				hatchPattern[i+4] = wPattern;
				wPattern <<= 1;
			}
			HBITMAP hatchBitmap = CreateBitmap(8, 8, 1, 1, hatchPattern);
			if (hatchBitmap == NULL)
			{
				AfxThrowResourceException();
			}

			// create black hatched brush
			m_hHatchBrush = CreatePatternBrush(hatchBitmap);
			DeleteObject(hatchBitmap);
			if (m_hHatchBrush == NULL)
			{
				AfxThrowResourceException();
			}
		}

		// Note: all track cursors must live in same module
		HINSTANCE hInst = AfxFindResourceHandle(
			ATL_MAKEINTRESOURCE(AFX_IDC_TRACK4WAY), ATL_RT_GROUP_CURSOR);

		// initialize the cursor array
		m_hCursors[0] = ::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_TRACKNWSE));
		m_hCursors[1] = ::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_TRACKNESW));
		m_hCursors[2] = m_hCursors[0];
		m_hCursors[3] = m_hCursors[1];
		m_hCursors[4] = ::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_TRACKNS));
		m_hCursors[5] = ::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_TRACKWE));
		m_hCursors[6] = m_hCursors[4];
		m_hCursors[7] = m_hCursors[5];
		m_hCursors[8] = ::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_TRACK4WAY));
		m_hCursors[9] = ::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_MOVE4WAY));

		// get default handle size from Windows profile setting
		static const TCHAR szWindows[] = _T("windows");
		static const TCHAR szInplaceBorderWidth[] =
			_T("oleinplaceborderwidth");
		bInitialized = TRUE;
	}

	m_nStyle = 0;
	m_nHandleSize = 4;
	m_sizeMin.cy = m_sizeMin.cx = m_nHandleSize*2;

	m_rectLast.SetRectEmpty();
	m_sizeLast.cx = m_sizeLast.cy = 0;
	m_bErase = FALSE;
	m_bFinalErase =  FALSE;
}

CUITracker::~CUITracker()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUITracker operations

void CUITracker::Draw(CDC* pDC) const
{
	// set initial DC state
	VERIFY(pDC->SaveDC() != 0);

	// get normalized rectangle
	CRect rect = m_rect;
	rect.NormalizeRect();

	CPen* pOldPen = NULL;
	CBrush* pOldBrush = NULL;
	CGdiObject* pTemp;
	int nOldROP;

	pDC->SetBkMode(TRANSPARENT);
	// draw lines
	if ((m_nStyle & (dottedLine|solidLine)) != 0)
	{
		if (m_nStyle & dottedLine)
			pOldPen = pDC->SelectObject(CPen::FromHandle(m_hDottedLinePen));
		else
			pOldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		nOldROP = pDC->SetROP2(R2_COPYPEN);
		int offset=GetHandleSize(rect)/2;
		rect.InflateRect(offset, offset);   // borders are one pixel outside
		pDC->Rectangle(rect.left, rect.top, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// if hatchBrush is going to be used, need to unrealize it
	if ((m_nStyle & (hatchInside|hatchedBorder)) != 0)
		UnrealizeObject(m_hHatchBrush);

	// hatch inside
	if ((m_nStyle & hatchInside) != 0)
	{
		pTemp = pDC->SelectStockObject(NULL_PEN);
		if (pOldPen == NULL)
			pOldPen = (CPen*)pTemp;
		pTemp = pDC->SelectObject(CBrush::FromHandle(m_hHatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(TRANSPARENT);
		nOldROP = pDC->SetROP2(R2_MASKNOTPEN);
		pDC->Rectangle(rect.left+1, rect.top+1, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// draw hatched border
	if ((m_nStyle & hatchedBorder) != 0)
	{
		pTemp = pDC->SelectObject(CBrush::FromHandle(m_hHatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(OPAQUE);
		CRect rectTrue;
		GetTrueRect(&rectTrue);
		pDC->PatBlt(rectTrue.left, rectTrue.top, rectTrue.Width(),
			rect.top-rectTrue.top, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.bottom,
			rectTrue.Width(), rectTrue.bottom-rect.bottom, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.top, rect.left-rectTrue.left,
			rect.Height(), 0x000F0001 /* Pn */);
		pDC->PatBlt(rect.right, rect.top, rectTrue.right-rect.right,
			rect.Height(), 0x000F0001 /* Pn */);
	}

	// draw resize handles
	pDC->SelectObject(m_hHandlePen);
	pDC->SelectObject(m_hHandleBrush);
	if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				pDC->Rectangle(&rect);
			}
		}
	}

	//draw move handle
	if(m_nControlType==typeContainer)
	{
		GetMoveHandleRect(&rect);
		CDC hCloneDC;
		hCloneDC.CreateCompatibleDC(pDC);
		HBITMAP hOldBitmap=(HBITMAP)hCloneDC.SelectObject(m_hMoveHandleBitmap);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &hCloneDC, 0, 0, SRCCOPY);
		hCloneDC.SelectObject(hOldBitmap);
		::DeleteDC(hCloneDC);
	}

	// cleanup pDC state
	if (pOldPen != NULL)
		pDC->SelectObject(pOldPen);
	if (pOldBrush != NULL)
		pDC->SelectObject(pOldBrush);
	VERIFY(pDC->RestoreDC(-1));
}

BOOL CUITracker::SetCursor(CPoint point, UINT nHitTest) const
{
	// trackers should only be in client area
	if (nHitTest != HTCLIENT)
		return FALSE;

	// do hittest and normalize hit
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
		return FALSE;

	// need to normalize the hittest such that we get proper cursors
	nHandle = NormalizeHit(nHandle);

	// handle special case of hitting area between handles
	//  (logically the same -- handled as a move -- but different cursor)
	if (nHandle == hitMiddle && !m_rect.PtInRect(point))
	{
		// only for trackers with hatchedBorder (ie. in-place resizing)
		if (m_nStyle & hatchedBorder)
			nHandle = (TrackerHit)10;
	}

	ENSURE(nHandle < _countof(m_hCursors));
 	::SetCursor(m_hCursors[nHandle]);
	return TRUE;
}

int CUITracker::HitTest(CPoint point) const
{
	TrackerHit hitResult = hitNothing;

	CRect rectTrue;
	GetTrueRect(&rectTrue);
	ASSERT(rectTrue.left <= rectTrue.right);
	ASSERT(rectTrue.top <= rectTrue.bottom);
	if (rectTrue.PtInRect(point))
	{
		if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
			hitResult = (TrackerHit)HitTestHandles(point);
		else
			hitResult = hitMiddle;
	}
	return hitResult;
}

int CUITracker::NormalizeHit(int nHandle) const
{
	ENSURE(nHandle <= 8 && nHandle >= -1);
	if (nHandle == hitMiddle || nHandle == hitNothing)
		return nHandle;
	ENSURE(0 <= nHandle && nHandle < _countof(g_HandleInfo));
	const HandleInfo* pHandleInfo = &g_HandleInfo[nHandle];
	if (m_rect.Width() < 0)
	{
		nHandle = (TrackerHit)pHandleInfo->nInvertX;
		ENSURE(0 <= nHandle && nHandle < _countof(g_HandleInfo));
		pHandleInfo = &g_HandleInfo[nHandle];
	}
	if (m_rect.Height() < 0)
		nHandle = (TrackerHit)pHandleInfo->nInvertY;
	return nHandle;
}

BOOL CUITracker::Track(CWnd* pWnd, CPoint point, BOOL bAllowInvert,
						 CDC* pDCClipTo)
{
	// perform hit testing on the handles
	int nHandle = HitTestHandles(point);
	if (nHandle < 0)
	{
		// didn't hit a handle, so just return FALSE
		return FALSE;
	}

	// otherwise, call helper function to do the tracking
	m_bAllowInvert = bAllowInvert;
	return TrackHandle(nHandle, pWnd, pDCClipTo);
}

BOOL CUITracker::TrackRubberBand(CWnd* pWnd, CPoint point, BOOL bAllowInvert)
{
	// simply call helper function to track from bottom right handle
	m_bAllowInvert = bAllowInvert;
	m_rect.SetRect(point.x, point.y, point.x, point.y);
	return TrackHandle(hitBottomRight, pWnd, NULL);
}

void CUITracker::DrawTrackerRect(LPCRECT lpRect, CDC* pDC)
{
	// first, normalize the rectangle for drawing
	CRect rect = *lpRect;
	rect.NormalizeRect();

	CSize size(0, 0);
	if (!m_bFinalErase)
	{
		// otherwise, size depends on the style
		if (m_nStyle & hatchedBorder)
		{
			size.cx = size.cy = max(1, GetHandleSize(rect)-1);
			rect.InflateRect(size);
		}
		else
		{
			size.cx = AFX_CX_BORDER;
			size.cy = AFX_CY_BORDER;
		}

		if(m_nStyle & (dottedLine|solidLine))
		{
			int offset=GetHandleSize(rect)/2;
			rect.InflateRect(offset,offset);
		}
	}

	pDC->LPtoDP(&rect);
	// and draw it
	if (m_bFinalErase || !m_bErase)
		pDC->DrawDragRect(rect, size, m_rectLast, m_sizeLast);

	// remember last rectangles
	m_rectLast = rect;
	m_sizeLast = size;
}

void CUITracker::AdjustRect(int nHandle, LPRECT)
{
	if (nHandle == hitMiddle)
		return;

	// convert the handle into locations within m_rect
	int *px, *py;
	GetModifyPointers(nHandle, &px, &py, NULL, NULL);

	// enforce minimum width
	int nNewWidth = m_rect.Width();
	int nAbsWidth = m_bAllowInvert ? abs(nNewWidth) : nNewWidth;
	if (px != NULL && nAbsWidth < m_sizeMin.cx)
	{
		nNewWidth = nAbsWidth != 0 ? nNewWidth / nAbsWidth : 1;
		ptrdiff_t iRectInfo = (int*)px - (int*)&m_rect;
		ENSURE(0 <= iRectInfo && iRectInfo < _countof(g_RectInfo));
		const RectInfo* pRectInfo = &g_RectInfo[iRectInfo];
		*px = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) +
			nNewWidth * m_sizeMin.cx * -pRectInfo->nSignAcross;
	}

	// enforce minimum height
	int nNewHeight = m_rect.Height();
	int nAbsHeight = m_bAllowInvert ? abs(nNewHeight) : nNewHeight;
	if (py != NULL && nAbsHeight < m_sizeMin.cy)
	{
		nNewHeight = nAbsHeight != 0 ? nNewHeight / nAbsHeight : 1;
		ptrdiff_t iRectInfo = (int*)py - (int*)&m_rect;
		ENSURE(0 <= iRectInfo && iRectInfo < _countof(g_RectInfo));
		const RectInfo* pRectInfo = &g_RectInfo[iRectInfo];
		*py = *(int*)((BYTE*)&m_rect + pRectInfo->nOffsetAcross) +
			nNewHeight * m_sizeMin.cy * -pRectInfo->nSignAcross;
	}
}

void CUITracker::GetTrueRect(LPRECT lpTrueRect) const
{
	ASSERT(AfxIsValidAddress(lpTrueRect, sizeof(RECT)));

	CRect rect = m_rect;
	rect.NormalizeRect();
	int nInflateBy = 0;
	if ((m_nStyle & (resizeOutside|hatchedBorder)) != 0)
		nInflateBy += GetHandleSize() - 1;
	if ((m_nStyle & (solidLine|dottedLine)) != 0)
		++nInflateBy;
	rect.InflateRect(nInflateBy, nInflateBy);
	*lpTrueRect = rect;
}

void CUITracker::OnChangedRect(const CRect& /*rectOld*/)
{
	// no default implementation, useful for derived classes
}

/////////////////////////////////////////////////////////////////////////////
// CUITracker implementation helpers

void CUITracker::GetHandleRect(int nHandle, CRect* pHandleRect) const
{
	ASSERT(nHandle < 8);

	// get normalized rectangle of the tracker
	CRect rectT = m_rect;
	rectT.NormalizeRect();
	if ((m_nStyle & (solidLine|dottedLine)) != 0)
		rectT.InflateRect(+1, +1);

	// since the rectangle itself was normalized, we also have to invert the
	//  resize handles.
	nHandle = NormalizeHit(nHandle);

	// handle case of resize handles outside the tracker
	int size = GetHandleSize();
	if (m_nStyle & resizeOutside)
		rectT.InflateRect(size-1, size-1);

	// calculate position of the resize handle
	int nWidth = rectT.Width();
	int nHeight = rectT.Height();
	CRect rect;
	const HandleInfo* pHandleInfo = &g_HandleInfo[nHandle];
	rect.left = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetX);
	rect.top = *(int*)((BYTE*)&rectT + pHandleInfo->nOffsetY);
	rect.left += size * pHandleInfo->nHandleX;
	rect.top += size * pHandleInfo->nHandleY;
	rect.left += pHandleInfo->nCenterX * (nWidth - size) / 2;
	rect.top += pHandleInfo->nCenterY * (nHeight - size) / 2;
	rect.right = rect.left + size;
	rect.bottom = rect.top + size;

	*pHandleRect = rect;
}

void CUITracker::GetMoveHandleRect(CRect* pHandleRect) const
{
	CRect rcTopLeft;
	CRect rcTop;
	GetHandleRect(hitTopLeft,&rcTopLeft);
	GetHandleRect(hitTop,&rcTop);

	CRect rect;
	int size=m_nMoveHandleSize/2;
	int offset=rcTopLeft.right+10+size;
	rect.left=offset>rcTop.left?rcTopLeft.right:offset;
	rect.left-=size;
	rect.right=rect.left+m_nMoveHandleSize;
	rect.top=rcTopLeft.bottom-size;
	rect.bottom=rect.top+m_nMoveHandleSize;

	*pHandleRect=rect;
}

int CUITracker::GetHandleSize(LPCRECT lpRect) const
{
	if (lpRect == NULL)
		lpRect = &m_rect;

	int size = m_nHandleSize;
	if (!(m_nStyle & resizeOutside))
	{
		// make sure size is small enough for the size of the rect
		int sizeMax = min(abs(lpRect->right - lpRect->left),
			abs(lpRect->bottom - lpRect->top));
		if (size * 2 > sizeMax)
			size = sizeMax / 2;
	}
	return size;
}

void CUITracker::SetControlType(int nType)
{
	m_nControlType=nType;

	if(m_nControlType==typeForm)
		m_nMask=0x64;
	else
		m_nMask=0xFF;
}

int CUITracker::HitTestHandles(CPoint point) const
{
	CRect rect;
	UINT mask = GetHandleMask();

	// see if hit anywhere inside the tracker
	GetTrueRect(&rect);
	if (!rect.PtInRect(point))
		return hitNothing;  // totally missed

	//see if we hit movehandle
	if(m_nControlType==typeContainer)
	{
		GetMoveHandleRect(&rect);
		if(rect.PtInRect(point))
			return hitMiddle;
	}

	// see if we hit a handle
	for (int i = 0; i < 8; ++i)
	{
		if (mask & (1<<i))
		{
			GetHandleRect((TrackerHit)i, &rect);
			if (rect.PtInRect(point))
				return (TrackerHit)i;
		}
	}

	// last of all, check for non-hit outside of object, between resize handles
	if ((m_nStyle & hatchedBorder) == 0)
	{
		CRect rect = m_rect;
		rect.NormalizeRect();
		if ((m_nStyle & dottedLine|solidLine) != 0)
			rect.InflateRect(+1, +1);
		if (!rect.PtInRect(point))
			return hitNothing;  // must have been between resize handles
	}
	return m_nControlType==typeControl?hitMiddle:hitNothing;   // no handle hit, but hit object (or object border)
}

BOOL CUITracker::TrackHandle(int nHandle, CWnd* pWnd, CDC* pDCClipTo)
{
	ASSERT(nHandle >= 0);
	ASSERT(nHandle <= 8);   // handle 8 is inside the rect

	// don't handle if capture already set
	if (::GetCapture() != NULL)
		return FALSE;

	AfxLockTempMaps();  // protect maps while looping

	ASSERT(!m_bFinalErase);

	// save original width & height in pixels
	int nWidth = m_rect.Width();
	int nHeight = m_rect.Height();

	// set capture to the window which received this message
	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());
	pWnd->UpdateWindow();
	CRect rectSave = m_rect;

	// convert cursor position to client co-ordinates
	CPoint point;
	GetCursorPos(&point);
	pWnd->ScreenToClient(&point);

	// find out what x/y coords we are supposed to modify
	int *px, *py;
	int xDiff, yDiff;
	GetModifyPointers(nHandle, &px, &py, &xDiff, &yDiff);
	xDiff = point.x - xDiff;
	yDiff = point.y - yDiff;

	// get DC for drawing
	CDC* pDrawDC;
	if (pDCClipTo != NULL)
	{
		pDrawDC = pDCClipTo;
	}
	else
	{
		// otherwise, just use normal DC
		pDrawDC = pWnd->GetDC();
	}
	ENSURE_VALID(pDrawDC);

	CRect rectOld;
	BOOL bMoved = FALSE;

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		switch (msg.message)
		{
			// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			rectOld = m_rect;
			// handle resize cases (and part of move)
			if (px != NULL)
				*px = GET_X_LPARAM(msg.lParam) - xDiff;
			if (py != NULL)
				*py = GET_Y_LPARAM(msg.lParam) - yDiff;

			// handle move case
			if (nHandle == hitMiddle)
			{
				m_rect.right = m_rect.left + nWidth;
				m_rect.bottom = m_rect.top + nHeight;
			}
			// allow caller to adjust the rectangle if necessary
			AdjustRect(nHandle, &m_rect);

			// only redraw and callback if the rect actually changed!
			m_bFinalErase = (msg.message == WM_LBUTTONUP);
			if (!rectOld.EqualRect(&m_rect) || m_bFinalErase)
			{
				if (bMoved)
				{
					m_bErase = TRUE;
					DrawTrackerRect(&rectOld, pDrawDC);
				}
				OnChangedRect(rectOld);
				if (msg.message != WM_LBUTTONUP)
					bMoved = TRUE;
			}
			if (m_bFinalErase)
				goto ExitLoop;

			if (!rectOld.EqualRect(&m_rect))
			{
				m_bErase = FALSE;
				DrawTrackerRect(&m_rect, pDrawDC);
			}
			break;

			// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;
		case WM_RBUTTONDOWN:
			if (bMoved)
			{
				m_bErase = m_bFinalErase = TRUE;
				DrawTrackerRect(&m_rect, pDrawDC);
			}
			m_rect = rectSave;
			goto ExitLoop;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	if(pDCClipTo==NULL)
		pWnd->ReleaseDC(pDrawDC);
	ReleaseCapture();

	AfxUnlockTempMaps(FALSE);

	// restore rect in case bMoved is still FALSE
	if (!bMoved)
		m_rect = rectSave;
	m_bFinalErase = FALSE;
	m_bErase = FALSE;

	// return TRUE only if rect has changed
	return !rectSave.EqualRect(&m_rect);
}

void CUITracker::GetModifyPointers(
									 int nHandle, int** ppx, int** ppy, int* px, int* py)
{
	ENSURE(nHandle >= 0);
	ENSURE(nHandle <= 8);

	if (nHandle == hitMiddle)
		nHandle = hitTopLeft;   // same as hitting top-left

	*ppx = NULL;
	*ppy = NULL;

	// fill in the part of the rect that this handle modifies
	//  (Note: handles that map to themselves along a given axis when that
	//   axis is inverted don't modify the value on that axis)

	const HandleInfo* pHandleInfo = &g_HandleInfo[nHandle];
	if (pHandleInfo->nInvertX != nHandle)
	{
		*ppx = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetX);
		if (px != NULL)
			*px = **ppx;
	}
	else
	{
		// middle handle on X axis
		if (px != NULL)
			*px = m_rect.left + abs(m_rect.Width()) / 2;
	}
	if (pHandleInfo->nInvertY != nHandle)
	{
		*ppy = (int*)((BYTE*)&m_rect + pHandleInfo->nOffsetY);
		if (py != NULL)
			*py = **ppy;
	}
	else
	{
		// middle handle on Y axis
		if (py != NULL)
			*py = m_rect.top + abs(m_rect.Height()) / 2;
	}
}

UINT CUITracker::GetHandleMask() const
{
	UINT mask = m_nMask;   // always have 4 corner handles
	int size = m_nHandleSize*3;
	if (abs(m_rect.Width()) - size < 4)
		mask &= (~0x50);
	if (abs(m_rect.Height()) - size < 4)
		mask &= (~0xA0);
	return mask;
}


/////////////////////////////////////////////////////////////////////////////
// CMultiUITracker

CTrackerElement::CTrackerElement(CControlUI* pControl,int nType,INotifyUI* pOwner)
:m_pControl(pControl),m_nType(nType),m_pOwner(pOwner)
{
}

CTrackerElement::~CTrackerElement(void)
{

}

const RECT& CTrackerElement::GetPos() const 
{
	ASSERT(m_pControl);

	return m_pControl->GetPos();
}

void CTrackerElement::SetPos(RECT rect,BOOL bMove/*=FALSE*/)
{
	ASSERT(m_pControl);

	CControlUI* pParent=m_pControl->GetParent();
	CRect rcParent(0,0,0,0);
	if(pParent)
		rcParent=pParent->GetPos();

	m_pControl->SetFixedXY(CSize(rect.left-rcParent.left,rect.top-rcParent.top));
	if(!bMove)
	{
		m_pControl->SetFixedHeight(rect.bottom-rect.top);
		m_pControl->SetFixedWidth(rect.right-rect.left);

		TNotifyUI Msg;
		Msg.pSender=m_pControl;
		Msg.sType=_T("setpos");
		Msg.wParam=FALSE;//Not Move
		Msg.lParam=NULL;
		m_pOwner->Notify(Msg);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMultiUITracker

CMultiUITracker::CMultiUITracker(void):m_pFocused(NULL)
{
	m_szForm.cx=0;
	m_szForm.cy=0;

	HINSTANCE hInst = AfxFindResourceHandle(
		ATL_MAKEINTRESOURCE(AFX_IDC_TRACK4WAY), ATL_RT_GROUP_CURSOR);
	m_hNoDropCursor=::LoadCursor(hInst, ATL_MAKEINTRESOURCE(AFX_IDC_NODROPCRSR));
}

CMultiUITracker::~CMultiUITracker(void)
{
	RemoveAll();
	ClearUIRect();
}

void CMultiUITracker::Draw(CDC* pDC,LPSIZE pOffset/*=NULL*/)
{
	HPEN hOldPen=m_hHandlePen;
	HPEN hWhitePen=CreatePen(PS_SOLID,1,RGB(255,255,255));
	HBRUSH hOldBrush=m_hHandleBrush;
	HBRUSH hBlackBrush=CreateSolidBrush(RGB(0,0,0));

	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		m_rect=pArrTracker->GetPos();
		if(pOffset)
			m_rect.OffsetRect(*pOffset);
		SetControlType(pArrTracker->m_nType);
		if(pArrTracker==m_pFocused)
		{
			m_hHandlePen=hOldPen;
			m_hHandleBrush=hOldBrush;
		}
		else
		{
			m_hHandlePen=hWhitePen;
			m_hHandleBrush=hBlackBrush;
		}

		CUITracker::Draw(pDC);
	}

	m_hHandlePen=hOldPen;
	m_hHandleBrush=hOldBrush;
	::DeleteObject(hWhitePen);
	::DeleteObject(hBlackBrush);
}

int CMultiUITracker::HitTest(CPoint point)
{
	int nHit=hitNothing;
	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		m_rect=pArrTracker->GetPos();
		SetControlType(pArrTracker->m_nType);
		nHit=CUITracker::HitTest(point);
		if (nHit!=hitNothing)
		{
			return nHit;
		}
	}

	return hitNothing;
}

int CMultiUITracker::HitTestHandles(CPoint point)
{
	int nHit=hitNothing;
	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		m_rect=pArrTracker->GetPos();
		SetControlType(pArrTracker->m_nType);
		nHit=CUITracker::HitTestHandles(point);
		if (nHit!=hitNothing)
		{
			return nHit;
		}
	}
	return hitNothing;
}

BOOL CMultiUITracker::Track( CWnd* pWnd, CPoint point,BOOL bAllowInvert/*=FALSE*/,CDC* pDCClipTo/*=NULL*/)
{
	if (IsEmpty())
		return FALSE;

	BOOL bRet=FALSE;
	int nHandle=HitTestHandles(point);
	switch(nHandle)
	{
	case hitNothing:
		break;
	case hitMiddle:
		bRet = MultiTrackHandle(pWnd, pDCClipTo);
		break;
	default:
		bRet = OneTrackHandle(nHandle, pWnd, bAllowInvert, pDCClipTo);
		break;
	}

	return bRet;
}

BOOL CMultiUITracker::SetCursor(CPoint point, UINT nHitTest)
{
	CRect rectSave = m_rect;
	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		m_rect=pArrTracker->GetPos();
		SetControlType(pArrTracker->m_nType);
		if (CUITracker::SetCursor(point, nHitTest))
		{
			m_rect = rectSave;
			return TRUE;
		}
	}
	m_rect = rectSave;
	return FALSE;
}

BOOL CMultiUITracker::MultiTrackHandle(CWnd* pWnd,CDC* pDCClipTo)
{
	if (::GetCapture() != NULL)
		return FALSE;

	AfxLockTempMaps();  // protect maps while looping

	ASSERT(!m_bFinalErase);

	// set capture to the window which received this message
	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());
	pWnd->UpdateWindow();

	// convert cursor position to client co-ordinates
	CPoint oldPoint;
	GetCursorPos(&oldPoint);
	pWnd->ScreenToClient(&oldPoint);

	// We work on the rects copies
	CopyUIRect();

	// get DC for drawing
	CDC* pDrawDC;
	if (pDCClipTo != NULL)
	{
		pDrawDC = pDCClipTo;
	}
	else
	{
		// otherwise, just use normal DC
		pDrawDC = pWnd->GetDC();
	}
	ASSERT_VALID(pDrawDC);

	CRect rectOld;
	CPoint point;
	HCURSOR hOldCursor=::GetCursor();
	BOOL bBeyond=FALSE;
	BOOL bMoved = FALSE;

	// get messages until capture lost or cancelled/accepted
	for (;;)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		//dispatch the message to window
		DispatchMessage(&msg);

		switch (msg.message)
		{
			// handle movement/accept messages
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:

			// only redraw and callback if the rect actually changed!
			m_bFinalErase = (msg.message == WM_LBUTTONUP);

			point=msg.lParam;
			pDrawDC->DPtoLP(&point);
			if(point.x<0||point.x>m_szForm.cx||
				point.y<0||point.y>m_szForm.cy)
			{
				bBeyond=TRUE;
				::SetCursor(m_hNoDropCursor);
			}
			else
			{
				bBeyond=FALSE;
				::SetCursor(hOldCursor);
			}
			point=msg.lParam;

			for (int i=0;i<m_arrCloneRect.GetSize();i++)
			{
				//must have the same father and be float
				CTrackerElement* pArrTracker=m_arrTracker.GetAt(i);
				if(pArrTracker->m_pControl->GetParent()!=m_pFocused->m_pControl->GetParent()||
						!pArrTracker->m_pControl->IsFloat())
					continue;

				m_rect=m_arrCloneRect.GetAt(i);
				rectOld = m_rect;

				m_rect.left += point.x - oldPoint.x;
				m_rect.top += point.y - oldPoint.y;
				m_rect.right = m_rect.left + rectOld.Width();
				m_rect.bottom = m_rect.top + rectOld.Height();

				if (!rectOld.EqualRect(&m_rect) || m_bFinalErase)
				{
					if (bMoved)
					{
						m_bErase = TRUE;
						DrawTrackerRect(&rectOld, pDrawDC);
					}
					if (!m_bFinalErase)
						bMoved = TRUE;
				}
				if (!rectOld.EqualRect(&m_rect) && !m_bFinalErase)
				{
					m_bErase = FALSE;
					DrawTrackerRect(&m_rect, pDrawDC);
				}
				m_arrCloneRect.SetAt(i,m_rect);
			}
			oldPoint = msg.lParam;
			if (m_bFinalErase)
				goto ExitLoop;

			break;

			// handle cancel messages
		case WM_KEYDOWN:
			if (msg.wParam != VK_ESCAPE)
				break;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

ExitLoop:
	if(pDCClipTo==NULL)
		pWnd->ReleaseDC(pDrawDC);
	ReleaseCapture();

	AfxUnlockTempMaps(FALSE);

	// update control's rect pos in case bMoved is TRUE
	if (bMoved&&!bBeyond)
		UpdateUIRect();
	ClearUIRect();

	m_bFinalErase = FALSE;
	m_bErase = FALSE;

	// return TRUE only if rect has changed
	return bMoved;
}

BOOL CMultiUITracker::OneTrackHandle(int nHandle, CWnd* pWnd, BOOL bAllowInvert, CDC* pDCClipTo)
{
	if(m_arrTracker.GetSize()>1)
		return FALSE;

	CControlUI* pControl = m_pFocused->m_pControl;
	m_rect = m_pFocused->GetPos();
	m_bAllowInvert = bAllowInvert;
	BOOL bRet = TrackHandle(nHandle, pWnd, pDCClipTo);
	if(bRet)
	{
		CString strVal;
		strVal.Format(_T("%d,%d,%d,%d"),pControl->GetFixedXY().cx, pControl->GetFixedXY().cy
			, pControl->GetFixedXY().cx + pControl->GetFixedWidth(), pControl->GetFixedXY().cy + pControl->GetFixedHeight());
		UIAttribute oldAttrib = {_T("pos"), strVal};
		TNotifyUI Msg;
		Msg.pSender = pControl;
		Msg.sType = _T("PropertyBeginChanged");
		Msg.wParam = 0;
		Msg.lParam = (LPARAM)&oldAttrib;
		m_pFocused->m_pOwner->Notify(Msg);

		m_pFocused->SetPos(m_rect);

		strVal.Format(_T("%d,%d,%d,%d"),pControl->GetFixedXY().cx, pControl->GetFixedXY().cy
			, pControl->GetFixedXY().cx + pControl->GetFixedWidth(), pControl->GetFixedXY().cy + pControl->GetFixedHeight());
		UIAttribute newAttrib = {_T("pos"), strVal};
		Msg.sType = _T("PropertyEndChanged");
		Msg.lParam = (LPARAM)&newAttrib;
		m_pFocused->m_pOwner->Notify(Msg);
	}

	return bRet;
}

void CMultiUITracker::CopyUIRect()
{
	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		m_arrCloneRect.Add(m_arrTracker.GetAt(i)->GetPos());
	}
}

void CMultiUITracker::ClearUIRect()
{
	m_arrCloneRect.RemoveAll();
}

void CMultiUITracker::UpdateUIRect()
{
	CArray<CControlUI*,CControlUI*> arrSelected;
	for(int i=0; i<m_arrTracker.GetSize(); i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);
		if(pArrTracker->m_pControl->GetParent() == m_pFocused->m_pControl->GetParent())
			arrSelected.Add(pArrTracker->m_pControl);
	}
	TNotifyUI Msg;
	Msg.pSender=m_pFocused->m_pControl;
	Msg.sType=_T("PosBeginChanged");
	Msg.wParam=0;
	Msg.lParam=(LPARAM)&arrSelected;
	m_pFocused->m_pOwner->Notify(Msg);

	for(int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker=m_arrTracker.GetAt(i);
		if(pArrTracker->m_pControl->GetParent()!=m_pFocused->m_pControl->GetParent())
			continue;

		pArrTracker->SetPos(m_arrCloneRect.GetAt(i),TRUE);
	}

	Msg.sType=_T("PosEndChanged");
	Msg.wParam=0;
	Msg.lParam=NULL;
	m_pFocused->m_pOwner->Notify(Msg);

	Msg.sType=_T("setpos");
	Msg.wParam=TRUE;//Move
	Msg.lParam=NULL;
	m_pFocused->m_pOwner->Notify(Msg);
}

BOOL CMultiUITracker::Add(CTrackerElement * pTracker)
{
	ASSERT(pTracker);

	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		if (pArrTracker->m_pControl == pTracker->m_pControl)
		{
			m_pFocused=pArrTracker;
			return FALSE;
		}
	}
	m_pFocused=pTracker;
	m_arrTracker.Add(pTracker);

	return TRUE;
}

BOOL CMultiUITracker::Remove(CTrackerElement * pTracker)
{
	ASSERT(pTracker);

	int len=m_arrTracker.GetSize();
	for (int i=0;i<len;i++)
	{
		int j = i;
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);
		if (pArrTracker == pTracker)
		{
			m_arrTracker.RemoveAt(j);
			if (m_pFocused == pTracker)
			{
				m_pFocused = m_arrTracker.GetAt(m_arrTracker.GetSize()-1);
			}
			delete pTracker;

			return TRUE;
		}
	}

	return FALSE;
}

void CMultiUITracker::RemoveAll()
{
	int len=m_arrTracker.GetSize();
	for (int i=0;i<len;i++)
	{
		delete m_arrTracker.GetAt(i);
	}
	m_arrTracker.RemoveAll();

	m_pFocused = NULL;
}

BOOL CMultiUITracker::SetFocus(CPoint point)
{
	for (int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		m_rect=pArrTracker->GetPos();
		SetControlType(pArrTracker->m_nType);
		if(CUITracker::HitTest(point)==hitMiddle)
		{
			m_pFocused=pArrTracker;

			return TRUE;
		}
	}

	return FALSE;
}

CControlUI* CMultiUITracker::GetFocused() const
{
	if(m_pFocused==NULL)
		return NULL;

	return m_pFocused->m_pControl;
}

BOOL CMultiUITracker::GetSelected(CArray<CControlUI*,CControlUI*>& arrSelected)
{
	if(IsEmpty())
		return FALSE;

	for(int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		arrSelected.Add(pArrTracker->m_pControl);
	}
	ExcludeChildren(arrSelected);

	return TRUE;
}

BOOL CMultiUITracker::SetFocus(CControlUI* pControl)
{
	m_pFocused=FindTracker(pControl);

	return m_pFocused?TRUE:FALSE;
}

CTrackerElement* CMultiUITracker::FindTracker(CControlUI* pControl) const
{
	for(int i=0;i<m_arrTracker.GetSize();i++)
	{
		CTrackerElement* pArrTracker = m_arrTracker.GetAt(i);

		if(pArrTracker->m_pControl==pControl)
			return pArrTracker;
	}

	return NULL;
}

void CMultiUITracker::ExcludeChildren(CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int size = arrSelected.GetSize();
	int* pDepth = new int[size];
	for(int i=0; i<size; i++)
	{
		ExtendedAttributes* pExtended = (ExtendedAttributes*)arrSelected[i]->GetTag();
		pDepth[i] = pExtended->nDepth;
	}

	for(int i=0; i<arrSelected.GetSize()-1; i++)
	{
		CControlUI* pControl1 = arrSelected[i];
		for(int j=i+1; j<arrSelected.GetSize(); j++)
		{
			if(pDepth[i] != pDepth[j])
			{
				CControlUI* pControl2 = arrSelected[j];
				if(pDepth[i] < pDepth[j])
				{
					int depth = pDepth[j] - pDepth[i];
					while(depth-- && pControl2)
						pControl2 = pControl2->GetParent();
					if(pControl1 == pControl2)
						arrSelected.RemoveAt(j--);
				}
				else
				{
					int depth = pDepth[i] - pDepth[j];
					while(depth-- && pControl1)
						pControl1 = pControl1->GetParent();
					if(pControl1 == pControl2)
						arrSelected.RemoveAt(i--);
				}
			}
		}
	}

	delete[] pDepth;
}