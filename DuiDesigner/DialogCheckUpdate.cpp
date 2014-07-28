// DialogCheckUpdate.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogCheckUpdate.h"
#include <wininet.h>

#pragma comment(lib, "WinInet")

// CDialogCheckUpdate 对话框

IMPLEMENT_DYNAMIC(CDialogCheckUpdate, CDialog)

CDialogCheckUpdate::CDialogCheckUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCheckUpdate::IDD, pParent)
{

}

CDialogCheckUpdate::~CDialogCheckUpdate()
{
}

void CDialogCheckUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_VERSION_UPDATE_LINK, m_btnUpdateURL);
}


BEGIN_MESSAGE_MAP(CDialogCheckUpdate, CDialog)
END_MESSAGE_MAP()


// CDialogCheckUpdate 消息处理程序

BOOL CDialogCheckUpdate::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strLatestVersionInfo = GetLatestVersionInfo(UIDESIGNER_VERSION_CHECK_URL);
	CString strLastestVersion;
	CString strUpdateURL;
	AfxExtractSubString(strLastestVersion,strLatestVersionInfo, 0, '|');
	AfxExtractSubString(strUpdateURL,strLatestVersionInfo, 1, '|');
	this->GetDlgItem(IDC_STATIC_CURRENT_VERSION)->SetWindowText(UIDESIGNER_VERSION);
	this->GetDlgItem(IDC_STATIC_LATEST_VERSION)->SetWindowText(strLastestVersion);
	CWnd* pWndInfo = this->GetDlgItem(IDC_STATIC_UPDATE_INFO);
	if(strLatestVersionInfo.IsEmpty())
		pWndInfo->SetWindowText(_T("检查新版本失败！"));
	else if(strLastestVersion == UIDESIGNER_VERSION)
		pWndInfo->SetWindowText(_T("您的版本已经是最新！"));
	else
	{
		m_btnUpdateURL.SetURL(strUpdateURL);
		m_btnUpdateURL.SetTooltip(_T("下载最新版本"));
		m_btnUpdateURL.SizeToContent();
		m_btnUpdateURL.ShowWindow(SW_SHOW);
		pWndInfo->SetWindowText(_T("新版本可供下载！"));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

CString CDialogCheckUpdate::GetLatestVersionInfo(LPCTSTR pstrURL)
{
	HINTERNET hInet = InternetOpen(_T("Check Update"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	HINTERNET hUrl = InternetOpenUrl(hInet, pstrURL, NULL, (DWORD)-1L
		, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE |WININET_API_FLAG_ASYNC, NULL);

	char szBuffer[512] = { 0 };
	TCHAR szString[512] = { 0 };
	if(hUrl)
	{
		DWORD dwRead;

		if (InternetReadFile(hUrl, szBuffer, sizeof(szBuffer), &dwRead))
		{
			if (dwRead > 0)
			{
				szBuffer[dwRead] = 0;
				MultiByteToWideChar(CP_ACP,0,szBuffer, strlen(szBuffer)+1, szString, sizeof(szString)/2);
			}
		}
	}

	InternetCloseHandle(hInet);
	return szString;
}
