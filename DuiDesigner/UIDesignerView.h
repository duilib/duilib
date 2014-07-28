// UIDesignerView.h : CUIDesignerView 类的接口
//
#pragma once
#include "..\DuiLib\UIlib.h"
#include "UIDesignerDoc.h"
#include "MultiUITracker.h"
#include "LayoutManager.h"
#include "UICommandHistory.h"

//////////////////////////////////////////////////////////////////////////
//CUIDesignerView

class CUIDesignerView : public CScrollView, public INotifyUI
{
protected: // 仅从序列化创建
	CUIDesignerView();
	DECLARE_DYNCREATE(CUIDesignerView)

// 属性
public:
	CUIDesignerDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDrawDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CUIDesignerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void Notify(TNotifyUI& msg);
	void SelectUI(CControlUI* pControl);
	void OnActivated();
	void ReDrawForm();
	CPaintManagerUI* GetPaintManager() { return m_LayoutManager.GetManager(); }

	CControlUI* NewUI(int nClass,CRect& rect,CControlUI* pParent);
	BOOL DeleteUI(CControlUI* pControl);
	void RedoUI(CControlUI* pControl, CControlUI* pParent);
	void ClearUITracker() { m_MultiTracker.RemoveAll(); }

	void OnShowPropertyDialog();

	void SaveSkinFile(LPCTSTR pstrPathName);
	BOOL SaveSkinImage(LPCTSTR pstrPathName);
	void OnTitleChanged(LPCTSTR pstrOldTitle, LPCTSTR pstrNewTitle);
	void SetModifiedFlag(BOOL bModified = TRUE);

protected:
	void UpDateDPtoLPOffset();
	CTrackerElement* CreateTracker(CControlUI* pControl);
	int GetControlType(CControlUI* pControl);

	void ShowPropertyDialog(CControlUI* pControl);

	void OnMicoMoveUp();
	void OnMicoMoveDown();
	void OnMicoMoveLeft();
	void OnMicoMoveRight();

	void InitUI(CControlUI* pControl, int depth);
	void CopyUI(TiXmlElement* pParentNode);
	void PasteUI(LPCTSTR xml);

	void RemoveForm(CArray<CControlUI*,CControlUI*>& arrSelected);

private:
	CLayoutManager m_LayoutManager;
	CMultiUITracker m_MultiTracker;
	CUICommandHistory m_UICommandHistory;

	CPoint m_ptDPtoLP;//Device coordinates to Logical coordinates
	bool m_bInit;

	static CLIPFORMAT m_cfUI;//custom clipboard format

	CBrush m_brHatch;
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnFormEditTest();
	afx_msg void OnAlignLeft();
	afx_msg void OnAlignRight();
	afx_msg void OnAlignTop();
	afx_msg void OnAlignBottom();
	afx_msg void OnAlignCenterVertically();
	afx_msg void OnAlignCenterHorizontally();
	afx_msg void OnAlignHorizontal();
	afx_msg void OnAlignVertical();
	afx_msg void OnAlignSameWidth();
	afx_msg void OnAlignSameHeight();
	afx_msg void OnAlignSameSize();
	afx_msg void OnShowGrid();
	afx_msg void OnShowAuxBorder();
	afx_msg void OnUpdateFormEditTest(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignBottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignCenterVertically(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignCenterHorizontally(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignHorizontal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignVertical(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignSameWidth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignSameHeight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAlignSameSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAuxBorder(CCmdUI* pCmdUI);

	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnDeleteUI();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateNeedSel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNeedClip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

	afx_msg void OnDestroy();
	afx_msg void OnTemplateSaveAs();
	afx_msg void OnStyleSaveAs();
};

#ifndef _DEBUG  // UIDesignerView.cpp 中的调试版本
inline CUIDesignerDoc* CUIDesignerView::GetDocument() const
   { return reinterpret_cast<CUIDesignerDoc*>(m_pDocument); }
#endif

