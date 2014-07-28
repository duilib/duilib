// DialogDefaultAttribList.cpp : implementation file
//
#include "stdafx.h"
#include "DuiDesigner.h"
#include "DialogDefaultAttribList.h"
#include "DialogUIAttribEdit.h"
#include "LayoutManager.h"
#include "UIUtil.h"

//////////////////////////////////////////////////////////////////////////
// CDialogDefaultAttribList dialog

IMPLEMENT_DYNAMIC(CDialogDefaultAttribList, CDialog)

CDialogDefaultAttribList::CDialogDefaultAttribList(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDefaultAttribList::IDD, pParent)
{
	m_pManager = NULL;
}

CDialogDefaultAttribList::~CDialogDefaultAttribList()
{
}

void CDialogDefaultAttribList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_UI_PROP_LOCATION, m_wndUIPropLocation);
	DDX_Control(pDX, IDC_LIST_DEFAULT_ATTRIB, m_lstDefaultAttrib);
	DDX_Control(pDX, IDC_BUTTON_ATTRIB_ADD, m_btnAdd);
}


BEGIN_MESSAGE_MAP(CDialogDefaultAttribList, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ATTRIB_ADD, &CDialogDefaultAttribList::OnBnClickedButtonAttribAdd)
	ON_BN_CLICKED(IDC_BUTTON_ATTRIB_DELETE, &CDialogDefaultAttribList::OnBnClickedButtonAttribDelete)
	ON_BN_CLICKED(IDC_BUTTON_ATTRIB_MODIFY, &CDialogDefaultAttribList::OnBnClickedButtonAttribModify)
	ON_LBN_SELCHANGE(IDC_LIST_DEFAULT_ATTRIB, &CDialogDefaultAttribList::OnLbnSelchangeListDefaultAttrib)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_AS_STYLE, &CDialogDefaultAttribList::OnBnClickedButtonSaveAsStyle)
END_MESSAGE_MAP()


// CDialogDefaultAttribList message handlers

BOOL CDialogDefaultAttribList::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rectUIProp;
	m_wndUIPropLocation.GetClientRect(&rectUIProp);
	m_wndUIPropLocation.MapWindowPoints(this, &rectUIProp);
	m_wndUIProperties.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER, rectUIProp, this, ID_UI_PROP_WND);

	m_menuUI.LoadMenu(IDR_UI);
	m_btnAdd.m_hMenu = m_menuUI.GetSubMenu(0)->GetSafeHmenu();
	m_btnAdd.SizeToContent();
	m_btnAdd.m_bOSMenu = FALSE;

#ifdef _DEBUG
	this->GetDlgItem(IDC_BUTTON_SAVE_AS_STYLE)->ShowWindow(SW_SHOW);
