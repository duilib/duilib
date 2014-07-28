// DialogSaveAsName.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogSaveAsName.h"


// CDialogSaveAsName 对话框

IMPLEMENT_DYNAMIC(CDialogSaveAsName, CDialog)

CDialogSaveAsName::CDialogSaveAsName(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSaveAsName::IDD, pParent)
	, m_strName(_T("未命名1"))
{

}

CDialogSaveAsName::~CDialogSaveAsName()
{
}

void CDialogSaveAsName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDialogSaveAsName, CDialog)
END_MESSAGE_MAP()


// CDialogSaveAsName 消息处理程序
