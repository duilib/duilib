#pragma once


// CDialogCheckUpdate 对话框

class CDialogCheckUpdate : public CDialog
{
	DECLARE_DYNAMIC(CDialogCheckUpdate)

public:
	CDialogCheckUpdate(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogCheckUpdate();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHECK_UPDATE };

protected:
	CString GetLatestVersionInfo(LPCTSTR pstrURL);

private:
	CMFCLinkCtrl m_btnUpdateURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
