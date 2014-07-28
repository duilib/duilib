#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ImagePreview.h"

//////////////////////////////////////////////////////////////////////////
//CMyColorButton

class CMyColorButton : public CMFCColorButton
{
	DECLARE_DYNAMIC(CMyColorButton)

protected:
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
};

//////////////////////////////////////////////////////////////////////////
// CImageDialog dialog

class CImageDialog : public CDialog
{
	DECLARE_DYNAMIC(CImageDialog)

public:
	CImageDialog(const CString& strImageProperty, CWnd* pParent = NULL);
	virtual ~CImageDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_IMAGE };

public:
	CString GetImage() const;

private:
	CString m_strImageProperty;
	CString m_strImagePathName;
	CRect m_rcDest;
	CRect m_rcSource;
	CRect m_rcCorner;
	CPaintManagerUI* m_pManager;

	static const CString m_strNullImage;
	CStringArray m_strImageArray;

protected:
	RECT StringToRect(CString& str);
	void SetImageProperty(LPCTSTR pstrPathName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	CListBox m_lstImages;
	CString m_strDest;
	CString m_strSource;
	CString m_strCorner;
	CMyColorButton m_btnMask;
	CSliderCtrl m_ctlFade;
	int m_nFade;
	BOOL m_bHole;
	CImagePreview m_ImagePreview;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonImageImport();
	afx_msg void OnBnClickedButtonImageClear();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnKillfocusEditImageFade();
	afx_msg void OnLbnDblclkListImageResource();
	afx_msg void OnDestroy();
	afx_msg void OnEnKillfocusEditImageDest();
	afx_msg void OnEnKillfocusEditImageSource();
	afx_msg void OnEnKillfocusEditImageCorner();
	afx_msg void OnBnClickedButtonImageMask();
	afx_msg void OnBnClickedRadioImageHoleFalse();
	afx_msg void OnBnClickedRadioImageHoleTrue();
	afx_msg void OnBnClickedOk();
};
