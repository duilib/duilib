// PropertyTabLayoutUI.cpp : implementation file
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "PropertyTabLayoutUI.h"

//////////////////////////////////////////////////////////////////////////
//CSingleCheckListBox

int CSingleCheckListBox::m_nLastChecked=0;

IMPLEMENT_DYNAMIC(CSingleCheckListBox, CCheckListBox)

BEGIN_MESSAGE_MAP(CSingleCheckListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CSingleCheckListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSingleCheckListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL bInCheck;
	int nIndex=CheckFromPoint(point, bInCheck);
	int nCheck=GetCheck( nIndex );
	if(bInCheck)
	{
		if(nCheck==1)
			return;
		else if(nCheck==0)
		{
			SetCheck(m_nLastChecked,0);
			m_nLastChecked=nIndex;
		}
	}

	CCheckListBox::OnLButtonDown(nFlags,point);
}

void CSingleCheckListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	BOOL bInCheck;
	CheckFromPoint(point, bInCheck);

	if (bInCheck)
	{
		return;
	}

	CListBox::OnLButtonDblClk(nFlags, point);
}

void CSingleCheckListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_SPACE)
	{
		int nIndex=GetCaretIndex();
		if(nIndex!=LB_ERR)
		{
			if (m_nStyle != BS_CHECKBOX && m_nStyle != BS_3STATE)
			{
				if ((GetStyle() & LBS_MULTIPLESEL) == 0)
				{
					if (IsEnabled(nIndex))
					{
						int nCheck = GetCheck(nIndex);
						if(nCheck==1)
							return;
						else if(nCheck==0)
						{
							SetCheck(m_nLastChecked,0);
							m_nLastChecked=nIndex;
						}
					}
				}
			}
		}
	}

	CCheckListBox::OnKeyDown(nChar,nRepCnt,nFlags);
}

//////////////////////////////////////////////////////////////////////////
// CPropertyTabLayoutUI Dialog

IMPLEMENT_DYNAMIC(CPropertyTabLayoutUI, CDialog)

CPropertyTabLayoutUI::CPropertyTabLayoutUI(CControlUI* const pControl,CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyTabLayoutUI::IDD, pParent)
{
	m_pTablayoutUI=static_cast<CTabLayoutUI*>(pControl->GetInterface(_T("TabLayout")));
}

CPropertyTabLayoutUI::~CPropertyTabLayoutUI()
{
}

void CPropertyTabLayoutUI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TABLAYOUTUI_TAB, m_lstTab);
	DDX_Control(pDX, IDC_BUTTON_TABLAYOUTUI_ADD, m_btnAdd);
}


BEGIN_MESSAGE_MAP(CPropertyTabLayoutUI, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TABLAYOUTUI_ADD, &CPropertyTabLayoutUI::OnBnClickedButtonTabLayoutUIAdd)
	ON_BN_CLICKED(IDOK, &CPropertyTabLayoutUI::OnBnClickedOk)
END_MESSAGE_MAP()


// CPropertyTabLayoutUI message handlers

BOOL CPropertyTabLayoutUI::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString str;
	for(int i=0;i<m_pTablayoutUI->GetCount();i++)
	{
		CControlUI* pControl=m_pTablayoutUI->GetItemAt(i);
		bool bVisible=pControl->IsVisible();

		str.Format(_T("Tab%d"),i);
		m_lstTab.AddString(str);
		m_lstTab.SetCheck(i,bVisible);

		if(bVisible==true)
			CSingleCheckListBox::m_nLastChecked=i;
	}

	m_menuUI.LoadMenu(IDR_UI);
	m_btnAdd.m_hMenu=m_menuUI.GetSubMenu(0)->GetSafeHmenu();
	m_btnAdd.SizeToContent();
	m_btnAdd.m_bOSMenu=FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyTabLayoutUI::OnBnClickedButtonTabLayoutUIAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	int nClass;
	switch(m_btnAdd.m_nMenuResult)
	{
	case ID_UI_CONTROL_BUTTON:
		nClass=classButton;
		break;
	case ID_UI_CONTROL_CONTROL:
		nClass=classControl;
		break;
	case ID_UI_CONTROL_LABEL:
		nClass=classLabel;
		break;
	case ID_UI_CONTROL_TEXT:
		nClass=classText;
		break;
	case ID_UI_CONTROL_EDIT:
		nClass=classEdit;
		break;
	case ID_UI_CONTROL_OPTION:
		nClass=classOption;
		break;
	case ID_UI_CONTROL_COMBO:
		nClass=classCombo;
		break;
	case ID_UI_CONTROL_PROGRESS:
		nClass=classProgress;
		break;
	case ID_UI_CONTROL_SLIDER:
		nClass=classSlider;
		break;
	case ID_UI_CONTROL_ACTIVEX:
		nClass=classActiveX;
		break;
	case ID_UI_LAYOUT_VERTICAL:
		nClass=classVerticalLayout;
		break;
	case ID_UI_LAYOUT_HORIZONTAL:
		nClass=classHorizontalLayout;
		break;
	case ID_UI_LAYOUT_CONTAINER:
		nClass=classContainer;
		break;
	case ID_UI_LAYOUT_TILE:
		nClass=classTileLayout;
		break;
	case ID_UI_LAYOUT_TAB:
		nClass=classTabLayout;
		break;
	default:
		return;
	}

	CRect rect=m_pTablayoutUI->GetPos();
	rect.left=(rect.left+rect.right)/2;
	rect.top=(rect.top+rect.bottom)/2;
	rect.right=rect.left+UI_DEFAULT_WIDTH;
	rect.bottom=rect.top+UI_DEFAULT_HEIGHT;
	CControlUI* pNewControl=g_pMainFrame->GetActiveUIView()->NewUI(nClass,rect,m_pTablayoutUI);
	g_pClassView->InsertUITreeItem(pNewControl);

	CString str;
	int nCount=m_lstTab.GetCount();
	str.Format(_T("Tab%d"),nCount);
	m_lstTab.AddString(str);
	if(nCount==0)
		m_lstTab.SetCheck(0,1);
}

void CPropertyTabLayoutUI::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0;i<m_lstTab.GetCount();i++)
	{
		int nCheck=m_lstTab.GetCheck(i);

		if(nCheck==1)
		{
			m_pTablayoutUI->SelectItem(i);
			m_pTablayoutUI->SetPos(m_pTablayoutUI->GetPos());
			break;
		}
	}

	OnOK();
}
