#pragma once
#include "..\DuiLib\UIlib.h"
using DuiLib::CContainerUI;
using DuiLib::CPaintManagerUI;
using DuiLib::CWindowWnd;
using DuiLib::TNotifyUI;
using DuiLib::IDialogBuilderCallback;

#define EXPORT_UI_SCRIPT

#if defined(EXPORT_UI_SCRIPT)
#include "tinyxml.h"
#endif

//////////////////////////////////////////////////////////////////////////
//CFormUI

class CWindowUI : public CContainerUI
{
public:
	CWindowUI();
	virtual ~CWindowUI();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	SIZE GetInitSize();
	void SetInitSize(int cx, int cy);
	RECT& GetSizeBox();
	void SetSizeBox(RECT& rcSizeBox);
	RECT& GetCaptionRect();
	void SetCaptionRect(RECT& rcCaption);
	SIZE GetRoundCorner() const;
	void SetRoundCorner(int cx, int cy);
	SIZE GetMinInfo() const;
	SIZE GetMaxInfo() const;
	void SetMinInfo(int cx, int cy);
	void SetMaxInfo(int cx, int cy);
	bool IsShowUpdateRect() const;
	void SetShowUpdateRect(bool show);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetPos(RECT rc);

public:
	void			SetAlpha(int nOpacity);
	int			GetAlpha() const;

	void			SetBackgroundTransparent(bool bTrans);
	bool			GetBackgroundTransparent() const;

	void			SetDefaultFontColor(DWORD dwColor);
	DWORD	GetDefaultFontColor() const;

	void			SetDefaultSelectedFontColor(DWORD dwColor);
	DWORD	GetDefaultSelectedFontColor() const;

	void			SetDefaultDisabledFontColor(DWORD dwColor);
	DWORD	GetDefaultDisabledFontColor() const;

	void			SetDefaultLinkFontColor(DWORD dwColor);
	DWORD	GetDefaultLinkFontColor() const;

	void			SetDefaultLinkHoverFontColor(DWORD dwColor);
	DWORD	GetDefaultLinkHoverFontColor() const;

private:
	int			m_nOpacity;
	bool			m_bAlphaBackground;
	DWORD	m_dwDefaultFontColor;
	DWORD	m_dwDefaultDisabledFontColor;
	DWORD	m_dwDefaultLinkFontColor;
	DWORD	m_dwDefaultLinkHoverFontColor;
	DWORD	m_dwDefaultSelectedFontColor;
};

//////////////////////////////////////////////////////////////////////////
//CFormTestWnd

class CFormTestWnd :  public CWindowWnd, public INotifyUI
{
public:
	CFormTestWnd();
	~CFormTestWnd();
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

public:
	void SetManager(CPaintManagerUI* pPaintManager);
	CPaintManagerUI* GetManager() const;
	void SetRoot(CControlUI* pControl);
	void Init();

	void Notify(TNotifyUI& msg);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CPaintManagerUI* m_pManager;
	CControlUI* m_pRoot;
};

//////////////////////////////////////////////////////////////////////////
//CLayoutManager

class CLayoutManager : public IDialogBuilderCallback
{
public:
	CLayoutManager(void);
	virtual ~CLayoutManager(void);

public:
	void Init(HWND hWnd,LPCTSTR pstrLoad);

	void Draw(CDC* pDC);
	void DrawAuxBorder(CDC* pDC,CControlUI* pControl);
	void DrawGrid(CDC* pDC,CRect& rect);

	static CControlUI* NewUI(int nClass,CRect& rect,CControlUI* pParent, CLayoutManager* pLayout);
	static BOOL DeleteUI(CControlUI* pControl);
	static void ReleaseExtendedAttrib(CControlUI* pControl, CPaintManagerUI* pManager);
	CPaintManagerUI* GetManager();
	inline CWindowUI* GetForm() const;
	CControlUI* FindControl(CPoint point) const;

	void TestForm(LPCTSTR pstrFile);
	BOOL IsEmptyForm() const;
	void AlignLeft(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignRight(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignTop(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignBottom(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignCenterVertically(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignCenterHorizontally(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignHorizontal(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignVertical(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignSameWidth(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignSameHeight(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void AlignSameSize(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected);
	void ShowGrid(bool bShow);
	bool IsShowGrid() const;
	void ShowAuxBorder(bool bShow);
	bool IsShowAuxBorder() const;
	void MicoMoveUp(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved);
	void MicoMoveDown(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved);
	void MicoMoveLeft(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved);
	void MicoMoveRight(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved);

	bool SaveSkinFile(LPCTSTR pstrPathName);
	static void SaveProperties(CControlUI* pControl, TiXmlElement* pParentNode
		, BOOL bSaveChildren = TRUE);

	void SetDefaultUIName(CControlUI* pControl);

	LPCTSTR GetSkinDir() const { return m_strSkinDir; }

	static CControlUI* CloneControls(CControlUI* pControl);
	static CControlUI* CloneControl(CControlUI* pControl);

	CControlUI* CreateControl(LPCTSTR pstrClass);
protected:
	static void SaveControlProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveLabelProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveButtonProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveOptionProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveProgressProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveSliderProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveEditProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveScrollBarProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveListProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveComboProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveListHeaderProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveListHeaderItemProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveListElementProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveContainerProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveHorizontalLayoutProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveTileLayoutProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveActiveXProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveListContainerElementProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveItemProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveTabLayoutProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveChildWindowProperty(CControlUI* pControl, TiXmlElement* pNode);
	static void SaveWebBrowserProperty(CControlUI* pControl, TiXmlElement* pNode);

	static CString ConvertImageFileName(LPCTSTR pstrImageAttrib);

protected:
	class CDelayRepos
	{
	public:
		CDelayRepos();
		~CDelayRepos();

	public:
		BOOL AddParent(CControlUI* pControl);
		void Repos();

	private:
		CArray<CControlUI*,CControlUI*> m_arrDelay;
	};

private:
	CPaintManagerUI m_Manager;
	CWindowUI* m_pFormUI;
	static CString m_strSkinDir;

	bool m_bShowGrid;
	bool m_bShowAuxBorder;
};