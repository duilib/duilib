#include "stdafx.h"
#include "IUIRender.h"

IUIRender::IUIRender()
	: m_pMemDC(NULL)
	, m_hDC(NULL)
{
}

void IUIRender::SetDevice(CMemDC *memDC)
{
	m_pMemDC = memDC;
	m_hDC = NULL;
}

void IUIRender::SetDevice(HDC hDc)
{
	m_hDC = hDc;
	m_pMemDC = NULL;
}

HDC IUIRender::GetPaintDC()
{
	if ( m_hDC != NULL && m_pMemDC == NULL)
	{
		return m_hDC;
	}
	if ( m_pMemDC )
	{
		if ( m_pMemDC->IsValid() )
		{
			return m_pMemDC->GetSafeHdc();
		}
	}

	return NULL;
}

void IUIRender::SetInvalidateRect(RECT& rect)
{
	m_rcInvalidate = rect;
}

RECT IUIRender::GetInvalidateRect()
{
	return m_rcInvalidate;
}

void IUIRender::DrawImage(ImageObject* pImageObj,LPCRECT pRcControl,LPCRECT pRcPaint)
{

}

IUIRender::~IUIRender()
{
	m_pMemDC = NULL;
}

void IUIRender::DrawColor(LPCRECT rcSrc,DWORD dwColor)
{

}

void IUIRender::DrawGradient(LPCRECT rcSrc,DWORD dwStart,DWORD dwEnd,bool bVertical,int nStep)
{

}

void IUIRender::DrawRoundRectBorder(LPCRECT rcSrc, INT nCornerWidth, INT nCornerHeight, INT nBorderWidth, DWORD dwColor)
{

}

void IUIRender::DrawRectangleBorder(LPCRECT rcSrc,LPCRECT rcBorder,DWORD dwColor)
{

}

void IUIRender::DrawText(FontObject* pFontObj,LPCRECT rcSrc,LPCTSTR lpszTextString,DWORD dwTextColor,DWORD dwStyle)
{

}

//////////////////////////////////////////////////////////////////////////

CRenderClip::CRenderClip()
	: hDC(NULL)
	, hRgn(NULL)
	, hOldRgn(NULL)
{
	ZeroMemory(&rcItem,sizeof(RECT));
}

CRenderClip::~CRenderClip()
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
	ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
	::SelectClipRgn(hDC, hOldRgn);
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);
}

void CRenderClip::GenerateClip(HDC hDC, RECT rc, CRenderClip& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
}

void CRenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip)
{
	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	clip.hRgn = ::CreateRectRgnIndirect(&rc);
	HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
	::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
	::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
	clip.hDC = hDC;
	clip.rcItem = rc;
	::DeleteObject(hRgnItem);
}

void CRenderClip::UseOldClipBegin(HDC hDC, CRenderClip& clip)
{
	::SelectClipRgn(hDC, clip.hOldRgn);
}

void CRenderClip::UseOldClipEnd(HDC hDC, CRenderClip& clip)
{
	::SelectClipRgn(hDC, clip.hRgn);
}

//////////////////////////////////////////////////////////////////////////
// CMemDC
CMemDC::CMemDC()
	: m_hMemDC(NULL)
	, m_hMemBmp(NULL)
	, m_hOldBmp(NULL)
	, m_cx(NULL)
	, m_cy(NULL)
{

}

CMemDC::~CMemDC()
{
	Clear();
}

void CMemDC::Create(HDC hDC,int cx,int cy)
{
	if ( m_hMemDC )
		this->Clear();

	// Create Memory Device Context
	m_hMemDC = ::CreateCompatibleDC(hDC);

	// DDB Create Buffer for Memory DC
	m_hMemBmp = ::CreateCompatibleBitmap (hDC,cx,cy);
	ASSERT(m_hMemDC);
	ASSERT(m_hMemBmp);

	m_hOldBmp = (HBITMAP)::SelectObject(m_hMemDC,m_hMemBmp);
	m_cx = cx;
	m_cy = cy;
}

void CMemDC::Clear()
{
	if ( m_hOldBmp )
	{
		::SelectObject(m_hMemDC,m_hOldBmp);
		m_hOldBmp = NULL;
	}
	if ( m_hMemBmp)
	{
		::DeleteObject(m_hMemBmp);
		m_hMemBmp = NULL;
	}
	if ( m_hMemDC)
	{
		::DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}
	m_cx = 0;
	m_cy = 0;
}

int CMemDC::GetHeight()
{
	return m_cy;
}

int CMemDC::GetWidth()
{
	return m_cx;
}

HBITMAP CMemDC::GetHBitmap()
{
	return m_hMemBmp;
}

HDC CMemDC::GetSafeHdc()
{
	return m_hMemDC;
}

BOOL CMemDC::IsValid()
{
	return m_hMemDC != NULL;
}
