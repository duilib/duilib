#pragma once
#include "ToolBoxCtrl.h"

// CToolBoxWnd

class CToolBoxWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CToolBoxWnd)

public:
	CToolBoxWnd();
	virtual ~CToolBoxWnd();

protected:
	CToolBoxCtrl m_ctlToolList;

public:
	CToolElement* GetCurSel() const { return m_ctlToolList.GetCurSel(); }
	void SetCurSel(int nClass,BOOL bRedraw=TRUE) { m_ctlToolList.SetCurSel(nClass,bRedraw); }

protected:
	void AdjustLayout();
	void InitToolList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


