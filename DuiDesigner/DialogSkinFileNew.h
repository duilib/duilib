#pragma once
#include "afxwin.h"
#include "UIImagePreview.h"

// CDialogSkinFileNew dialog

class CDialogSkinFileNew : public CDialog
{
	DECLARE_DYNAMIC(CDialogSkinFileNew)

public:
	CDialogSkinFileNew(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogSkinFileNew();

// Dialog Data
	enum { IDD = IDD_SKINFILE_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	CString& GetStyleFilePath();

private:
	void FindStyleFiles(CString& strDir);

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUITitle;

private:
	CListBox m_lstStyles;
	CUIImagePreview m_StylePreview;
	CString m_strStyleFile;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeListStyle();
	afx_msg void OnBnClickedOk();
};
