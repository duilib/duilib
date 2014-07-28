#pragma once

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridColor32Property

#define ARGB(a,r,g,b)        ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24))
#define GetAValue(argb)      (LOBYTE((argb)>>24))

class CMFCPropertyGridColor32Property : public CMFCPropertyGridColorProperty
{
public:
	CMFCPropertyGridColor32Property(const CString& strName,const COLORREF& color,CPalette* pPalette=NULL,LPCTSTR lpszDescr=NULL,DWORD_PTR dwData=0);

public:
	virtual BOOL OnUpdateValue();
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual CString FormatProperty();
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridImageProperty

#define AFX_PROP_HAS_BUTTON 0x0002

class CMFCPropertyGridImageProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridImageProperty)

public:
	CMFCPropertyGridImageProperty(const CString& strName, const CString& strImage, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

public:
	virtual void OnClickButton(CPoint point);
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridCustomFontsProperty

class CMFCPropertyGridCustomFontsProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridCustomFontsProperty)

public:
	CMFCPropertyGridCustomFontsProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

public:
	virtual void OnClickButton(CPoint point);
};

//////////////////////////////////////////////////////////////////////////
//CMFCPropertyGridDefaultAttribListProperty

class CMFCPropertyGridDefaultAttribListProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridDefaultAttribListProperty)

public:
	CMFCPropertyGridDefaultAttribListProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);

public:
	virtual void OnClickButton(CPoint point);
};

//////////////////////////////////////////////////////////////////////////
//CPropertiesToolBar

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

//////////////////////////////////////////////////////////////////////////
//CUIProperties

enum PropertyTag
{
	tagNULL=0,

	//Window
	tagWindow,
	tagWindowSize=tagWindow,
	tagCaption,
	tagSizeBox,
	tagRoundCorner,
	tagMinInfo,
	tagMaxInfo,
	tagShowDirty,
	tagCustomFonts,
	tagDefaultAttribList,
	tagAlpha,
	tagBkTrans,
	tagDefaultFontColor,
	tagSelectedFontColor,
	tagDisabledFontColor,
	tagLinkFontColor,
	tagLinkHoverFontColor,

	//Control
	tagControl,
	tagName=tagControl,
	tagText,
	tagPos,
	tagRelativePos,
	tagSize,
	tagMinSize,
	tagMaxSize,
	tagPadding,
	tagBkImage,
	tagBkColor,
	tagBkColor2,
	tagBorderColor,
	tagFocusBorderColor,
	tagBorderSize,
	tagBorderRound,
	tagEnabled,
	tagFloat,
	tagVisible,
	tagMouse,
	tagMenu,
	tagColorHSL,
	tagTooltip,
	tagUserData,
	tagKeyBoard,

	//Label
	tagLabel,
	tagAlign=tagLabel,
	tagTextColor,
	tagDisabledTextColor,
	tagFont,
	tagTextPadding,
	tagShowHtml,
	tagEndEllipsis,

	//Button
	tagButton,
	tagNormalImage=tagButton,
	tagHotImage,
	tagPushedImage,
	tagFocusedImage,
	tagDisabledImage,

	//Edit
	tagEdit,
	tagEditNormalImage=tagEdit,
	tagEditHotImage,
	tagEditFocusedImage,
	tagEditDisabledImage,
	tagReadOnly,
	tagPassword,
	tagMaxChar,
	tagNativeBKColor,

	//Option
	tagOption,
	tagOptForeImage=tagOption,
	tagSelectedImage,
	tagSelected,
	tagGroup,

	//Progress
	tagProgress,
	tagForeImage=tagProgress,
	tagMinMax,
	tagValue,
	tagHor,
	tagIsStretchFore,

	//Slider
	tagSlider,
	tagThumbImage=tagSlider,
	tagThumbHotImage,
	tagThumbPushedImage,
	tagThumbSize,

	//Item
	tagItem,
	tagItemBkImage=tagItem,
	tagItemHotImage,
	tagItemSelectedImage,
	tagItemDisabledImage,
	tagItemTextPadding,
	tagItemAlign,
	tagItemTextColor,
	tagItemBkColor,
	tagItemSelectedTextColor,
	tagItemSelectedBkColor,
	tagItemHotTextColor,
	tagItemHotBkColor,
	tagItemDisabledTextColor,
	tagItemDisabledBkColor,
	tagItemLineColor,
	tagItemFont,
	tagItemShowHtml,
	tagMultiExpanding,

	//Combo
	tagCombo,
	tagComboNormalImage=tagCombo,
	tagComboHotImage,
	tagComboPushedImage,
	tagComboFocusedImage,
	tagComboDisabledImage,
	tagComboTextPadding,
	tagComboDropBoxSize,

