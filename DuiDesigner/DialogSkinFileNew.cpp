// DialogSkinFileNew.cpp : implementation file
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogSkinFileNew.h"


// CDialogSkinFileNew dialog

IMPLEMENT_DYNAMIC(CDialogSkinFileNew, CDialog)

CDialogSkinFileNew::CDialogSkinFileNew(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSkinFileNew::IDD, pParent)
	, m_strUITitle(_T(""))
	, m_strStyleFile(_T(""))
{

}

CDialogSkinFileNew::~CDialogSkinFileNew()
{
}

void CDialogSkinFileNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_strUITitle);
	DDX_Control(pDX, IDC_LIST_STYLE, m_lstStyles);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_StylePreview);
}


BEGIN_MESSAGE_MAP(CDialogSkinFileNew, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_STYLE, &CDialogSkinFileNew::OnLbnSelchangeListStyle)
	ON_BN_CLICKED(IDOK, &CDialogSkinFileNew::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogSkinFileNew message handlers

BOOL CDialogSkinFileNew::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_lstStyles.AddString(_T("(нч)"));
	m_lstStyles.SetCurSel(0);
	FindStyleFiles(CGlobalVariable::GetStylesDir());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CString& CDialogSkinFileNew::GetStyleFilePath()
{
	return m_strStyleFile;
}

void CDialogSkinFileNew::FindStyleFiles(CString& strDir)
{
	WIN32_FIND_DATA FindFileData = {0};
	CString strFind = strDir + _T("*.*");
	HANDLE hFind = ::FindFirstFile(strFind, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if((FindFileData.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			if(FindFileData.cFileName[0] == '.')
				if (FindFileData.cFileName[1] == '\0' ||
					(FindFileData.cFileName[1] == '.' &&
					FindFileData.cFileName[2] == '\0'))
					continue;

			m_lstStyles.AddString(FindFileData.cFileName);
		}
	}while(::FindNextFile(hFind, &FindFileData));
	::FindClose(hFind);
}

void CDialogSkinFileNew::OnLbnSelchangeListStyle()
{
	// TODO: Add your control notification handler code here
	CString strText;
	int nIndex = m_lstStyles.GetCurSel();
	if(nIndex == -1)
		return;

	m_lstStyles.GetText(nIndex, strText);
	CString strStyleDir;
	strStyleDir = (strText == _T("(нч)")) ? _T("") : CGlobalVariable::GetStylesDir() \
		+ strText;
	if(!strStyleDir.IsEmpty())
	{
		m_strStyleFile = strStyleDir + _T("\\style.xml");
		m_StylePreview.SetPreviewImage(strStyleDir + _T("\\style.ui"));
	}
	else
	{
		m_strStyleFile.Empty();
		m_StylePreview.SetPreviewImage(_T(""));
	}
}

void CDialogSkinFileNew::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	OnOK();
}
