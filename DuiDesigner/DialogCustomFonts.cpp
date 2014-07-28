// DialogCustomFonts.cpp : implementation file
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogCustomFonts.h"


// CDialogCustomFonts dialog

IMPLEMENT_DYNAMIC(CDialogCustomFonts, CDialog)

CDialogCustomFonts::CDialogCustomFonts(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCustomFonts::IDD, pParent)
{
	m_pManager = NULL;
}

CDialogCustomFonts::~CDialogCustomFonts()
{
}

void CDialogCustomFonts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CUSTOM_FONTS, m_lstCustomFonts);
}


BEGIN_MESSAGE_MAP(CDialogCustomFonts, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FONT_ADD, &CDialogCustomFonts::OnBnClickedButtonFontAdd)
	ON_BN_CLICKED(IDC_BUTTON_FONT_DELETE, &CDialogCustomFonts::OnBnClickedButtonFontDelete)
	ON_BN_CLICKED(IDC_BUTTON_FONT_MODIFY, &CDialogCustomFonts::OnBnClickedButtonFontModify)
END_MESSAGE_MAP()


// CDialogCustomFonts message handlers

BOOL CDialogCustomFonts::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pManager = g_pMainFrame->GetActiveUIView()->GetPaintManager();
	ASSERT(m_pManager);

	m_lstCustomFonts.InsertColumn(0, _T("Index"), LVCFMT_CENTER, 50);
	m_lstCustomFonts.InsertColumn(1, _T("Name"), LVCFMT_CENTER, 60);
	m_lstCustomFonts.InsertColumn(2, _T("Size"), LVCFMT_CENTER, 40);
	m_lstCustomFonts.InsertColumn(3, _T("Bold"), LVCFMT_CENTER, 40);
	m_lstCustomFonts.InsertColumn(4, _T("Italic"), LVCFMT_CENTER, 40);
	m_lstCustomFonts.InsertColumn(5, _T("Underline"), LVCFMT_CENTER, 60);
	m_lstCustomFonts.SetExtendedStyle(m_lstCustomFonts.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nCount = m_pManager->GetCustomFontCount();
	CString str;
	for(int i=0; i<nCount; i++)
	{
		TFontInfo* pFontInfo = m_pManager->GetFontInfo(i);
		str.Format(_T("%d"), i);
		m_lstCustomFonts.InsertItem(i, str);
		m_lstCustomFonts.SetItemText(i, 1, pFontInfo->sFontName);
		str.Format(_T("%d"), pFontInfo->iSize);
		m_lstCustomFonts.SetItemText(i, 2, str);
		m_lstCustomFonts.SetItemText(i, 3, pFontInfo->bBold ? _T("true") : _T("false"));
		m_lstCustomFonts.SetItemText(i, 4, pFontInfo->bItalic ? _T("true") : _T("false"));
		m_lstCustomFonts.SetItemText(i, 5, pFontInfo->bUnderline ? _T("true") : _T("false"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogCustomFonts::OnBnClickedButtonFontAdd()
{
	// TODO: Add your control notification handler code here
	CFontDialog dlg(NULL);
	if(dlg.DoModal() == IDOK)
	{
		LOGFONT lf = { 0 };
		dlg.GetCurrentFont(&lf);
		m_pManager->AddFont(lf.lfFaceName, -lf.lfHeight, (lf.lfWeight == FW_BOLD), lf.lfUnderline!=0, lf.lfItalic!=0);

		CString str;
		int nCount = m_lstCustomFonts.GetItemCount();
		str.Format(_T("%d"), nCount);
		m_lstCustomFonts.InsertItem(nCount, str);
		m_lstCustomFonts.SetItemText(nCount, 1, lf.lfFaceName);
		str.Format(_T("%d"), -lf.lfHeight);
		m_lstCustomFonts.SetItemText(nCount, 2, str);
		m_lstCustomFonts.SetItemText(nCount, 3, (lf.lfWeight == FW_BOLD) ? _T("true") : _T("false"));
		m_lstCustomFonts.SetItemText(nCount, 4, lf.lfItalic ? _T("true") : _T("false"));
		m_lstCustomFonts.SetItemText(nCount, 5, lf.lfUnderline ? _T("true") : _T("false"));

		g_pMainFrame->GetActiveUIView()->SetModifiedFlag();
	}
}

void CDialogCustomFonts::OnBnClickedButtonFontDelete()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_lstCustomFonts.GetSelectionMark();
	if(nIndex == -1)
		return;
	if(MessageBox(_T("删除此字体后，使用了其后字体的控件，\n均需手动修改序号，请慎用此功能！")
		, _T("提示"), MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL)
		return;

	if(m_pManager->RemoveFontAt(nIndex))
	{
		m_lstCustomFonts.DeleteItem(nIndex);
		int nCount = m_lstCustomFonts.GetItemCount();
		CString str;
		for(int i=nIndex; i<nCount; i++)
		{
			str.Format(_T("%d"), i);
			m_lstCustomFonts.SetItemText(i, 0, str);
		}
	}
	g_pMainFrame->GetActiveUIView()->ReDrawForm();
	g_pMainFrame->GetActiveUIView()->SetModifiedFlag();
}

void CDialogCustomFonts::OnBnClickedButtonFontModify()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_lstCustomFonts.GetSelectionMark();
	if(nIndex == -1)
		return;
	TFontInfo* pFontInfo = m_pManager->GetFontInfo(nIndex);

	LOGFONT lf = { 0 };
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	_tcscpy(lf.lfFaceName, pFontInfo->sFontName);
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = -pFontInfo->iSize;
	if(pFontInfo->bBold)
		lf.lfWeight += FW_BOLD;
	if(pFontInfo->bUnderline)
		lf.lfUnderline = TRUE;
	if(pFontInfo->bItalic)
		lf.lfItalic = TRUE;
	CFontDialog dlg(&lf);
	if(dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&lf);
		::DeleteObject(pFontInfo->hFont);
		pFontInfo->hFont = ::CreateFontIndirect(&lf);
		pFontInfo->sFontName = lf.lfFaceName;
		pFontInfo->iSize = -lf.lfHeight;
		pFontInfo->bBold = (lf.lfWeight == FW_BOLD);
		pFontInfo->bItalic = (lf.lfItalic!=0);
		pFontInfo->bUnderline = (lf.lfUnderline!=0);

		CString str;
		m_lstCustomFonts.SetItemText(nIndex, 1, pFontInfo->sFontName);
		str.Format(_T("%d"), pFontInfo->iSize);
		m_lstCustomFonts.SetItemText(nIndex, 2, str);
		m_lstCustomFonts.SetItemText(nIndex, 3, pFontInfo->bBold ? _T("true") : _T("false"));
		m_lstCustomFonts.SetItemText(nIndex, 4, pFontInfo->bItalic ? _T("true") : _T("false"));
		m_lstCustomFonts.SetItemText(nIndex, 5, pFontInfo->bUnderline ? _T("true") : _T("false"));

		g_pMainFrame->GetActiveUIView()->ReDrawForm();
		g_pMainFrame->GetActiveUIView()->SetModifiedFlag();
	}
}
