#pragma once
#include "afxwin.h"


// CDialogCustomFonts dialog

class CDialogCustomFonts : public CDialog
{
	DECLARE_DYNAMIC(CDialogCustomFonts)

public:
	CDialogCustomFonts(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogCustomFonts();

// Dialog Data
	enum { IDD = IDD_DIALOG_CUSTOM_FONTS };

private:
	CPaintManagerUI* m_pManager;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_lstCustomFonts;
	afx_msg void OnBnClickedButtonFontAdd();
	afx_msg void OnBnClickedButtonFontDelete();
	afx_msg void OnBnClickedButtonFontModify();
};
