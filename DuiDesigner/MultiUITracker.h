#pragma once
#include "resource.h"
#include "..\DuiLib\UIlib.h"
using DuiLib::CControlUI;
using DuiLib::INotifyUI;

/////////////////////////////////////////////////////////////////////////////
// CUITracker - Modified according to CRectTracker

// the struct below is used to determine the qualities of a particular handle
typedef struct tagHandleInfo
{
	size_t nOffsetX;    // offset within RECT for X coordinate
	size_t nOffsetY;    // offset within RECT for Y coordinate
	int nCenterX;       // adjust X by Width()/2 * this number
	int nCenterY;       // adjust Y by Height()/2 * this number
	int nHandleX;       // adjust X by handle size * this number
	int nHandleY;       // adjust Y by handle size * this number
	int nInvertX;       // handle converts to this when X inverted
	int nInvertY;       // handle converts to this when Y inverted
}HandleInfo;

// the struct below gives us information on the layout of a RECT struct and
//  the relationship between its members
typedef struct tagRectInfo
{
	size_t nOffsetAcross;   // offset of opposite point (ie. left->right)
	int nSignAcross;        // sign relative to that point (ie. add/subtract)
}RectInfo;

// Style Flags
enum StyleFlags
{
	solidLine = 1, dottedLine = 2, hatchedBorder = 4,
	resizeInside = 8, resizeOutside = 16, hatchInside = 32,
};

// Hit-Test codes
enum TrackerHit
{
	hitNothing = -1,
	hitTopLeft = 0, hitTopRight = 1, hitBottomRight = 2, hitBottomLeft = 3,
	hitTop = 4, hitRight = 5, hitBottom = 6, hitLeft = 7, hitMiddle = 8,
};

class CUITracker
{
public:
	// Constructors
	CUITracker();
	CUITracker(LPCRECT lpSrcRect, UINT nStyle);

	// Operations
	void GetTrueRect(LPRECT lpTrueRect) const;
	BOOL SetCursor(CPoint point, UINT nHitTest) const;
	BOOL Track(CWnd* pWnd, CPoint point, BOOL bAllowInvert = FALSE,
		CDC* pDCClipTo = NULL);
	BOOL TrackRubberBand(CWnd* pWnd, CPoint point, BOOL bAllowInvert = TRUE);
	int HitTest(CPoint point) const;
	int NormalizeHit(int nHandle) const;
	RECT GetRect() const { return m_rect; }

	void SetHandleSize(int nHandleSize) { m_nHandleSize=nHandleSize; }
	void SetStyle(UINT style) { m_nStyle=style; }

	// Overridables
	virtual void Draw(CDC* pDC) const;
	virtual void DrawTrackerRect(LPCRECT lpRect, CDC* pDC);
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
	virtual void OnChangedRect(const CRect& rectOld);
	virtual UINT GetHandleMask() const;
	virtual int GetHandleSize(LPCRECT lpRect = NULL) const;
	virtual void SetControlType(int nType);

	// Implementation
public:
	virtual ~CUITracker();

protected:
	// Attributes
	UINT m_nStyle;      // current state
	CRect m_rect;       // current position (always in pixels)
	CSize m_sizeMin;    // minimum X and Y size during track operation
	int m_nHandleSize;  // size of resize handles
	int m_nMoveHandleSize;//size of move handle
	UINT m_nMask;
	UINT m_nControlType;

	BOOL m_bAllowInvert;    // flag passed to Track or TrackRubberBand
	CRect m_rectLast;
	CSize m_sizeLast;
	BOOL m_bErase;          // TRUE if DrawTrackerRect is called for erasing
	BOOL m_bFinalErase;     // TRUE if DragTrackerRect called for final erase

	static HCURSOR  m_hCursors[10];
	static HBRUSH   m_hHatchBrush;
	COLORREF m_clrDottedLine;
	HPEN     m_hDottedLinePen;
	COLORREF m_clrHandleBorder;
	HPEN     m_hHandlePen;
	COLORREF m_clrHandleBackground;
	HBRUSH   m_hHandleBrush;
	HBITMAP  m_hMoveHandleBitmap;

	// implementation helpers
	int HitTestHandles(CPoint point) const;
	void GetHandleRect(int nHandle, CRect* pHandleRect) const;
	void GetMoveHandleRect(CRect* pHandleRect) const;
	void GetModifyPointers(int nHandle, int**ppx, int**ppy, int* px, int*py);
	BOOL TrackHandle(int nHandle, CWnd* pWnd, CDC* pDCClipTo);
	void Init();
	void Construct();
};

////////////////////////////////////////////////////////////
// CTrackerElement

class CMultiUITracker;

class CTrackerElement
{
public:
	CTrackerElement(void);
	CTrackerElement(CControlUI* pControl,int nType,INotifyUI* pOwner);
	virtual ~CTrackerElement(void);

	friend CMultiUITracker;

public:
	const RECT& GetPos() const;
	void SetPos(RECT rect,BOOL bMove=FALSE);
	CControlUI* GetControl() { return m_pControl; }

protected:
	CControlUI * m_pControl;
	int m_nType;
	INotifyUI* m_pOwner;
};

////////////////////////////////////////////////////////////
// CMultiUITracker

class CMultiUITracker : public CUITracker
{
public:
	CMultiUITracker(void);
	virtual ~CMultiUITracker(void);

public:
	void Draw(CDC* pDC,LPSIZE pOffset=NULL);

	int HitTest(CPoint point);
	int HitTestHandles(CPoint point);
	BOOL Track(CWnd* pWnd, CPoint point,BOOL bAllowInvert=FALSE,CDC* pDCClipTo=NULL);
	BOOL SetCursor(CPoint point, UINT nHitTest);

	BOOL Add(CTrackerElement * pTracker);
	BOOL Remove(CTrackerElement * pTracker);
	void RemoveAll();
	BOOL SetFocus(CPoint point);
	BOOL SetFocus(CControlUI* pControl);
	CTrackerElement* FindTracker(CControlUI* pControl) const;
	CControlUI* GetFocused() const;
	BOOL GetSelected(CArray<CControlUI*,CControlUI*>& arrSelected);
	BOOL IsEmpty() const { return m_arrTracker.IsEmpty(); }
	int  GetSize() const { return m_arrTracker.GetSize(); }

	SIZE GetFormSize() const { return m_szForm; }
	void SetFormSize(SIZE size) { m_szForm=size; }

protected:
	BOOL MultiTrackHandle(CWnd* pWnd,CDC* pDCClipTo);
	BOOL OneTrackHandle(int nHandle, CWnd* pWnd, BOOL bAllowInvert, CDC* pDCClipTo);
	void CopyUIRect();
	void ClearUIRect();
	void UpdateUIRect();
	void ExcludeChildren(CArray<CControlUI*,CControlUI*>& arrSelected);

private:
	CArray<CTrackerElement*,CTrackerElement*> m_arrTracker; //Tracking controls array
	CArray<RECT,const RECT&> m_arrCloneRect;//Copies of the tracking rectangle
	CTrackerElement* m_pFocused; // Control has focus

	SIZE m_szForm;
	HCURSOR m_hNoDropCursor;
};
