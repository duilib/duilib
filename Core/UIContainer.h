#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
class CScrollBarUI;

class UILIB_API CContainerUI : public CControlUI, public IContainerUI
{
public:
    CContainerUI();
    virtual ~CContainerUI();
	UI_DECLARE_DYNCREATE();

public:
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    virtual CControlUI* GetItemAt(int iIndex) const;
    int GetItemIndex(CControlUI* pControl) const;
    bool SetItemIndex(CControlUI* pControl, int iIndex);
    int GetCount() const;
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    void EventHandler(TEventUI& event);
    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    void SetMouseEnabled(bool bEnable = true);

	// 控件显示状态，内部处理后的最终状态
	virtual void VisibleState(bool bVisible);

    virtual RECT GetInset() const;
    virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
    virtual int GetChildPadding() const;
    virtual void SetChildPadding(int iPadding);
    virtual bool IsAutoDestroy() const;
    virtual void SetAutoDestroy(bool bAuto);
    virtual bool IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(bool bDelayed);
    virtual bool IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(bool bEnable = true);

    virtual int FindSelectable(int iIndex, bool bForward = true) const;

	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	virtual void SetNotifyFilter(INotifyUI *pNotifyFilter);
    void SetManager(CWindowUI* pManager, CControlUI* pParent, bool bInit = true);
    CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	bool SetSubControlText(LPCTSTR pstrSubControlName,LPCTSTR pstrText);
	bool SetSubControlFixedHeight(LPCTSTR pstrSubControlName,int cy);
	bool SetSubControlFixedWdith(LPCTSTR pstrSubControlName,int cx);
	bool SetSubControlUserData(LPCTSTR pstrSubControlName,LPCTSTR pstrText);

	CDuiString GetSubControlText(LPCTSTR pstrSubControlName);
	int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
	int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
	const CDuiString GetSubControlUserData(LPCTSTR pstrSubControlName);
	CControlUI* FindSubControl(LPCTSTR pstrSubControlName);

    virtual SIZE GetScrollPos() const;
    virtual SIZE GetScrollRange() const;
    virtual void SetScrollPos(SIZE szPos);
    virtual void LineUp();
    virtual void LineDown();
    virtual void PageUp();
    virtual void PageDown();
    virtual void HomeUp();
    virtual void EndDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void PageLeft();
    virtual void PageRight();
    virtual void HomeLeft();
    virtual void EndRight();
    virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual CScrollBarUI* GetVerticalScrollBar() const;
    virtual CScrollBarUI* GetHorizontalScrollBar() const;

protected:
    virtual void SetFloatPos(int iIndex);
    virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

protected:
    CStdPtrArray m_items;
    RECT m_rcInset;
    int m_iChildPadding;
    bool m_bAutoDestroy;
    bool m_bDelayedDestroy;
    bool m_bMouseChildEnabled;
    bool m_bScrollProcess; // 防止SetPos循环调用

    CScrollBarUI* m_pVerticalScrollBar;
    CScrollBarUI* m_pHorizontalScrollBar;
};

} // namespace DuiLib

#endif // __UICONTAINER_H__
