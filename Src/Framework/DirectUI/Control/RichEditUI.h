#ifndef RichEditUI_h__
#define RichEditUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	RichEditUI.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-12-11 16:07:19
// 说明			:	
/////////////////////////////////////////////////////////////*/
#pragma once
#include <Richedit.h>

class CTxtWinHost;
class DIRECTUI_API CRichEditUI
	: public CContainerUI
	, public IMessageFilterUI
{
public:
	CRichEditUI(void);
	virtual ~CRichEditUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	bool IsWantTab();
	void SetWantTab(bool bWantTab = true);
	bool IsWantReturn();
	void SetWantReturn(bool bWantReturn = true);
	bool IsWantCtrlReturn();
	void SetWantCtrlReturn(bool bWantCtrlReturn = true);
	bool IsRich();
	void SetRich(bool bRich = true);
	bool IsReadOnly();
	void SetReadOnly(bool bReadOnly = true);
	bool GetWordWrap();
	void SetWordWrap(bool bWordWrap = true);
	LPCTSTR GetFont();
	void SetFontName(LPCTSTR lpszFontName);
	void SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
	LONG GetWinStyle();
	void SetWinStyle(LONG lStyle);
	DWORD GetTextColor();
	void SetTextColor(DWORD dwTextColor);
	int GetLimitText();
	void SetLimitText(int iChars);
	long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
	LPCTSTR GetText() const;
	void SetText(LPCTSTR pstrText);
	bool GetModify() const;
	void SetModify(bool bModified = true) const;
	void GetSel(CHARRANGE &cr) const;
	void GetSel(long& nStartChar, long& nEndChar) const;
	int SetSel(CHARRANGE &cr);
	int SetSel(long nStartChar, long nEndChar);
	void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo);
	void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
	CDuiString GetSelText() const;
	int SetSelAll();
	int SetSelNone();
	WORD GetSelectionType() const;
	bool GetZoom(int& nNum, int& nDen) const;
	bool SetZoom(int nNum, int nDen);
	bool SetZoomOff();
	bool GetAutoURLDetect() const;
	bool SetAutoURLDetect(bool bAutoDetect = true);
	DWORD GetEventMask() const;
	DWORD SetEventMask(DWORD dwEventMask);
	CDuiString GetTextRange(long nStartChar, long nEndChar) const;
	void HideSelection(bool bHide = true, bool bChangeStyle = false);
	void ScrollCaret();
	int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
	int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
	DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
	bool SetDefaultCharFormat(CHARFORMAT2 &cf);
	DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
	bool SetSelectionCharFormat(CHARFORMAT2 &cf);
	bool SetWordCharFormat(CHARFORMAT2 &cf);
	DWORD GetParaFormat(PARAFORMAT2 &pf) const;
	bool SetParaFormat(PARAFORMAT2 &pf);
	bool Redo();
	bool Undo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();
	int GetLineCount() const;
	CDuiString GetLine(int nIndex, int nMaxLength) const;
	int LineIndex(int nLine = -1) const;
	int LineLength(int nLine = -1) const;
	bool LineScroll(int nLines, int nChars = 0);
	CDuiPoint GetCharPos(long lChar) const;
	long LineFromChar(long nIndex) const;
	CDuiPoint PosFromChar(UINT nChar) const;
	int CharFromPos(CDuiPoint pt) const;
	void EmptyUndoBuffer();
	UINT SetUndoLimit(UINT nLimit);
	long StreamIn(int nFormat, EDITSTREAM &es);
	long StreamOut(int nFormat, EDITSTREAM &es);

	void DoInit();
	// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
	// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
	bool SetDropAcceptFile(bool bAccept);
	virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
	IDropTarget* GetTxDropTarget();
	virtual bool OnTxViewChanged();
	virtual void OnTxNotify(DWORD iNotify, void *pv);

	void SetScrollPos(SIZE szPos);
	void LineUp();
	void LineDown();
	void PageUp();
	void PageDown();
	void HomeUp();
	void EndDown();
	void LineLeft();
	void LineRight();
	void PageLeft();
	void PageRight();
	void HomeLeft();
	void EndRight();

	SIZE EstimateSize(SIZE szAvailable);
	void SetPosition(LPCRECT lpRect);
	bool EventHandler(TEventUI& event);
	void Render(IUIRender* pRender,LPCRECT pRcPaint);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	LRESULT MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
	CTxtWinHost* m_pTwh;
	bool m_bVScrollBarFixing;
	bool m_bWantTab;
	bool m_bWantReturn;
	bool m_bWantCtrlReturn;
	bool m_bRich;
	bool m_bReadOnly;
	bool m_bWordWrap;
	DWORD m_dwTextColor;
	CDuiString m_strFontName;
	int m_iLimitText;
	LONG m_lTwhStyle;
	bool m_bInited;
};

#endif // RichEditUI_h__
