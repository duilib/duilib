#include "stdafx.h"
#include "MainFrm.h"
#include "FileView.h"
#include "Resource.h"
#include "DuiDesigner.h"

#include "DialogProjectNew.h"
#include "UIUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileViewTree

CFileViewTree::CFileViewTree()
{
}

CFileViewTree::~CFileViewTree()
{
}

BEGIN_MESSAGE_MAP(CFileViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CFileViewTree::OnNMDblclk)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CFileViewTree::OnTvnEndlabeledit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileViewTree 消息处理程序

BOOL CFileViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CFileViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	g_pFileView->OpenSkinFile(GetSelectedItem());
}

void CFileViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CString strNewName = pTVDispInfo->item.pszText;
	CString strOldName = GetItemText(pTVDispInfo->item.hItem);
	if(!strNewName.IsEmpty() && g_pFileView->RenameFile(strNewName, pTVDispInfo->item.hItem))
		SetItemText(pTVDispInfo->item.hItem, strNewName);
}

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
	g_pFileView = this;
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDR_PROJECT_NEW, OnProjectNew)
	ON_COMMAND(IDR_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(IDR_FILE_NEW, &CMainFrame::OnUpdateProjectExist)
	ON_COMMAND(IDR_FILE_DELETE, OnFileDelete)
	ON_UPDATE_COMMAND_UI(IDR_FILE_DELETE, &CMainFrame::OnUpdateProjectExist)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_DIRECTORY_NEW, &CFileView::OnDirectoryNew)
	ON_COMMAND(ID_CREATE_COPY, &CFileView::OnCreateCopy)
	ON_COMMAND(ID_FILE_RENAME, &CFileView::OnFileRename)
	ON_COMMAND(ID_TREE_COLLAPSE, &CFileView::OnTreeCollapse)
	ON_COMMAND(ID_TREE_EXPAND, &CFileView::OnTreeExpand)
	ON_COMMAND(ID_PROJECT_CLOSE, &CMainFrame::OnProjectClose)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_CLOSE, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_CREATE_COPY, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_FILE_RENAME, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_TREE_COLLAPSE, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_TREE_EXPAND, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_OPEN, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &CMainFrame::OnUpdateProjectExist)
	ON_UPDATE_COMMAND_UI(ID_DIRECTORY_NEW, &CMainFrame::OnUpdateProjectExist)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_EDITLABELS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROJECT_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_PROJECT_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProjectNew()
{
	CDialogProjectNew dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(CGlobalVariable::m_bIsProjectExist)
			OnProjectClose();

		CGlobalVariable::m_strProjectName = dlg.m_strName;
		CGlobalVariable::m_strProjectPath = dlg.m_strPath + dlg.m_strName + _T("\\");

		CreateDirectory(CGlobalVariable::m_strProjectPath, NULL);
		CGlobalVariable::m_bIsProjectExist = true;

		HTREEITEM hRoot = m_wndFileView.InsertItem(CGlobalVariable::m_strProjectName, 0, 0, TVI_ROOT);
		m_wndFileView.SetItemData(hRoot, INFO_PROJECT);
		m_wndFileView.SetItemState(hRoot, TVIS_BOLD,TVIS_BOLD);
	}
}

void CFileView::OnProjectClose()
{
	if(CGlobalVariable::m_bIsProjectExist == false)
		return;

	g_pMainFrame->OnFileCloseAll();
	SaveProject();
	m_wndFileView.DeleteAllItems();
	CGlobalVariable::m_bIsProjectExist = false;
}

void CFileView::OnProjectOpen()
{
	CFileDialog dlg(TRUE, _T(""), NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT
		, _T("项目文件(*.uiproj)|*.uiproj|所有文件(*.*)|*.*||"));
	if(dlg.DoModal() == IDOK)
	{
		if(CGlobalVariable::m_bIsProjectExist)
			OnProjectClose();

		CGlobalVariable::m_strProjectName = dlg.GetFileTitle();
		CString strPath = dlg.GetPathName();
		int nPos = strPath.ReverseFind(_T('\\'));
		CGlobalVariable::m_strProjectPath = strPath.Left(nPos + 1);

		LoadUITree(strPath);
		CGlobalVariable::m_bIsProjectExist = true;
	}
}

