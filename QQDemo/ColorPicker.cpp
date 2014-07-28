#include "stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "ColorPicker.hpp"
#include "chat_dialog.hpp"

static DWORD Colors[5][8] = 
{
	{
		0xFF000000,0xFFA52A00,0xFF004040,0xFF005500,0xFF00005E,0xFF00008B,0xFF4B0082,0xFF282828
	},
	{
	0xFF8B0000,0xFFFF6820,0xFF8B8B00,0xFF009300,0xFF388E8E,0xFF0000FF,0xFF7B7BC0,0xFF666666
	},
	{
		0xFFFF0000,0xFFFFAD5B,0xFF32CD32,0xFF3CB371,0xFF7FFFD4,0xFF7D9EC0,0xFF800080,0xFF7F7F7F
	},
	{
		0xFFFFC0CB,0xFFFFD700,0xFFFFFF00,0xFF00FF00,0xFF40E0D0,0xFFC0FFFF,0xFF480048,0xFFC0C0C0
	},
	{
		0xFFFFE4E1,0xFFD2B48C,0xFFFFFFE0,0xFF98FB98,0xFFAFEEEE,0xFF68838B,0xFFE6E6FA,0xFFA020F0
	}
};

CColorPicker::CColorPicker(ChatDialog* chat_dialog, POINT ptMouse)
: based_point_(ptMouse)
, chat_dialog_(chat_dialog)
{
	Create(NULL, _T("color"), WS_POPUP, WS_EX_TOOLWINDOW, 0, 0);
	ShowWindow(true);
}

LPCTSTR CColorPicker::GetWindowClassName() const 
{ 
	return _T("ColorWindow");
}

void CColorPicker::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CColorPicker::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CControlUI* pOne = static_cast<CControlUI*>(m_PaintManager.FindControl(msg.ptMouse));
		if (_tcsicmp(pOne->GetClass(), _T("ButtonUI")) == 0)
		{
			DWORD nColor = pOne->GetBkColor();
			CVerticalLayoutUI* pColorContiner = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("color")));
			pColorContiner->SetBkColor(nColor);
			UpdateWindow(m_hWnd);
			Sleep(500);
			chat_dialog_->SetTextColor(nColor);
		}
	}
}

void CColorPicker::InitWindow()
{
	CVerticalLayoutUI* pColorContiner = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("color")));
	for (int i = 0; (i < 5) && (pColorContiner != NULL); i ++)
	{
		CHorizontalLayoutUI* pLine = new CHorizontalLayoutUI();
		pLine->SetFixedHeight(12);
		pColorContiner->Add(pLine);
		for (int j = 0; j < 8; j++)
		{
			CButtonUI* pOne = new CButtonUI();
			pOne->ApplyAttributeList(_T("bordersize=\"1\" bordercolor=\"#FF000000\" width=\"10\" height=\"10\""));
			pOne->SetBkColor(Colors[i][j]);
			pLine->Add(pOne);
			if (i < 7)
			{
				CControlUI* pMargin = new CControlUI();
				pMargin->SetFixedWidth(2);
				pLine->Add(pMargin);
			}
		}
	}

	SIZE size = m_PaintManager.GetInitSize();
	MoveWindow(GetHWND(), based_point_.x - static_cast<LONG>(size.cx / 2), based_point_.y - size.cy, size.cx, size.cy, FALSE);
}

CDuiString CColorPicker::GetSkinFile()
{
	return _T("color.xml");
}

CDuiString CColorPicker::GetSkinFolder()
{
	return CDuiString(CPaintManagerUI::GetInstancePath()) + _T("skin\\");
}

LRESULT CColorPicker::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	chat_dialog_->SetTextColor(0);
	Close();
	return 0;
}
