#include "StdAfx.h"
#include "Util.h"

HBITMAP CreateDesktopBitmap(HWND hWnd)
{
	HWND hWndDesktop = ::GetDesktopWindow();
	RECT rect;
	GetWindowRect(hWndDesktop, &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.left = rect.top = 0;

	HDC hDcDesktop = GetDC(hWndDesktop);
	HDC hDcMem = CreateCompatibleDC(hDcDesktop);
	HBITMAP hBmp = CreateCompatibleBitmap(hDcDesktop, rect.right, rect.bottom);
	HGDIOBJ hOld = SelectObject(hDcMem, hBmp);
	::BitBlt(hDcMem, 0, 0, rect.right, rect.bottom, hDcDesktop, 0, 0, SRCCOPY|CAPTUREBLT);
	::ReleaseDC(hWndDesktop, hDcDesktop);
	::DeleteDC(hDcMem);
	return hBmp;
}

HBITMAP CreateDesktopMaskBitmap(HWND hWnd)
{
	HDC hDCMem = CreateCompatibleDC(NULL);
	RECT rect;
	GetWindowRect(::GetDesktopWindow(), &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.left = rect.top = 0;

	HBITMAP hBmp = NULL;

	HDC hDC = GetDC(hWnd);
	hBmp = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	ReleaseDC(hWnd, hDC);

	HGDIOBJ hOld = SelectObject(hDCMem, hBmp);
	CRenderEngine::DrawColor(hDCMem, rect, 0x4F000000);
	SelectObject(hDCMem, hOld);

	DeleteObject(hDCMem);
	return hBmp;
}

HWND SmallestWindowFromCursor(RECT& rcWindow)
{	
	HWND hWnd, hTemp;
	POINT pt;
	::GetCursorPos(&pt);
	hWnd = ::ChildWindowFromPointEx(::GetDesktopWindow(), pt, CWP_SKIPDISABLED|CWP_SKIPINVISIBLE);
	if( hWnd != NULL )
	{
		hTemp = hWnd;  
		while (true) {
			::GetCursorPos(&pt);
			::ScreenToClient(hTemp, &pt);  
			hTemp = ::ChildWindowFromPointEx(hTemp, pt, CWP_SKIPINVISIBLE);  
			if (hTemp == NULL || hTemp == hWnd)  
				break;
			hWnd = hTemp;
		}  
		::GetWindowRect(hWnd, &rcWindow);
	}
	return hWnd;
}
