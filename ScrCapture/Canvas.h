#pragma once

class CCanvasUI: public CControlUI
{
public:
	CCanvasUI();
	~CCanvasUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event) ;
};