void CFileView::LoadUITree(CString& strPath)
{
	if(strPath.IsEmpty())
		return;

	TiXmlDocument xmlDoc;
	xmlDoc.LoadFile(StringConvertor::WideToAnsi(strPath));
	
	TiXmlElement* pRootElem = xmlDoc.RootElement();
	HTREEITEM hRoot = m_wndFileView.InsertItem(CGlobalVariable::m_strProjectName, 0, 0, TVI_ROOT);
	m_wndFileView.SetItemData(hRoot, INFO_PROJECT);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD,TVIS_BOLD);
	LoadUITree(pRootElem, hRoot);
	m_wndFileView.Expand(hRoot, TVE_EXPAND);
}

void CFileView::LoadUITree(TiXmlElement* pElement, HTREEITEM hParent)
{
	if(pElement == NULL)
		return;

	TiXmlNode* pNode = pElement->FirstChild();
	while(pNode)
	{
		if(pNode->Type() == TiXmlNode::ELEMENT)
		{
			DWORD dwInfo = 0;
			CStringA strName = pNode->ToElement()->Attribute("Name");
			HTREEITEM hItem;
			if(strcmp(pNode->Value(), "File") == 0)
			{
				hItem = m_wndFileView.InsertItem(StringConvertor::Utf8ToWide(strName), 2, 2, hParent);
				dwInfo = INFO_FILE;
			}
			else if(strcmp(pNode->Value(), "Directory") == 0)
			{
				hItem = m_wndFileView.InsertItem(StringConvertor::Utf8ToWide(strName), 1, 1, hParent);
				dwInfo = INFO_DIRECTORY;
			}
			m_wndFileView.SetItemData(hItem, dwInfo);
			if(dwInfo == INFO_DIRECTORY)
			{
				LoadUITree(pNode->ToElement(), hItem);
			}
		}

		pNode = pNode->NextSibling();
	}
}

void CFileView::SaveProject()
{
	if(CGlobalVariable::m_bIsProjectExist == false)
		return;

	CString strPathName = CGlobalVariable::m_strProjectPath + CGlobalVariable::m_strProjectName + _T(".uiproj");
	TiXmlDocument xmlDoc(StringConvertor::WideToAnsi(strPathName));
	TiXmlDeclaration Declaration("1.0","utf-8","yes");
	xmlDoc.InsertEndChild(Declaration);

	TiXmlElement xmlElem("DirectUIProject");
	xmlElem.SetAttribute("Name", StringConvertor::WideToUtf8(CGlobalVariable::m_strProjectName));
	xmlElem.SetAttribute("Version", StringConvertor::WideToUtf8(UIDESIGNER_VERSION));
	TiXmlNode* pNode = xmlDoc.InsertEndChild(xmlElem);

	SaveUITree(m_wndFileView.GetRootItem(), pNode->ToElement());
	xmlDoc.SaveFile();
}

void CFileView::SaveUITree(HTREEITEM hItem, TiXmlElement* pParentNode)
{
	if(hItem == NULL || pParentNode == NULL)
		return;

	TiXmlElement* pNode = NULL;
	HTREEITEM hChild = m_wndFileView.GetChildItem(hItem);
	while(hChild != NULL)
	{
		DWORD dwInfo = m_wndFileView.GetItemData(hChild);
		CString strName = m_wndFileView.GetItemText(hChild);
		HTREEITEM hNext = m_wndFileView.GetNextItem(hChild, TVGN_NEXT);
		if(dwInfo == INFO_FILE)
		{
			 pNode = new TiXmlElement("File");
			 pNode->SetAttribute("Name", StringConvertor::WideToUtf8(strName));
			 pParentNode->InsertEndChild(*pNode);

			 delete pNode;
			 pNode = NULL;
		}
		else if(dwInfo == INFO_DIRECTORY)
		{
			pNode = new TiXmlElement("Directory");
			pNode->SetAttribute("Name", StringConvertor::WideToUtf8(strName));
			TiXmlNode* pChildNode = pParentNode->InsertEndChild(*pNode);
			SaveUITree(hChild, pChildNode->ToElement());

			delete pNode;
			pNode = NULL;
		}

		hChild = hNext;
	}
}

