#pragma once
#include "afxwin.h"
#include "UIProperties.h"

//////////////////////////////////////////////////////////////////////////
// CDialogDefaultAttribList dialog

class CDialogDefaultAttribList : public CDialog
{
	DECLARE_DYNAMIC(CDialogDefaultAttribList)

public:
	CDialogDefaultAttribList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogDefaultAttribList();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEFAULT_ATTRIB_LIST };

private:
	CUIProperties m_wndUIProperties;
	CPaintManagerUI* m_pManager;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	BOOL GetDefaultAttrib(CControlUI* pControl, CString& strValue);

private:
	CStatic m_wndUIPropLocation;
	CMFCMenuButton m_btnAdd;
	CMenu m_menuUI;
	CListBox m_lstDefaultAttrib;
public:
	afx_msg void OnBnClickedButtonAttribAdd();
	afx_msg void OnBnClickedButtonAttribDelete();
	afx_msg void OnBnClickedButtonAttribModify();
	afx_msg void OnLbnSelchangeListDefaultAttrib();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonSaveAsStyle();
};
