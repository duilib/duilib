#include "stdafx.h"
#include "LabelUI.h"

UI_IMPLEMENT_DYNCREATE(CLabelUI);

CLabelUI::CLabelUI(void)
{
}


CLabelUI::~CLabelUI(void)
{
}

LPCTSTR CLabelUI::GetClass() const
{
	return _T("Label");
}

LPVOID CLabelUI::GetInterface(LPCTSTR lpszClass)
{
	if( _tcscmp(lpszClass, _T("Label")) == 0 )
		return this;
	else
		return CControlUI::GetInterface(lpszClass);
}

bool CLabelUI::EventHandler(TEventUI& event)
{
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
	if( event.dwType == UIEVENT_MOUSEENTER )
	{
		// return;
	}
	if( event.dwType == UIEVENT_MOUSELEAVE )
	{
		// return;
	}
	return CControlUI::EventHandler(event);
}

void CLabelUI::Render(IUIRender* pRender, LPCRECT pRcPaint)
{

}

void CLabelUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
}
