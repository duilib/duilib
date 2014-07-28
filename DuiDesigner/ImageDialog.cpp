// ImageDialog.cpp : implementation file
//
#include "stdafx.h"
#include "DuiDesigner.h"
#include "ImageDialog.h"

//////////////////////////////////////////////////////////////////////////
//CMyColorButton

IMPLEMENT_DYNAMIC(CMyColorButton, CMFCColorButton)

static const int nImageHorzMargin = 8;
const CString CImageDialog::m_strNullImage=_T("");

void CMyColorButton::OnDraw(CDC* pDC, const CRect& rect, UINT uiState)
{
	ASSERT_VALID(pDC);

	if (m_pPalette == NULL)
	{
		RebuildPalette(NULL);
	}

	CPalette* pCurPalette = pDC->SelectPalette(m_pPalette, FALSE);
	pDC->RealizePalette();

	CSize sizeArrow = CMenuImages::Size();

	CRect rectColor = rect;
	rectColor.right -= sizeArrow.cx + nImageHorzMargin;

	CRect rectArrow = rect;
	rectArrow.left = rectColor.right;

	COLORREF color = m_Color;

	//---------------------------
	// Draw current color and color's value:
	//---------------------------
	CString strColor;
	color==(COLORREF)-1?strColor=_T("无"):strColor.Format(_T("0x%06x"),color);

	rectColor.right = rectColor.left + rectColor.Height();

	CRect rectText = rect;
	rectText.left = rectColor.right;
	rectText.right = rectArrow.left;

	CFont* pOldFont=(CFont *)pDC->SelectStockObject(DEFAULT_GUI_FONT);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(afxGlobalData.clrBtnText);
	pDC->DrawText(strColor, rectText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	pDC->SelectObject(pOldFont);

	//----------------
	// Draw color box:
	//----------------
	rectColor.DeflateRect(2, 2);
	pDC->Draw3dRect(rectColor, afxGlobalData.clrBtnHilite, afxGlobalData.clrBtnHilite);
	rectColor.DeflateRect(1, 1);
	pDC->Draw3dRect(rectColor, afxGlobalData.clrBtnDkShadow, afxGlobalData.clrBtnDkShadow);
	rectColor.DeflateRect(1, 1);

	if (color != (COLORREF)-1 &&(uiState & ODS_DISABLED) == 0)
	{
		if (afxGlobalData.m_nBitsPerPixel == 8) // 256 colors
		{
			ASSERT_VALID(m_pPalette);
			color =  PALETTEINDEX(m_pPalette->GetNearestPaletteIndex(color));
		}

		CBrush br(color);
		pDC->FillRect(rectColor, &br);
	}

	//----------------------
	// Draw drop-down arrow:
	//----------------------
	CRect rectArrowWinXP = rectArrow;
	rectArrowWinXP.DeflateRect(2, 2);

	if (!m_bWinXPTheme || !CMFCVisualManager::GetInstance()->DrawComboDropButtonWinXP(pDC, rectArrowWinXP, (uiState & ODS_DISABLED), m_bPushed, m_bHighlighted))
	{
		pDC->FillRect(rectArrow, &afxGlobalData.brBtnFace);

		CMenuImages::Draw(pDC, CMenuImages::IdArrowDownLarge, rectArrow, (uiState & ODS_DISABLED) ? CMenuImages::ImageGray : CMenuImages::ImageBlack);

		pDC->Draw3dRect(rectArrow, afxGlobalData.clrBtnLight, afxGlobalData.clrBtnDkShadow);
		rectArrow.DeflateRect(1, 1);
		pDC->Draw3dRect(rectArrow, afxGlobalData.clrBtnHilite, afxGlobalData.clrBtnShadow);
	}

	if (pCurPalette != NULL)
	{
		pDC->SelectPalette(pCurPalette, FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////
// CImageDialog dialog

IMPLEMENT_DYNAMIC(CImageDialog, CDialog)

CImageDialog::CImageDialog(const CString& strImageProperty, CWnd* pParent /*=NULL*/)
	: CDialog(CImageDialog::IDD, pParent)
	, m_strDest(_T("0,0,0,0"))
	, m_strSource(_T("0,0,0,0"))
	, m_strCorner(_T("0,0,0,0"))
	, m_nFade(255)
	, m_bHole(FALSE)
	, m_pManager(NULL)
{
	m_strImageProperty=strImageProperty;
	m_rcDest.SetRectEmpty();
	m_rcSource.SetRectEmpty();
	m_rcCorner.SetRectEmpty();
}

CImageDialog::~CImageDialog()
{
	m_strImageArray.RemoveAll();
}

void CImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGE_RESOURCE, m_lstImages);
	DDX_Text(pDX, IDC_EDIT_IMAGE_DEST, m_strDest);
	DDX_Text(pDX, IDC_EDIT_IMAGE_SOURCE, m_strSource);
	DDX_Text(pDX, IDC_EDIT_IMAGE_CORNER, m_strCorner);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_MASK, m_btnMask);
	DDX_Control(pDX, IDC_SLIDER_IMAGE_FADE, m_ctlFade);
	DDX_Text(pDX, IDC_EDIT_IMAGE_FADE, m_nFade);
	DDX_Radio(pDX, IDC_RADIO_IMAGE_HOLE_FALSE, m_bHole);
	DDX_Control(pDX, IDC_STATIC_IMAGE_PREVIEW, m_ImagePreview);
}


BEGIN_MESSAGE_MAP(CImageDialog, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_IMPORT, &CImageDialog::OnBnClickedButtonImageImport)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_CLEAR, &CImageDialog::OnBnClickedButtonImageClear)
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_FADE, &CImageDialog::OnEnKillfocusEditImageFade)
	ON_LBN_DBLCLK(IDC_LIST_IMAGE_RESOURCE, &CImageDialog::OnLbnDblclkListImageResource)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_DEST, &CImageDialog::OnEnKillfocusEditImageDest)
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_SOURCE, &CImageDialog::OnEnKillfocusEditImageSource)
	ON_EN_KILLFOCUS(IDC_EDIT_IMAGE_CORNER, &CImageDialog::OnEnKillfocusEditImageCorner)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_MASK, &CImageDialog::OnBnClickedButtonImageMask)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_HOLE_FALSE, &CImageDialog::OnBnClickedRadioImageHoleFalse)
	ON_BN_CLICKED(IDC_RADIO_IMAGE_HOLE_TRUE, &CImageDialog::OnBnClickedRadioImageHoleTrue)
	ON_BN_CLICKED(IDOK, &CImageDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CImageDialog message handlers

CString CImageDialog::GetImage() const
{
	return m_strImageProperty;
}

BOOL CImageDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_btnMask.EnableOtherButton(_T("Other"));
	m_btnMask.SetColor((COLORREF)0);
	m_btnMask.SetColumnsNumber(10);

	m_ctlFade.SetRange(0,255);
	m_ctlFade.SetPos(255);

	CUIDesignerView* pUIView = g_pMainFrame->GetActiveUIView();
	m_pManager = pUIView->GetPaintManager();
	m_ImagePreview.SetManager(m_pManager);

	m_strImagePathName=m_strImageProperty;
	LPCTSTR pStrImage=m_strImageProperty;
	CDuiString sItem;
	CDuiString sValue;
	LPTSTR pstr = NULL;
	while( *pStrImage != _T('\0') ) {
		sItem.Empty();
		sValue.Empty();
		while( *pStrImage != _T('\0') && *pStrImage != _T('=') ) {
			LPTSTR pstrTemp = ::CharNext(pStrImage);
			while( pStrImage < pstrTemp) {
				sItem += *pStrImage++;
			}
		}
		if( *pStrImage++ != _T('=') ) break;
		if( *pStrImage++ != _T('\'') ) break;
		while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
			LPTSTR pstrTemp = ::CharNext(pStrImage);
			while( pStrImage < pstrTemp) {
				sValue += *pStrImage++;
			}
		}
		if( *pStrImage++ != _T('\'') ) break;
		if( !sValue.IsEmpty() ) {
			if( sItem == _T("file"))
				m_strImagePathName = sValue;
			else if( sItem == _T("dest") )
				m_strDest = sValue;
			else if( sItem == _T("source") )
				m_strSource = sValue;
			else if( sItem == _T("corner") )
				m_strCorner = sValue;
			else if( sItem == _T("mask") ) {
				DWORD dwMask;
				if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
				else dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
				dwMask&=0x00FFFFFF;
				m_btnMask.SetColor(RGB(GetBValue(dwMask),GetGValue(dwMask),GetRValue(dwMask)));
			}
			else if( sItem == _T("fade") ) {
				m_nFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
			}
			else if( sItem == _T("hole") ) {
				m_bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
			}
		}
		if( *pStrImage++ != _T(' ') ) break;
	}
	m_ctlFade.SetPos(m_nFade);

	int nIndex=m_lstImages.AddString(_T("(无)"));
	m_lstImages.SetItemDataPtr(nIndex,(void*)(LPCTSTR)m_strNullImage);

	LPCTSTR pstrImage=NULL;
	LPTSTR pszFileName=NULL;
 	const CStringArray* parrImage=g_pResourceView->GetImageTree(pUIView->GetDocument()->GetTitle());
	if(parrImage)
	{
		for(int i=0;i<parrImage->GetSize();i++)
		{
			pstrImage=parrImage->GetAt(i);
			pszFileName=_tcsrchr((LPTSTR)pstrImage,_T('\\'))+1;
			nIndex=m_lstImages.AddString(pszFileName);
			m_lstImages.SetItemDataPtr(nIndex,(void*)pstrImage);
		}
	}
	int nPos = m_strImagePathName.ReverseFind(_T('\\'));
	CString strFileName = (nPos==-1) ? m_strImagePathName : m_strImagePathName.Right(m_strImagePathName.GetLength() - nPos - 1);
	m_strImagePathName.IsEmpty()?m_lstImages.SelectString(-1,_T("(无)")):m_lstImages.SelectString(-1,strFileName);

	UpdateData(FALSE);
	SetImageProperty(m_strImagePathName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImageDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_nFade=m_ctlFade.GetPos();
	UpdateData(FALSE);
	SetImageProperty(m_strImagePathName);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImageDialog::OnBnClickedButtonImageImport()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,_T(""),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("图片文件(*.bmp;*.jpg;*.png)|*.bmp;*.jpg;*.png|所有文件(*.*)|*.*||"));
	if(dlg.DoModal()==IDOK)
	{
		CString strFileName = dlg.GetFileName();
		int nIndex = m_lstImages.AddString(strFileName);
		int nPos = m_strImageArray.Add(dlg.GetPathName());
		LPCTSTR pstrPath = m_strImageArray[nPos];
		m_lstImages.SetItemDataPtr(nIndex,(void*)pstrPath);
		m_lstImages.SelectString(-1, strFileName);
		SetImageProperty(pstrPath);
	}
}

void CImageDialog::OnBnClickedButtonImageClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strDest=_T("0,0,0,0");
	m_strSource=_T("0,0,0,0");
	m_strCorner=_T("0,0,0,0");
	m_nFade=255;
	m_bHole=FALSE;
	m_ctlFade.SetPos(m_nFade);
	m_btnMask.SetColor(0);

	UpdateData(FALSE);
	SetImageProperty(m_strNullImage);
}

void CImageDialog::SetImageProperty(LPCTSTR pstrPathName)
{
	UpdateData(TRUE);

	m_strImagePathName=pstrPathName;
	m_rcDest=StringToRect(m_strDest);
	m_rcSource=StringToRect(m_strSource);
	m_rcCorner=StringToRect(m_strCorner);
	COLORREF clrMask=m_btnMask.GetColor();
	clrMask=(RGB(GetBValue(clrMask),GetGValue(clrMask),GetRValue(clrMask))|0xFF000000);

	m_ImagePreview.SetImageProperty(pstrPathName,m_rcDest,m_rcSource,m_rcCorner,clrMask,m_nFade,m_bHole);
}

RECT CImageDialog::StringToRect(CString& str)
{
	RECT rc={0};
	LPTSTR pstr = NULL;

	rc.left = _tcstol(str, &pstr, 10);		  ASSERT(pstr);
	rc.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	rc.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	rc.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

	return rc;
}

