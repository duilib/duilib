#include "stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "color_skin.hpp"
#include "main_frame.hpp"

const TCHAR* const kAdjustColorControlName = _T("adjcolor");
const TCHAR* const kAdjustBkControlName = _T("adjbk");

const TCHAR* const kAdjustColorSliderRControlName = _T("AdjustColorSliderR");
const TCHAR* const kAdjustColorSliderGControlName = _T("AdjustColorSliderG");
const TCHAR* const kAdjustColorSliderBControlName = _T("AdjustColorSliderB");

const TCHAR* const kHColorLayoutControlName = _T("HColorLayout");
const TCHAR* const kHBkLayoutControlName = _T("HBkLayout");

const TCHAR* const kTabControlName = _T("tabs");

ColorSkinWindow::ColorSkinWindow(MainFrame* main_frame, RECT rcParentWindow)
: main_frame_(main_frame)
, parent_window_rect_(rcParentWindow)
{
	Create(NULL, _T("colorskin"), WS_POPUP, WS_EX_TOOLWINDOW, 0, 0);
	ShowWindow(true);
}

LPCTSTR ColorSkinWindow::GetWindowClassName() const 
{ 
	return _T("ColorSkinWindow");
}

void ColorSkinWindow::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void ColorSkinWindow::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (pTabControl != NULL)
		{
			if (pTabControl->GetCurSel() == 0)
			{
				if (_tcsstr(msg.pSender->GetName(), _T("colour_")) != 0)
				{
					CSliderUI* AdjustColorSliderR = static_cast<CSliderUI*>(m_PaintManager.FindControl(kAdjustColorSliderRControlName));
					CSliderUI* AdjustColorSliderG = static_cast<CSliderUI*>(m_PaintManager.FindControl(kAdjustColorSliderGControlName));
					CSliderUI* AdjustColorSliderB = static_cast<CSliderUI*>(m_PaintManager.FindControl(kAdjustColorSliderBControlName));
					if ((AdjustColorSliderR != NULL) && (AdjustColorSliderG != NULL) && (AdjustColorSliderB != NULL))
					{
						DWORD dwColor = msg.pSender->GetBkColor();
						AdjustColorSliderR->SetValue(static_cast<BYTE>(GetRValue(dwColor)));
						AdjustColorSliderG->SetValue(static_cast<BYTE>(GetGValue(dwColor)));
						AdjustColorSliderB->SetValue(static_cast<BYTE>(GetBValue(dwColor)));

						main_frame_->SetBkColor(dwColor);
					}
				}
			}
			else if (pTabControl->GetCurSel() == 1)
			{}
		}
	}
	else if (_tcsicmp(msg.sType, _T("valuechanged")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (pTabControl != NULL)
		{
			if (pTabControl->GetCurSel() == 0)
			{
				CSliderUI* AdjustColorSliderR = static_cast<CSliderUI*>(m_PaintManager.FindControl(kAdjustColorSliderRControlName));
				CSliderUI* AdjustColorSliderG = static_cast<CSliderUI*>(m_PaintManager.FindControl(kAdjustColorSliderGControlName));
				CSliderUI* AdjustColorSliderB = static_cast<CSliderUI*>(m_PaintManager.FindControl(kAdjustColorSliderBControlName));
				if ((AdjustColorSliderR != NULL) && (AdjustColorSliderG != NULL) && (AdjustColorSliderB != NULL))
				{
					if ((_tcsicmp(msg.pSender->GetName(), kAdjustColorSliderRControlName) == 0) ||
						(_tcsicmp(msg.pSender->GetName(), kAdjustColorSliderGControlName) == 0) ||
						(_tcsicmp(msg.pSender->GetName(), kAdjustColorSliderBControlName) == 0))
					{
						BYTE red = AdjustColorSliderR->GetValue();
						BYTE green = AdjustColorSliderG->GetValue();
						BYTE blue = AdjustColorSliderB->GetValue();
						COLORREF crColor = RGB(red, green, blue);
						TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_CE)
						_stprintf(szBuf, _T("FF%02X%02X%02X"), GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
#else
						_stprintf_s(szBuf, MAX_PATH - 1, _T("FF%02X%02X%02X"), GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
#endif
						LPTSTR pstr = NULL;
						DWORD dwColor = _tcstoul(szBuf, &pstr, 16);
						main_frame_->SetBkColor(dwColor);
					}
				}
			}
			else if (pTabControl->GetCurSel() == 1)
			{}
		}
	}
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (_tcsicmp(msg.pSender->GetName(), kAdjustColorControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 0)
			{
				pTabControl->SelectItem(0);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kAdjustBkControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 1)
			{
				pTabControl->SelectItem(1);
			}
		}
	}
}

void ColorSkinWindow::InitWindow()
{
	SIZE size = m_PaintManager.GetInitSize();
	MoveWindow(m_hWnd, parent_window_rect_.right - size.cx, parent_window_rect_.top, size.cx, size.cy, FALSE);
}

CDuiString ColorSkinWindow::GetSkinFile()
{
	return _T("ColorWnd.xml");
}

CDuiString ColorSkinWindow::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath()) + _T("skin\\");
}

LRESULT ColorSkinWindow::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	return 0;
}
