#pragma once

// CDialogSaveAsName 对话框

class CDialogSaveAsName : public CDialog
{
	DECLARE_DYNAMIC(CDialogSaveAsName)

public:
	CDialogSaveAsName(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogSaveAsName();

// 对话框数据
	enum { IDD = IDD_DIALOG__SAVE_AS_NAME };

public:
	CString GetSaveAsName() const { return m_strName; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString m_strName;
};
