// DialogProjectNew.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogProjectNew.h"


// CDialogProjectNew 对话框

IMPLEMENT_DYNAMIC(CDialogProjectNew, CDialog)

CDialogProjectNew::CDialogProjectNew(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProjectNew::IDD, pParent)
	, m_strPath(_T(""))
	, m_strName(_T(""))
{

}

CDialogProjectNew::~CDialogProjectNew()
{
}

void CDialogProjectNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROJECT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_PROJECT_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDialogProjectNew, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CDialogProjectNew::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDOK, &CDialogProjectNew::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogProjectNew 消息处理程序

void CDialogProjectNew::OnBnClickedButtonBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	BROWSEINFO info;
	TCHAR szDefaultDir[MAX_PATH]={0};
	_tcscpy(szDefaultDir,m_strPath);
	ZeroMemory(&info, sizeof(BROWSEINFO));
	info.hwndOwner = this->GetSafeHwnd();
	info.lpszTitle = _T("请选择一个文件夹:");
	info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	info.lpfn = BrowseCallbackProc;
	info.lParam = long(&szDefaultDir);
	ITEMIDLIST *pItem;
	pItem = SHBrowseForFolder(&info);
	if(pItem)
	{  
		TCHAR szPath[MAX_PATH]={0};
		SHGetPathFromIDList(pItem, szPath);
		GlobalFree(pItem);
		m_strPath = szPath;

		UpdateData(FALSE);
	}
}

int CALLBACK CDialogProjectNew::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			break;
		}
	case BFFM_SELCHANGED:
		{
			TCHAR curr[MAX_PATH];   
			SHGetPathFromIDList((LPCITEMIDLIST)lParam, curr);   
			::SendMessage(hwnd, BFFM_SETSTATUSTEXT,0, (LPARAM)curr);   
		}
		break;
	default:
		break;
	}

	return 0;   
}

void CDialogProjectNew::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if(m_strName.IsEmpty())
	{
		MessageBox(_T("项目名称不能设置为空。"),_T("提示"));
		return;
	}
	if(m_strPath.IsEmpty())
	{
		MessageBox(_T("路径名称不能设置为空。"),_T("提示"));
		return;
	}
	if(!PathIsDirectory(m_strPath))
	{
		MessageBox(_T("所设路径已不存在，可能被移除。"),_T("提示"));
		return;
	}
	if(m_strPath.Right(1) != _T("\\"))
		m_strPath += _T("\\");

	UpdateData(FALSE);
	OnOK();
}
