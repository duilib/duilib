#include "stdafx.h"
#include "GDIRender.h"

CGDIRender::CGDIRender(void)
{
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

void CGDIRender::DrawGradient(LPCRECT rcSrc,DWORD dwStart,DWORD dwEnd,bool bVertical)
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

void CGDIRender::DrawText(LPCRECT rcSrc,LPCTSTR lpszTextString)
{
	
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

	typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
	static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");
	if ( lpGradientFill != NULL)
	{
		if(!bVertical)
		{
			lpGradientFill(GetPaintDC(),vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
		}
		else
		{
			lpGradientFill(GetPaintDC(),vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
		}	
	}
}
