#pragma once
#include "ViewTree.h"
#include "tinyxml.h"

//////////////////////////////////////////////////////////////////////////
//CFileViewTree
class CFileViewTree : public CTreeCtrl
{
	// 构造
public:
	CFileViewTree();
	virtual ~CFileViewTree();

	// 重写
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};

//////////////////////////////////////////////////////////////////////////
//CFileView

#define INFO_DIRECTORY 1
#define INFO_FILE      2
#define INFO_PROJECT   3

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
	friend class CFileViewTree;

// 构造
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// 属性
protected:

	CFileViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;

// 实现
public:
	virtual ~CFileView();

public:
	void OnProjectNew();
	void OnProjectClose();
	void OnProjectOpen();
	void SaveProject();

protected:
	BOOL DeleteSkinFile(HTREEITEM hItem);
	BOOL DeleteDirectory(HTREEITEM hItem);
	void OpenSkinFile(HTREEITEM hItem);
	void SaveUITree(HTREEITEM hItem, TiXmlElement* pParentNode);
	void LoadUITree(CString& strPath);
	void LoadUITree(TiXmlElement* pElement, HTREEITEM hParent);
	CDocument* FindSkinFile(CString& strPath);
	HTREEITEM FindDirectory(CString& strDirName, HTREEITEM hParent);
	BOOL CloseSkinFile(CString& strPath);
	BOOL RenameFile(CString strNewName, HTREEITEM hItem);

	void ExpandFileViewTree(HTREEITEM hItem, UINT nCode);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFileNew();
	afx_msg void OnFileDelete();
	afx_msg void OnFileOpen();
	afx_msg void OnDirectoryNew();
	afx_msg void OnCreateCopy();
	afx_msg void OnFileRename();
	afx_msg void OnTreeCollapse();
	afx_msg void OnTreeExpand();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

