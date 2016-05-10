#include "StdAfx.h"
#include "Canvas.h"

CCanvasUI::CCanvasUI()
{

}

CCanvasUI::~CCanvasUI()
{

}

LPCTSTR CCanvasUI::GetClass() const
{
	return DUI_CTR_CANVAS;
}

LPVOID CCanvasUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_CANVAS) == 0 ) return static_cast<CCanvasUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CCanvasUI::DoEvent(TEventUI& event) 
{
	// if m_rcItem.left < 20 ×ó±ß¿ÉÒÔÐÞ¸Ä
}