void CFileView::OnFileNew()
{
	CMultiDocTemplate* pUIDocTemplate = theApp.GetUIDocTemplate();
	CDocument* pDoc = pUIDocTemplate->OpenDocumentFile(NULL);
	CString strFilePath = CGlobalVariable::m_strProjectPath + pDoc->GetTitle() + _T(".xml");
	pDoc->DoSave(strFilePath);
	HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
	DWORD dwInfo = m_wndFileView.GetItemData(hSelectedItem);
	HTREEITEM hParent = (dwInfo != INFO_FILE) ? hSelectedItem : m_wndFileView.GetParentItem(hSelectedItem);
	HTREEITEM hNewItem = m_wndFileView.InsertItem(pDoc->GetTitle(), 2, 2, hParent);
	m_wndFileView.SetItemData(hNewItem, INFO_FILE);
	m_wndFileView.Expand(hParent, TVE_EXPAND);
}

void CFileView::OnFileDelete()
{
	HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
	DWORD dwInfo = m_wndFileView.GetItemData(hSelectedItem);
	if(dwInfo == INFO_FILE)
	{
		if(MessageBox(_T("您确定删除此文件？\n点击“是”，确认删除，“否”取消删除。"),
			_T("警告"), MB_YESNO | MB_ICONWARNING) == IDNO)
			return;

		DeleteSkinFile(hSelectedItem);
	}
	else if(dwInfo == INFO_DIRECTORY)
	{
		if(MessageBox(_T("您确定该目录中的所有内容，删除后将无法恢复？\n点击“是”，确认删除，“否”取消删除。"),
			_T("警告"), MB_YESNO | MB_ICONWARNING) == IDNO)
			return;

		DeleteDirectory(hSelectedItem);
	}
}

BOOL CFileView::DeleteSkinFile(HTREEITEM hItem)
{
	CString strFilePath;
	strFilePath = CGlobalVariable::m_strProjectPath + m_wndFileView.GetItemText(hItem);
	CloseSkinFile(strFilePath);
	if(DeleteFile(strFilePath))
	{
		m_wndFileView.DeleteItem(hItem);
		return TRUE;
	}

	return FALSE;
}

BOOL CFileView::DeleteDirectory(HTREEITEM hItem)
{
	HTREEITEM hChild = m_wndFileView.GetChildItem(hItem);
	while(hChild != NULL)
	{
		DWORD dwInfo = m_wndFileView.GetItemData(hChild);
		HTREEITEM hNext = m_wndFileView.GetNextItem(hChild, TVGN_NEXT);
		if(dwInfo == INFO_FILE)
			DeleteSkinFile(hChild);
		else if(dwInfo == INFO_DIRECTORY)
			DeleteDirectory(hChild);

		hChild = hNext;
	}

	m_wndFileView.DeleteItem(hItem);
	return FALSE;
}

void CFileView::OpenSkinFile(HTREEITEM hItem)
{
	if(m_wndFileView.GetItemData(hItem) != INFO_FILE)
		return;

	CString strFilePath = CGlobalVariable::m_strProjectPath + m_wndFileView.GetItemText(hItem);
	AfxGetApp()->OpenDocumentFile(strFilePath);
}

CDocument* CFileView::FindSkinFile(CString& strPath)
{
	CDocTemplate* pDocTemplate = theApp.GetUIDocTemplate();
	POSITION posDoc = pDocTemplate->GetFirstDocPosition();
	while (posDoc != NULL)
	{
		CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
		if ((pDoc != NULL) && (pDoc->GetPathName() == strPath))
		{
			return pDoc;
		}
	}

	return NULL;
}

HTREEITEM CFileView::FindDirectory(CString& strDirName, HTREEITEM hParent)
{
	HTREEITEM hChild = m_wndFileView.GetChildItem(hParent);
	while(hChild != NULL)
	{
		DWORD dwInfo = m_wndFileView.GetItemData(hChild);
		if(dwInfo == INFO_DIRECTORY)
		{
			CString strName = m_wndFileView.GetItemText(hChild);
			if(strDirName == strName)
				return hChild;
		}

		hChild = m_wndFileView.GetNextItem(hChild, TVGN_NEXT);;
	}

	return NULL;
}

BOOL CFileView::CloseSkinFile(CString& strPath)
{
	CDocument* pDoc = FindSkinFile(strPath);
	if(pDoc == NULL)
		return FALSE;

	pDoc->OnCloseDocument();

	return TRUE;
}