#endif

	m_pManager = g_pMainFrame->GetActiveUIView()->GetPaintManager();
	ASSERT(m_pManager);
	const CStdStringPtrMap& DefaultAttribHash = m_pManager->GetDefaultAttribultes();
	for(int i=0; i<DefaultAttribHash.GetSize(); i++)
	{
		LPCTSTR pstrName = DefaultAttribHash.GetAt(i);
		m_lstDefaultAttrib.AddString(pstrName);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDefaultAttribList::OnBnClickedButtonAttribAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	int nClass;
	switch(m_btnAdd.m_nMenuResult)
	{
	case ID_UI_CONTROL_BUTTON:
		nClass = classButton;
		break;
	case ID_UI_CONTROL_CONTROL:
		nClass = classControl;
		break;
	case ID_UI_CONTROL_LABEL:
		nClass = classLabel;
		break;
	case ID_UI_CONTROL_TEXT:
		nClass = classText;
		break;
	case ID_UI_CONTROL_EDIT:
		nClass = classEdit;
		break;
	case ID_UI_CONTROL_OPTION:
		nClass = classOption;
		break;
	case ID_UI_CONTROL_COMBO:
		nClass = classCombo;
		break;
	case ID_UI_CONTROL_PROGRESS:
		nClass = classProgress;
		break;
	case ID_UI_CONTROL_SLIDER:
		nClass = classSlider;
		break;
	case ID_UI_CONTROL_ACTIVEX:
		nClass = classActiveX;
		break;
	case ID_UI_LAYOUT_VERTICAL:
		nClass = classVerticalLayout;
		break;
	case ID_UI_LAYOUT_HORIZONTAL:
		nClass = classHorizontalLayout;
		break;
	case ID_UI_LAYOUT_CONTAINER:
		nClass = classContainer;
		break;
	case ID_UI_LAYOUT_TILE:
		nClass = classTileLayout;
		break;
	case ID_UI_LAYOUT_TAB:
		nClass = classTabLayout;
		break;
	default:
		return;
	}

	CControlUI* pControl = CLayoutManager::NewUI(nClass, CRect(0,0,0,0), NULL, NULL);
	CString strName = pControl->GetClass();
	strName = strName.Left(strName.GetLength() - 2);
	if(m_pManager->GetDefaultAttributeList(strName) != NULL)
	{
		MessageBox(_T("此控件的默认属性已经存在，无法创建！"), _T("提示"), MB_ICONINFORMATION);
		CLayoutManager::DeleteUI(pControl);
		return;
	}

	CDialogUIAttribEdit dlg(pControl);
	if(dlg.DoModal() == IDOK)
	{
		CString strValue;
		if(GetDefaultAttrib(pControl, strValue) == FALSE)
		{
			MessageBox(_T("创建默认属性失败！"), _T("提示"), MB_ICONINFORMATION);
			CLayoutManager::DeleteUI(pControl);
			return;
		}
		m_pManager->AddDefaultAttributeList(strName, strValue);
		int nIndex = m_lstDefaultAttrib.AddString(strName);
		m_lstDefaultAttrib.SetItemDataPtr(nIndex, pControl);
		m_lstDefaultAttrib.SetCurSel(nIndex);
		m_wndUIProperties.ShowProperty(pControl);
		g_pMainFrame->GetActiveUIView()->SetModifiedFlag();
	}
	else
		CLayoutManager::DeleteUI(pControl);
}

void CDialogDefaultAttribList::OnBnClickedButtonAttribDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_lstDefaultAttrib.GetCurSel();
	if(nIndex == -1)
		return;

	if(MessageBox(_T("默认属性删除后，将无法还原！\n您确定要删除此默认属性？")
		, _T("提示"), MB_ICONINFORMATION | MB_YESNO) == IDNO)
		return;

	CString strUIName;
	m_lstDefaultAttrib.GetText(nIndex, strUIName);
	if(m_pManager->RemoveDefaultAttributeList(strUIName))
	{
		CControlUI* pControl = (CControlUI*)m_lstDefaultAttrib.GetItemDataPtr(nIndex);
		CLayoutManager::DeleteUI(pControl);
		m_lstDefaultAttrib.DeleteString(nIndex);
		MessageBox(_T("已成功将此默认属性删除，请重新打开此UI文件，更新显示！")
			, _T("提示"), MB_ICONINFORMATION);
		m_wndUIProperties.ShowProperty(NULL);
		g_pMainFrame->GetActiveUIView()->SetModifiedFlag();
	}
}

