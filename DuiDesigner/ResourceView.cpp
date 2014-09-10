// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "DuiDesigner.h"
#include "MainFrm.h"
#include "ResourceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CResourceViewBar::CResourceViewBar()
{
	g_pResourceView = this;
}

CResourceViewBar::~CResourceViewBar()
{
	m_mapTree.RemoveAll();

	POSITION pos;
	CString strKey;
	CStringArray* pstrArray;
	for(pos=m_mapImageArray.GetStartPosition(); pos != NULL; )
	{
		m_mapImageArray.GetNextAssoc(pos, strKey, (void*&)pstrArray);
		delete pstrArray;
	}
	m_mapImageArray.RemoveAll();
}

BEGIN_MESSAGE_MAP(CResourceViewBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

int CResourceViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndResourceView.Create(dwViewStyle, rectDummy, this, 3))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_ResourceViewImages.Create(IDB_RESOURCE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndResourceView.SetImageList(&m_ResourceViewImages, TVSIL_NORMAL);

	// Fill view context(dummy code, don't seek here something magic :-)):
	InitResourceView();

	OnChangeVisualStyle();
	return 0;
}

void CResourceViewBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	//if (CanAdjustLayout())
	{
		m_wndResourceView.SetWindowPos(NULL, 1, 1, cx - 2, cy - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CResourceViewBar::InitResourceView()
{
}

void CResourceViewBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndResourceView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
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
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_RESOURCE, point.x, point.y, this, TRUE);
}

void CResourceViewBar::OnEditCut()
{
	// TODO: Add your command handler code here

}

void CResourceViewBar::OnEditCopy()
{
	// TODO: Add your command handler code here

}

void CResourceViewBar::OnEditPaste()
{
	// TODO: Add your command handler code here

}

void CResourceViewBar::OnEditClear()
{
	// TODO: Add your command handler code here

}

void CResourceViewBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndResourceView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CResourceViewBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndResourceView.SetFocus();
}

void CResourceViewBar::OnChangeVisualStyle()
{
	m_ResourceViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_RESOURCE_VIEW24 : IDB_RESOURCE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ResourceViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ResourceViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndResourceView.SetImageList(&m_ResourceViewImages, TVSIL_NORMAL);
}

void CResourceViewBar::InsertImageTree(CString strTitle, CString strPath)
{
	HTREEITEM hRoot = m_wndResourceView.InsertItem(strTitle, 0, 0, TVI_ROOT);
	m_wndResourceView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	m_mapTree.SetAt(strTitle, (void*)hRoot);
	CStringArray* pstrArray = new CStringArray;
	m_mapImageArray.SetAt(strTitle, (void*)pstrArray);

	if(strPath.IsEmpty())
		return;

	int nPos = strPath.ReverseFind('\\');
	if(nPos == -1)
		return;
	CString strDir = strPath.Left(nPos + 1);
	WIN32_FIND_DATA FindFileData = {0};
	CString strFind = strDir + _T("*.*");
	HANDLE hFind = ::FindFirstFile(strFind, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;
	do
	{
		CString strExt = _tcsrchr(FindFileData.cFileName, _T('.')) + 1;
		if(strExt.IsEmpty())
			continue;
		int nType = 0;
		if(strExt == _T("bmp"))
			nType = 1;
		else if(strExt == _T("jpg"))
			nType = 2;
		else if(strExt == _T("png"))
			nType = 3;
		if(nType > 0)
		{
			pstrArray->Add(strDir + FindFileData.cFileName);
			HTREEITEM hItem = m_wndResourceView.InsertItem(FindFileData.cFileName, nType, nType, hRoot);
		}
	}while(::FindNextFile(hFind, &FindFileData));
	::FindClose(hFind);
	m_wndResourceView.Expand(hRoot, TVE_EXPAND);
}

void CResourceViewBar::RemoveImageTree(CString strTree)
{
	HTREEITEM hTree;
	if(!m_mapTree.Lookup(strTree, (void*&)hTree))
		return;

	m_wndResourceView.DeleteItem(hTree);
	m_mapTree.RemoveKey(strTree);

	CStringArray* pstrArray = NULL;
	if(!m_mapImageArray.Lookup(strTree, (void*&)pstrArray))
		return;

	m_mapImageArray.RemoveKey(strTree);
	delete pstrArray;
}

void  CResourceViewBar::RenameImageTree(LPCTSTR pstrTree, LPCTSTR pstrNewName)
{
	HTREEITEM hTree;
	if(!m_mapTree.Lookup(pstrTree, (void*&)hTree))
		return;

	m_wndResourceView.SetItemText(hTree, pstrNewName);
	m_mapTree.RemoveKey(pstrTree);
	m_mapTree.SetAt(pstrNewName, (void*)hTree);

	CStringArray* pstrArray = NULL;
	if(!m_mapImageArray.Lookup(pstrTree, (void*&)pstrArray))
		return;

	m_mapImageArray.RemoveKey(pstrTree);
	m_mapImageArray.SetAt(pstrNewName, (void*)pstrArray);
}

void CResourceViewBar::InsertImage(CString strImage, CString strTree)
{
	if(strImage.IsEmpty())
		return;
	HTREEITEM hTree;
	if(!m_mapTree.Lookup(strTree, (void*&)hTree))
		return;
	CStringArray* pstrArray = NULL;
	if(!m_mapImageArray.Lookup(strTree, (void*&)pstrArray))
		return;

	for(int i=0; i<pstrArray->GetSize(); i++)
	{
		if((*pstrArray)[i] == strImage)
			return;
	}

	int nPos = strImage.ReverseFind('\\');
	if(nPos == -1)
		return;
	CString strName = strImage.Right(strImage.GetLength() - nPos - 1);
	nPos = strName.ReverseFind('.');
	if(nPos == -1)
		return;
	CString strExt = strName.Right(strName.GetLength() - nPos - 1);
	int nType = 0;
	if(strExt == _T("bmp"))
		nType = 1;
	else if(strExt == _T("jpg"))
		nType = 2;
	else if(strExt == _T("png"))
		nType = 3;
	m_wndResourceView.InsertItem(strName, nType, nType, hTree);
	pstrArray->Add(strImage);
}

const CStringArray* CResourceViewBar::GetImageTree(CString strTree) const
{

	CStringArray* pstrArray = NULL;
	m_mapImageArray.Lookup(strTree, (void*&)pstrArray);

	return pstrArray;
}

void CResourceViewBar::CopyImageToSkinDir(LPCTSTR pstrSkinDir, LPCTSTR pstrTree)
{
	CStringArray* pstrArray = NULL;
	if(!m_mapImageArray.Lookup(pstrTree, (void*&)pstrArray))
		return;

	for(int i=0; i<pstrArray->GetSize(); i++)
	{
		CString strPath = (*pstrArray)[i];
		int nPos = strPath.ReverseFind('\\');
		if(nPos == -1)
			continue;

		CString strDir = strPath.Left(nPos + 1);
		if(strDir != pstrSkinDir)
		{
			CString strName = strPath.Right(strPath.GetLength() - nPos - 1);
			CopyFile(strPath, pstrSkinDir + strName, TRUE);
		}
	}
}
