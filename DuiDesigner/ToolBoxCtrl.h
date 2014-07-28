#pragma once

//////////////////////////////////////////////////////////////////////////
// CToolElement

class CToolElement : public CObject
{
	DECLARE_DYNAMIC(CToolElement)

	friend class CToolBoxCtrl;

public:
	CToolElement(const CString& strTabName);
	CToolElement(const CString& strName,int nClass,UINT nIDIcon);
	virtual ~CToolElement();

public:
	BOOL AddSubTool(CToolElement* pTool);
	BOOL IsToolTab() const { return m_bTab; }
	void Expand(BOOL bExpand = TRUE);
	BOOL IsExpanded() const { return m_bExpanded; }
	BOOL IsEnabled() const { return m_bEnabled; }
	BOOL IsVisible() const { return m_bIsVisible; }
	void SetClass(int nClass) { m_nClass=nClass; }
	CToolElement* GetTool(int nClass) const;
	int GetClass() const { return m_nClass; }
	virtual BOOL IsSelected() const;
	virtual BOOL IsHovered() const;
	BOOL IsParentExpanded() const;
	int GetExpandedSubTools(BOOL bIncludeHidden = TRUE) const;
	BOOL RemoveSubTool(CToolElement*& pTool, BOOL bDelete = TRUE);
	CToolElement* HitTest(CPoint point);

	void Redraw();
	virtual void OnDrawExpandBox(CDC* pDC, CRect rectExpand);
	virtual void OnDrawIcon(CDC* pDC, CRect rect);
	virtual void OnDrawName(CDC* pDC, CRect rect);

protected:
	void Init();
	BOOL IsSubTool(CToolElement* pTool) const;
	void SetOwnerList(CToolBoxCtrl* pWndList);
	void Reposition(int& y);

protected:
	CString m_strName;//Tool Name
	int m_nClass;//Tool Class
	BOOL    m_bTab;//Is ToolTab?
	HICON   m_hIcon;//Tool Icon
	CRect m_Rect;//Tool rectangle (in the ToolBox.list coordinates)
	BOOL m_bEnabled;         // Is Tool enabled?
	BOOL m_bIsVisible;       // Is property visible
	BOOL m_bExpanded;        // Is ToolTab expanded (for ToolTab only)
	BOOL m_bNameIsTruncated;

	CToolBoxCtrl*     m_pWndList;    // Pointer to the ToolBoxList window
	CToolElement*     m_pParent;     // Parent Tool (NULL for ToolTab)
	CList<CToolElement*, CToolElement*> m_lstSubTools; // Sub-tools list
};

////////////////////////////////////////////////////////////
// CToolBoxCtrl

class CToolBoxCtrl : public CWnd
{
	DECLARE_DYNAMIC(CToolBoxCtrl)

	friend class CToolElement;

public:
	CToolBoxCtrl();
	virtual ~CToolBoxCtrl();

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

public:
	int AddToolTab(CToolElement* pToolTab, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	BOOL DeleteToolTab(CToolElement*& pTool, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	void RemoveAll();
	int GetToolTabCount() const { return (int) m_lstToolTabs.GetCount(); }

	void SetCurSel(CToolElement* pTool, BOOL bRedraw = TRUE);
	void SetCurSel(int nClass,BOOL bRedraw=TRUE);
	CToolElement* GetCurSel() const { return m_pSel; }
	void ExpandAll(BOOL bExpand = TRUE);

	virtual void AdjustLayout();
	CToolElement* HitTest(CPoint pt) const;

public:
	virtual int OnDrawTool(CDC* pDC, CToolElement* pTool) const;
	void SetCustomColors(COLORREF clrBackground, COLORREF clrSelected, COLORREF clrHover, COLORREF m_clrSelectedBorder,
		COLORREF clrToolTab);
	void GetCustomColors(COLORREF& clrBackground, COLORREF& clrSelected, COLORREF& clrHover, COLORREF& m_clrSelectedBorder,
		COLORREF& clrToolTab);

protected:
	virtual void OnFillBackground(CDC* pDC, CRect rectClient);
	virtual void OnDrawList(CDC* pDC);
	HFONT SetCurrFont(CDC* pDC);
	void CreateToolTabFont();

	void ReposTools();
	void SetScrollSizes();
	int GetTotalItems(BOOL bIncludeHidden = TRUE) const;

protected:
	CToolTipCtrl        m_ToolTip;       // Tooltip control
	CScrollBar          m_wndScrollVert; // Vertical scroll bar

protected:
	CList<CToolElement*, CToolElement*> m_lstToolTabs;         // List of ToolTab
	CToolElement*                       m_pSel;                // Current selection
	CToolElement*                       m_pHover;              //Hover over

	CRect m_rectList; // ToolBox area
	int m_nTabHeight; //ToolTab height of the single row
	int m_nToolHeight;          // Tool height of the single row
	int m_nVertScrollOffset;   // In rows
	int m_nVertScrollTotal;
	int m_nVertScrollPage;

	HFONT m_hFont;    // ToolBox list regular font
	CFont m_fontToolTab; // ToolTab font
	CBrush m_brBackground;

	COLORREF m_clrBackground;
	COLORREF m_clrSelected;
	COLORREF m_clrHover;
	COLORREF m_clrSelectedBorder;
	COLORREF m_clrToolTab;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();
	afx_msg void OnDestroy();
};


