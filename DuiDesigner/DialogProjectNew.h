#pragma once
#include "resource.h"

// CDialogProjectNew 对话框

class CDialogProjectNew : public CDialog
{
	DECLARE_DYNAMIC(CDialogProjectNew)

public:
	CDialogProjectNew(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogProjectNew();

// 对话框数据
	enum { IDD = IDD_PROJECT_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString m_strPath;
	CString m_strName;
	afx_msg void OnBnClickedButtonBrowse();
	static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);
	afx_msg void OnBnClickedOk();
};