void CFileView::OnFileOpen()
{
	// TODO: 在此处添加命令处理程序代码
	HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
	OpenSkinFile(hSelectedItem);
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROJECT_EXPLORER_24 : IDR_PROJECT_EXPLORER, 0, 0, TRUE /* 锁定 */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

void CFileView::OnDirectoryNew()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
	DWORD dwInfo = m_wndFileView.GetItemData(hSelectedItem);
	HTREEITEM hParent = (dwInfo != INFO_FILE) ? hSelectedItem:m_wndFileView.GetParentItem(hSelectedItem);
	int nCount = 2;
	CString strDirName = _T("新建文件夹");
	while(FindDirectory(strDirName, hParent))
		strDirName.Format(_T("新建文件夹(%d)"), nCount++);

	HTREEITEM hNewItem = m_wndFileView.InsertItem(strDirName, 1, 1, hParent);
	m_wndFileView.SetItemData(hNewItem, INFO_DIRECTORY);
	m_wndFileView.Expand(hParent, TVE_EXPAND);
	m_wndFileView.SelectItem(hNewItem);
	m_wndFileView.EditLabel(hNewItem);
}

void CFileView::OnCreateCopy()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
	if(m_wndFileView.GetItemData(hSelectedItem) != INFO_FILE)
		return;

	CString strFileName= m_wndFileView.GetItemText(hSelectedItem);
	CString strNewFileName = strFileName;
	strNewFileName.Insert(0, _T("复件 "));
	int nCount = 0;
	while(!CopyFile(CGlobalVariable::m_strProjectPath + strFileName,
		CGlobalVariable::m_strProjectPath + strNewFileName, TRUE))
	{
		strNewFileName = strFileName;
		CString strCount;
		strCount.Format(_T("复件(%d) "), nCount++);
		strNewFileName.Insert(0, strCount);
	}
	HTREEITEM hParent  = m_wndFileView.GetParentItem(hSelectedItem);
	HTREEITEM hNewItem = m_wndFileView.InsertItem(strNewFileName, 2, 2, hParent);
	m_wndFileView.SetItemData(hNewItem, INFO_FILE);
}

void CFileView::OnFileRename()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelectedItem = m_wndFileView.GetSelectedItem();
	m_wndFileView.EditLabel(hSelectedItem);
}

BOOL CFileView::RenameFile(CString strNewName, HTREEITEM hItem)
{
	CString strOldName = m_wndFileView.GetItemText(hItem);
	DWORD dwInfo = m_wndFileView.GetItemData(hItem);
	if(dwInfo == INFO_FILE)
	{
		if(FindSkinFile(CGlobalVariable::m_strProjectPath + strOldName))
		{
			MessageBox(_T("此文件正处于打开状态，请先关闭后再进行重命名。"), _T("提示"), MB_ICONINFORMATION);
			return FALSE;
		}
		if(!MoveFile(CGlobalVariable::m_strProjectPath + strOldName
			, CGlobalVariable::m_strProjectPath + strNewName))
		{
			MessageBox(_T("此文件已经存在，不能重复命名。"), _T("提示"), MB_ICONINFORMATION);
			return FALSE;
		}
	}
	else if(dwInfo == INFO_DIRECTORY)
	{
		if(FindDirectory(strNewName, m_wndFileView.GetParentItem(hItem)))
		{
			MessageBox(_T("此目录已经存在，不能重复命名。"), _T("提示"), MB_ICONINFORMATION);
			return FALSE;
		}
	}

	return TRUE;
}

void CFileView::OnTreeCollapse()
{
	// TODO: 在此添加命令处理程序代码
	ExpandFileViewTree(m_wndFileView.GetRootItem(), TVE_COLLAPSE);
}

void CFileView::OnTreeExpand()
{
	// TODO: 在此添加命令处理程序代码
	ExpandFileViewTree(m_wndFileView.GetRootItem(), TVE_EXPAND);
}

void CFileView::ExpandFileViewTree(HTREEITEM hItem, UINT nCode)
{
	HTREEITEM hChild = m_wndFileView.GetChildItem(hItem);
	while(hChild != NULL)
	{
		ExpandFileViewTree(hChild, nCode);
		m_wndFileView.Expand(hItem, nCode);
		hChild = m_wndFileView.GetNextItem(hChild, TVGN_NEXT);
	}
}
void CFileView::OnDestroy()
{
	CDockablePane::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	SaveProject();
}
