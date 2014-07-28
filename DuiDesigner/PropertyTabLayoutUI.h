#pragma once
#include "afxwin.h"

//////////////////////////////////////////////////////////////////////////
//CSingleCheckListBox

class CSingleCheckListBox : public CCheckListBox
{
	DECLARE_DYNAMIC(CSingleCheckListBox)

public:
	static int m_nLastChecked;

protected:
	//{{AFX_MSG(CSingleCheckListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// CPropertyTabLayoutUI Dialog

class CPropertyTabLayoutUI : public CDialog
{
	DECLARE_DYNAMIC(CPropertyTabLayoutUI)

public:
	CPropertyTabLayoutUI(CControlUI* const pControl,CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertyTabLayoutUI();

// Dialog Data
	enum { IDD = IDD_PROPERTY_TABLAYOUTUI };

private:
	CSingleCheckListBox m_lstTab;
	CMFCMenuButton m_btnAdd;
	CMenu m_menuUI;

	CTabLayoutUI* m_pTablayoutUI;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonTabLayoutUIAdd();
	afx_msg void OnBnClickedOk();
};
