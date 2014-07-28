#pragma once
#include "../DuiLib/UIlib.h"
#include "UIProperties.h"

//////////////////////////////////////////////////////////////////////////
//CPropertiesWnd

#define UPDATE_POS 0x01
#define UPDATE_REDRAW_CONTROL 0x02
#define UPDATE_REDRAW_PARENT 0x03

class CPropertiesWnd : public CDockablePane
{
// 构造
public:
	CPropertiesWnd();
	virtual ~CPropertiesWnd();

// 实现
public:
	void AdjustLayout();
	void SetVSDotNetLook(BOOL bSet);

	void ShowProperty(CControlUI* pControl);
	void SetPropValue(CControlUI* pControl,int nTag);

protected:
	void SetUIValue(CMFCPropertyGridProperty* pProp,int nTag);

protected:
	CUIProperties m_wndUIProperties;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUIPropChanged(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};