BOOL CImageDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN)
		{
			switch(this->GetFocus()->GetDlgCtrlID())
			{
			case IDC_EDIT_IMAGE_FADE:
				UpdateData(TRUE);
				if(m_nFade>255)
					m_nFade=255;
				else if(m_nFade<0)
					m_nFade=0;
				m_ctlFade.SetPos(m_nFade);
				UpdateData(FALSE);
			case IDC_EDIT_IMAGE_DEST:
			case IDC_EDIT_IMAGE_SOURCE:
			case IDC_EDIT_IMAGE_CORNER:
				SetImageProperty(m_strImagePathName);

				break;
			}

			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CImageDialog::OnEnKillfocusEditImageFade()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_nFade>255)
		m_nFade=255;
	else if(m_nFade<0)
		m_nFade=0;
	m_ctlFade.SetPos(m_nFade);

	UpdateData(FALSE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnLbnDblclkListImageResource()
{
	// TODO: Add your control notification handler code here
	int nIndex=m_lstImages.GetCurSel();
	LPCTSTR pstrImage=(LPCTSTR)m_lstImages.GetItemDataPtr(nIndex);

	SetImageProperty(pstrImage);
}

void CImageDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}
void CImageDialog::OnEnKillfocusEditImageDest()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnEnKillfocusEditImageSource()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnEnKillfocusEditImageCorner()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnBnClickedButtonImageMask()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnBnClickedRadioImageHoleFalse()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}

void CImageDialog::OnBnClickedRadioImageHoleTrue()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	SetImageProperty(m_strImagePathName);
}
void CImageDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_strImageProperty=m_ImagePreview.GetImageProperty();
	CUIDesignerView* pUIView = g_pMainFrame->GetActiveUIView();
	g_pResourceView->InsertImage(m_strImagePathName, pUIView->GetDocument()->GetTitle());

	OnOK();
}