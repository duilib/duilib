#include "stdafx.h"
#include "GDIRender.h"


CGDIRender::CGDIRender(void)
{
}


CGDIRender::~CGDIRender(void)
{
}

void CGDIRender::DrawImage(ImageObject* pImageObj, int nAlpha, int x, int y, int nIndex /*= 0*/, bool bHole /*=false*/)
{
	HDC hDC = GetPaintDC();

	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	CDuiRect rc;
	rc.right = 100;
	rc.bottom = 100;
	DWORD dwPenColor = 0xFF00FF00;
	LOGPEN lg;
	lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
	lg.lopnStyle = PS_SOLID;
	lg.lopnWidth.x = 2;
	HPEN hPen = CreatePenIndirect(&lg);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	POINT ptTemp = { 0 };
	::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
	::LineTo(hDC, rc.right, rc.bottom);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

void CGDIRender::DrawImage(ImageObject* pImageObj, int nAlpha, RECT& rect, int nIndex /*= 0*/, bool bHole /*=false*/)
{
	
}

void CGDIRender::DrawImage(ImageObject* pImageObj, RECT& rcSrc, RECT& rcDest, int nIndex /*= 0*/)
{
	
}

void CGDIRender::DrawColor(LPCRECT rcSrc,DWORD dwColor)
{
	if( dwColor <= 0x00FFFFFF )
		return;
	if( dwColor >= 0xFF000000 )
	{
		
		::SetBkColor(m_pMemDC->GetSafeHdc(), RGB(GetRValue(dwColor),GetGValue(dwColor),GetBValue(dwColor)));
		::ExtTextOut(m_pMemDC->GetSafeHdc(), 0, 0, ETO_OPAQUE, rcSrc, NULL, 0, NULL);
	}
}
