
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "PropertiesWnd.h"
#include "ResourceView.h"
#include "ToolBoxWnd.h"
#include "UIDesignerView.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CUIDesignerView* GetActiveUIView() const;

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	CResourceViewBar m_wndResourceView;
	CPropertiesWnd    m_wndProperties;
	CToolBoxWnd       m_wndToolBox;

	CMFCToolBar m_wndToolbarFormEdit;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnGetTabToolTip(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnChangeActiveTab(WPARAM wp,LPARAM lp);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);

	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertTabGroup();

public:
	afx_msg void OnProjectNew();
	afx_msg void OnProjectOpen();
	afx_msg void OnProjectClose();
	afx_msg void OnTemplateOpen();
	afx_msg void OnFileCloseAll();
	afx_msg void OnFileSaveAll();
	afx_msg void OnUpdateProjectExist(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileCloseAll(CCmdUI *pCmdUI);
	afx_msg void OnAppCheckUpdate();

	afx_msg void OnViewControls();
	afx_msg void OnViewResource();
	afx_msg void OnViewProject();
	afx_msg void OnViewToolsBox();
	afx_msg void OnViewProperty();
/*
	bool m_bViewControls;
	bool m_bViewResources;
	bool m_bViewProject;
	bool m_bViewToolsBox;
	bool m_bViewProperty;
*/
};