	//ActiveX
	tagActiveX,
	tagClsid=tagActiveX,
	tagDelayCreate,
	tagModuleName,

	//Container
	tagContainer,
	tagInset=tagContainer,
	tagChildPadding,
	tagMouseChild,
	tagHScrollBar,
	tagVScrollBar,

	//HorizontalLayout
	tagHorizontalLayout,
	tagSepWidth=tagHorizontalLayout,
	tagSepImm,

	//TileLayout
	tagTileLayout,
	tagColumns=tagTileLayout,

	//List
	tagList,
	tagListHeader=tagList,
	tagListHeaderBKImage,

	//ScrollBar
	tagScrollBar,
	tagScrollBarRange=tagScrollBar,
	tagScrollBarValue,
	tagScrollBarLineSize,
	tagScrollBarShowButton1,
	tagScrollBarShowButton2,
	tagScrollBarButton1NormalImage,
	tagScrollBarButton1HotImage,
	tagScrollBarButton1PushedImage,
	tagScrollBarButton1DisabledImage,
	tagScrollBarButton2NormalImage,
	tagScrollBarButton2HotImage,
	tagScrollBarButton2PushedImage,
	tagScrollBarButton2DisabledImage,
	tagScrollBarThumbNormalImage,
	tagScrollBarThumbHotImage,
	tagScrollBarThumbPushedImage,
	tagScrollBarThumbDisabledImage,
	tagScrollBarRailNormalImage,
	tagScrollBarRailHotImage,
	tagScrollBarRailPushedImage,
	tagScrollBarRailDisabledImage,
	tagScrollBarBKNormalImage,
	tagScrollBarBKHotImage,
	tagScrollBarBKPushedImage,
	tagScrollBarBKDisabledImage,

	// TabLayout
	tagTabLayout,
	tagSelectedID=tagTabLayout,

	// ListHeaderItem
	tagListHeaderItem,
	tagDragable=tagListHeaderItem,
	tagListHeaderItemSepWidth,
	tagListHeaderItemNormalImage,
	tagListHeaderItemHotImage,
	tagListHeaderItemPushedImage,
	tagListHeaderItemFocusedImage,
	tagSepImage,

	// WebBrowser
	tagWebBrowser,
	tagWebBrowserHomePage=tagWebBrowser,
	tagWebBrowserAutoNavi,
};

extern UINT WM_UI_PROPERTY_CHANGED;

class CUIProperties : public CWnd
{
	DECLARE_DYNAMIC(CUIProperties)

public:
	CUIProperties(void);
	virtual ~CUIProperties(void);

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void SetVSDotNetLook(BOOL bSet);
	void AdjustLayout();

	void ShowProperty(CControlUI* pControl);
	CControlUI* GetCurUI() const { return m_pControl; }

	CMFCPropertyGridProperty* FindPropByData(DWORD_PTR dwData, BOOL bSearchSubProps = TRUE) const;
	static CString FormatOrigProperty(CMFCPropertyGridProperty* pProp);

protected:
	void InitPropList();
	void SetPropListFont();

	void HideAllProperties(BOOL bAdjustLayout=FALSE);

	//WindowProperty
	void ShowWindowProperty(CControlUI* pControl);

	//Control Property
	void ShowControlProperty(CControlUI* pControl);
	void ShowLabelProperty(CControlUI* pControl);
	void ShowButtonProperty(CControlUI* pControl);
	void ShowEditProperty(CControlUI* pControl);
	void ShowOptionProperty(CControlUI* pControl);
	void ShowProgressProperty(CControlUI* pControl);
	void ShowSliderProperty(CControlUI* pControl);
	void ShowComboProperty(CControlUI* pControl);
	void ShowActiveXProperty(CControlUI* pControl);
	void ShowListProperty(CControlUI* pControl);
	void ShowItemProperty(CControlUI* pControl);
	void ShowScrollBarProperty(CControlUI* pControl);
	void ShowTabLayoutPropery(CControlUI* pControl);
	void ShowListHeaderItemPropery(CControlUI* pControl);
	void ShowWebBrowserPropery(CControlUI* pControl);

	//Container Property
	void ShowContainerProperty(CControlUI* pControl);
	void ShowHorizontalLayoutProperty(CControlUI* pControl);
	void ShowTileLayoutProperty(CControlUI* pControl);

protected:
	CMFCPropertyGridCtrl m_wndPropList;
	CPropertiesToolBar m_wndToolBar;
	CFont m_fntPropList;

	CControlUI* m_pControl;//save the control pointer

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);
	afx_msg void OnPropertiesSort();
	afx_msg void OnUpdatePropertiesSort(CCmdUI* pCmdUI);
	afx_msg void OnPropertiesAlphaBetic();
	afx_msg void OnUpdatePropertiesAlphaBetic(CCmdUI* pCmdUI);
};