void CDialogDefaultAttribList::OnBnClickedButtonAttribModify()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_lstDefaultAttrib.GetCurSel();
	if(nIndex == -1)
		return;

	CString strDefaultAttribName;
	m_lstDefaultAttrib.GetText(nIndex, strDefaultAttribName);
	LPCTSTR pstrDefaultAttrib = m_pManager->GetDefaultAttributeList(strDefaultAttribName);
	CControlUI* pControl = (CControlUI*)m_lstDefaultAttrib.GetItemDataPtr(nIndex);
	if(pControl == NULL)
	{
		int nClass = gGetUIClass(strDefaultAttribName + _T("UI"));
		if(nClass == classPointer)
		{
			MessageBox(_T("无法识别此控件！"), _T("提示"), MB_ICONINFORMATION);
			return;
		}
		pControl = CLayoutManager::NewUI(nClass, CRect(0, 0, 0, 0), NULL, NULL);
		ASSERT(pControl);
		pControl->ApplyAttributeList(pstrDefaultAttrib);
		m_lstDefaultAttrib.SetItemDataPtr(nIndex, pControl);
	}

	CControlUI* pCopyControl = CLayoutManager::CloneControl(pControl);
	CDialogUIAttribEdit dlg(pCopyControl);
	if(dlg.DoModal() == IDOK)
	{
		CString strValue;
		if(GetDefaultAttrib(pCopyControl, strValue) == FALSE)
		{
			MessageBox(_T("修改默认属性失败！"), _T("提示"), MB_ICONINFORMATION);
			delete pCopyControl;
			return;
		}
		m_pManager->RemoveDefaultAttributeList(strDefaultAttribName);
		m_pManager->AddDefaultAttributeList(strDefaultAttribName, strValue);

		delete pControl;
		m_lstDefaultAttrib.SetItemDataPtr(nIndex, pCopyControl);
		m_wndUIProperties.ShowProperty(pCopyControl);
		g_pMainFrame->GetActiveUIView()->SetModifiedFlag();
	}
	else
		delete pCopyControl;
}

void CDialogDefaultAttribList::OnLbnSelchangeListDefaultAttrib()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_lstDefaultAttrib.GetCurSel();
	if(nIndex == -1)
		return;

	CString strUIName;
	m_lstDefaultAttrib.GetText(nIndex, strUIName);
	LPCTSTR pstrDefaultAttrib = m_pManager->GetDefaultAttributeList(strUIName);
	CControlUI* pControl = (CControlUI*)m_lstDefaultAttrib.GetItemDataPtr(nIndex);
	if(pControl == NULL)
	{
		strUIName += _T("UI");
		int nClass = gGetUIClass(strUIName);
		if(nClass == classPointer)
		{
			MessageBox(_T("无法识别此控件！"), _T("提示"), MB_ICONINFORMATION);
			return;
		}
		pControl = CLayoutManager::NewUI(nClass, CRect(0, 0, 0, 0), NULL, NULL);
		ASSERT(pControl);
		m_lstDefaultAttrib.SetItemDataPtr(nIndex, pControl);
		pControl->ApplyAttributeList(pstrDefaultAttrib);
	}
	m_wndUIProperties.ShowProperty(pControl);
}

void CDialogDefaultAttribList::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	for(int i=0; i<m_lstDefaultAttrib.GetCount(); i++)
	{
		CControlUI* pControl = (CControlUI*)m_lstDefaultAttrib.GetItemDataPtr(i);
		CLayoutManager::DeleteUI(pControl);
	}
}

BOOL CDialogDefaultAttribList::GetDefaultAttrib(CControlUI* pControl, CString& strValue)
{
	TiXmlElement xmlElement("DefaultAtrib");
	CLayoutManager::SaveProperties(pControl, &xmlElement, FALSE);

	TiXmlNode* pNode;
	TiXmlAttribute* pAttrib;
	pNode = xmlElement.FirstChild();
	if(pNode->Type() != TiXmlNode::ELEMENT)
	{
		return FALSE;
	}

	CStringA strValueA;
	CStringA strA;
	pAttrib = pNode->ToElement()->FirstAttribute();
	while(pAttrib)
	{
		const char* pstrName = pAttrib->Name();
		if(strcmp(pstrName, "name") != 0 && strcmp(pstrName, "pos") != 0)
		{
			strA.Format("%s=\"%s\" ", pstrName, pAttrib->Value());
			strValueA += strA;
		}
		pAttrib = pAttrib->Next();
	}

	strValue = StringConvertor::Utf8ToWide(strValueA);
	return TRUE;
}

void CDialogDefaultAttribList::OnBnClickedButtonSaveAsStyle()
{
	// TODO: Add your control notification handler code here
	g_pMainFrame->GetActiveUIView()->OnStyleSaveAs();
}
