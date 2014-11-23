#include "stdafx.h"
#include "GDIRender.h"

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC,
							   int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn);
static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken);

CGDIRender::CGDIRender(void)
{
	m_lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
	m_lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");
	if( m_lpAlphaBlend == NULL )
		m_lpAlphaBlend = AlphaBitBlt;
}


CGDIRender::~CGDIRender(void)
{
}

void CGDIRender::DrawColor(LPCRECT rcSrc,DWORD dwColor)
{
	if( dwColor <= 0x00FFFFFF )
		return;
	if( dwColor >= 0xFF000000 )
	{
		
		::SetBkColor(GetPaintDC(), RGB(GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor)));
		::ExtTextOut(GetPaintDC(), 0, 0, ETO_OPAQUE, rcSrc, NULL, 0, NULL);
	}
}

void CGDIRender::DrawGradient(LPCRECT rcSrc,DWORD dwStart,DWORD dwEnd,bool bVertical,int nStep)
{
	CDuiRect rcPaint(rcSrc);
	if ( rcPaint.IsRectEmpty())
		return ;

	GradientFill(rcSrc,dwStart,dwEnd,bVertical);
}

void CGDIRender::DrawRoundRectBorder(LPCRECT rcSrc, INT nCornerWidth, INT nCornerHeight, INT nBorderWidth, DWORD dwColor)
{
	CDuiRect rcPaint(rcSrc);
	if ( rcPaint.IsRectEmpty() || nBorderWidth <=0 )
	{
		ASSERT(FALSE);
		return;
	}

	HPEN hNewPen = CreatePen(PS_SOLID|PS_INSIDEFRAME, nBorderWidth, dwColor);
	HPEN hOldPen = (HPEN)SelectObject(this->GetPaintDC(), hNewPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(this->GetPaintDC(), GetStockObject(NULL_BRUSH));
	::RoundRect(this->GetPaintDC(), rcPaint.left, rcPaint.top, rcPaint.right-1, rcPaint.bottom-1, nCornerWidth, nCornerHeight);
	::SelectObject(this->GetPaintDC(), hOldBrush);
	::SelectObject(this->GetPaintDC(), hOldPen);
	::DeleteObject(hNewPen);
}

void CGDIRender::DrawRectangleBorder(LPCRECT rcSrc,LPCRECT rcBorder,DWORD dwColor)
{
	CDuiRect rcPaint(rcSrc);
	if ( rcPaint.IsNull() )
	{
		ASSERT(FALSE);
		return ;
	}

	if ( ( rcBorder->left + rcBorder->right )>= rcPaint.GetWidth() 
		|| ( rcBorder->top + rcBorder->bottom ) >= rcPaint.GetHeight() )
	{
		this->DrawColor(rcSrc,dwColor);
		return;
	}

	CDuiRect rcLeft(rcSrc);
	rcLeft.top = rcLeft.top + rcBorder->top;
	rcLeft.bottom = rcSrc->bottom - rcBorder->bottom	;
	rcLeft.right = rcLeft.left + rcBorder->left;

	CDuiRect rcRight(rcLeft);
	rcRight.right = rcSrc->right;
	rcRight.left = rcRight.right - rcBorder->right;

	CDuiRect rcTop(rcSrc);
	rcTop.bottom = rcSrc->top + rcBorder->top;

	CDuiRect rcBottom(rcSrc);
	rcBottom.top = rcBottom.bottom - rcBorder->bottom;

	this->DrawColor(&rcLeft,dwColor);
	this->DrawColor(&rcRight,dwColor);
	this->DrawColor(&rcTop,dwColor);
	this->DrawColor(&rcBottom,dwColor);
}

void CGDIRender::DrawText(FontObject* pFontObj,LPCRECT rcSrc,LPCTSTR lpszTextString,DWORD dwTextColor,DWORD dwStyle)
{
	ASSERT(::GetObjectType(GetPaintDC())==OBJ_DC || ::GetObjectType(GetPaintDC())==OBJ_MEMDC);

	::SetBkMode(GetPaintDC(), TRANSPARENT);
	
	::SetTextColor(GetPaintDC(), RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
	HFONT hOldFont = (HFONT)::SelectObject(GetPaintDC(), pFontObj->GetFont());
	::DrawText(GetPaintDC(), lpszTextString, -1, (LPRECT)rcSrc, dwStyle);
	::SelectObject(GetPaintDC(), hOldFont);
}

VOID CGDIRender::GradientFill(LPCRECT rcSrc, DWORD dwStart, DWORD dwEnd, bool bVertical)
{
	TRIVERTEX vert[2] ;
	GRADIENT_RECT    gRect;

	vert [0] .x      = rcSrc->left;
	vert [0] .y      = rcSrc->top;
	vert [0] .Red    = GetRValue(dwStart) << 8;
	vert [0] .Green  = GetGValue(dwStart) << 8;
	vert [0] .Blue   = GetBValue(dwStart) << 8;
	vert [0] .Alpha  = 0xFF00;

	vert [1] .x      = rcSrc->right;
	vert [1] .y      = rcSrc->bottom; 
	vert [1] .Red    = GetRValue(dwEnd) << 8;
	vert [1] .Green  = GetGValue(dwEnd) << 8;
	vert [1] .Blue   = GetBValue(dwEnd) << 8;
	vert [1] .Alpha  = 0xFF00;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	if ( m_lpGradientFill != NULL)
	{
		if(!bVertical)
		{
			m_lpGradientFill(GetPaintDC(),vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
		}
		else
		{
			m_lpGradientFill(GetPaintDC(),vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
		}	
	}
}

void CGDIRender::DrawImage(ImageObject* pImageObj,LPCRECT pRcControl,LPCRECT pRcPaint)
{
	HDC hDC = GetPaintDC();
	HBITMAP hBitmap = pImageObj->GetHBitmap();
	RECT rcCorners = pImageObj->Get9Gird();
	bool alphaChannel =pImageObj->IsAlphaImage();
	BYTE uFade =pImageObj->GetAlpha();
	bool hole = pImageObj->GetHole();
	bool xtiled = pImageObj->GetXTiled();
	bool ytiled = pImageObj->GetYTiled();


	RECT rc(*pRcControl);
	RECT rcPaint( *pRcPaint);
	CDuiRect rcBmpPart =pImageObj->GetSource();

	CDuiRect rcDest(pImageObj->GetDest());
	if ( !rcDest.IsNull())
	{
		rc.left = pRcControl->left + rcDest.left;
		rc.top = pRcControl->top + rcDest.top;
		rc.right = pRcControl->right + rcDest.right;
		rc.bottom = pRcControl->bottom + rcDest.bottom;

		if ( rc.right > pRcControl->right)
			rc.right = pRcControl->right;
		if ( rc.bottom > pRcControl->bottom)
			rc.bottom = pRcControl->bottom;
	}

	int imageWidht =  pImageObj->GetImageWidth();
	int imageHeight = pImageObj->GetImageHeight();
	if ( rcBmpPart.IsNull())
	{
		rcBmpPart.right = imageWidht;
		rcBmpPart.bottom = imageHeight;
	}

	if ( rcBmpPart.right > imageWidht )
		rcBmpPart.right = imageWidht;
	if ( rcBmpPart.bottom > imageHeight)
		rcBmpPart.bottom = imageHeight;

	CDuiRect rcTemp;
	if ( !rcTemp.IntersectRect(&rc,pRcControl))
		return;
	if ( !rcTemp.IntersectRect(&rc,pRcPaint))
		return;

	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	if( hBitmap == NULL )
		return;

	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
	::SetStretchBltMode(hDC, HALFTONE);

	rcTemp.Empty();
	rcDest.Empty();
	if( m_lpAlphaBlend && (alphaChannel || uFade < 255) )
	{
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
		// middle
		if( !hole )
		{
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				if( !xtiled && !ytiled )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
				else if( xtiled && ytiled )
				{
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int j = 0; j < iTimesY; ++j )
					{
						LONG lDestTop = rcDest.top + lHeight * j;
						LONG lDestBottom = rcDest.top + lHeight * (j + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom )
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						for( int i = 0; i < iTimesX; ++i )
						{
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right )
							{
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							m_lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
								lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
								rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
						}
					}
				}
				else if( xtiled )
				{
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					for( int i = 0; i < iTimes; ++i )
					{
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if( lDestRight > rcDest.right )
						{
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}
						m_lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
							hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
					}
				}
				else
				{ // ytiled
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int i = 0; i < iTimes; ++i )
					{
						LONG lDestTop = rcDest.top + lHeight * i;
						LONG lDestBottom = rcDest.top + lHeight * (i + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom )
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						m_lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
							hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
					}
				}
			}
		}

		// left-top
		if( rcCorners.left > 0 && rcCorners.top > 0 )
		{
			rcDest.left = rc.left;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
			}
		}
		// top
		if( rcCorners.top > 0 )
		{
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
					rcCorners.left - rcCorners.right, rcCorners.top, bf);
			}
		}
		// right-top
		if( rcCorners.right > 0 && rcCorners.top > 0 )
		{
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
			}
		}
		// left
		if( rcCorners.left > 0 )
		{
			rcDest.left = rc.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
					rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// right
		if( rcCorners.right > 0 )
		{
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
					rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
			}
		}
		// left-bottom
		if( rcCorners.left > 0 && rcCorners.bottom > 0 )
		{
			rcDest.left = rc.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
			}
		}
		// bottom
		if( rcCorners.bottom > 0 )
		{
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
					rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
			}
		}
		// right-bottom
		if( rcCorners.right > 0 && rcCorners.bottom > 0 )
		{
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				m_lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
					rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
					rcCorners.bottom, bf);
			}
		}
	}
	else 
	{
		if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
			&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
			&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
		{
			if( ::IntersectRect(&rcTemp, &rcPaint, &rc) )
			{
				::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
					hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
			}
		}
		else
		{
			// middle
			if( !hole )
			{
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					if( !xtiled && !ytiled )
					{
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
					else if( xtiled && ytiled )
					{
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int j = 0; j < iTimesY; ++j )
						{
							LONG lDestTop = rcDest.top + lHeight * j;
							LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom )
							{
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							for( int i = 0; i < iTimesX; ++i )
							{
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right )
								{
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
									lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
									rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
							}
						}
					}
					else if( xtiled )
					{
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						for( int i = 0; i < iTimes; ++i )
						{
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right )
							{
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
						}
					}
					else
					{ // ytiled
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int i = 0; i < iTimes; ++i )
						{
							LONG lDestTop = rcDest.top + lHeight * i;
							LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom )
							{
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
						}
					}
				}
			}

			// left-top
			if( rcCorners.left > 0 && rcCorners.top > 0 )
			{
				rcDest.left = rc.left;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
				}
			}
			// top
			if( rcCorners.top > 0 )
			{
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
						rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
				}
			}
			// right-top
			if( rcCorners.right > 0 && rcCorners.top > 0 )
			{
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
				}
			}
			// left
			if( rcCorners.left > 0 )
			{
				rcDest.left = rc.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
						rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
				}
			}
			// right
			if( rcCorners.right > 0 )
			{
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
				}
			}
			// left-bottom
			if( rcCorners.left > 0 && rcCorners.bottom > 0 )
			{
				rcDest.left = rc.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
				}
			}
			// bottom
			if( rcCorners.bottom > 0 )
			{
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
				}
			}
			// right-bottom
			if( rcCorners.right > 0 && rcCorners.bottom > 0 )
			{
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
				{
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
						rcCorners.bottom, SRCCOPY);
				}
			}
		}
	}

	::SelectObject(hCloneDC, hOldBitmap);
	::DeleteDC(hCloneDC);
}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC,
							   int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
	HDC hTempDC = ::CreateCompatibleDC(hDC);
	if (NULL == hTempDC)
		return FALSE;

	//Creates Source DIB
	LPBITMAPINFO lpbiSrc = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL)
	{
		::DeleteDC(hTempDC);
		return FALSE;
	}
	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = dwWidth;
	lpbiSrc->bmiHeader.biHeight = dwHeight;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biClrUsed = 0;
	lpbiSrc->bmiHeader.biClrImportant = 0;

	COLORREF* pSrcBits = NULL;
	HBITMAP hSrcDib = CreateDIBSection (
		hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
		NULL, NULL);

	if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
	{
		delete [] lpbiSrc;
		::DeleteDC(hTempDC);
		return FALSE;
	}

	HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
	::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	//Creates Destination DIB
	LPBITMAPINFO lpbiDest = NULL;
	// Fill in the BITMAPINFOHEADER
	lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiDest == NULL)
	{
		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);
		::DeleteDC(hTempDC);
		return FALSE;
	}

	lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiDest->bmiHeader.biWidth = dwWidth;
	lpbiDest->bmiHeader.biHeight = dwHeight;
	lpbiDest->bmiHeader.biPlanes = 1;
	lpbiDest->bmiHeader.biBitCount = 32;
	lpbiDest->bmiHeader.biCompression = BI_RGB;
	lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
	lpbiDest->bmiHeader.biXPelsPerMeter = 0;
	lpbiDest->bmiHeader.biYPelsPerMeter = 0;
	lpbiDest->bmiHeader.biClrUsed = 0;
	lpbiDest->bmiHeader.biClrImportant = 0;

	COLORREF* pDestBits = NULL;
	HBITMAP hDestDib = CreateDIBSection (
		hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
		NULL, NULL);

	if ((NULL == hDestDib) || (NULL == pDestBits))
	{
		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);
		::DeleteDC(hTempDC);
		return FALSE;
	}

	::SelectObject (hTempDC, hDestDib);
	::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	double src_darken;
	BYTE nAlpha;

	for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
	{
		nAlpha = LOBYTE(*pSrcBits >> 24);
		src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
		if( src_darken < 0.0 ) src_darken = 0.0;
		*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
	} //for

	::SelectObject (hTempDC, hDestDib);
	::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
	::SelectObject (hTempDC, hOldTempBmp);

	delete [] lpbiDest;
	::DeleteObject(hDestDib);

	delete [] lpbiSrc;
	::DeleteObject(hSrcDib);

	::DeleteDC(hTempDC);
	return TRUE;
}

static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
		GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
		GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

}

