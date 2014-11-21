#ifndef ContainerUI_h__
#define ContainerUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ContainerUI.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-13 18:10:34
// 说明			:	
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Control/ControlUI.h"
#include "Control/ScrollBarUI.h"

class DIRECTUI_API CContainerUI
	: public CControlUI
	, public IContainerUI
{
public:
	CContainerUI(void);
	virtual ~CContainerUI(void);
	UI_DECLARE_DYNCREATE();

	//////////////////////////////////////////////////////////////////////////
	// IContainerUI
	virtual CControlUI* GetItemAt(int iIndex) const;
	virtual int GetItemIndex(CControlUI* pControl) const;
	virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
	virtual int GetCount() const;
	virtual bool Add(CControlUI* pControl);
	virtual bool AddAt(CControlUI* pControl, int iIndex);
	virtual bool Remove(CControlUI* pControl);
	virtual bool RemoveAt(int iIndex);
	virtual void RemoveAll();

	//////////////////////////////////////////////////////////////////////////
	// CObjectUI
	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);

	//////////////////////////////////////////////////////////////////////////
	// CControlUI
	virtual void SetNotifyFilter(INotifyUI* pNotifyFilter);
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);
	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual CControlUI* FindSubControl(LPCTSTR pstrSubControlName);
	virtual void Render(IUIRender* pRender,LPCRECT pRcPaint);
	virtual void SetPosition(LPCRECT rc);
	virtual bool EventHandler(TEventUI& event);

public:
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

protected:
	virtual void SetFloatPos(int iIndex);

private:
	CStdPtrArray m_items;
	CDuiRect m_rcInset;
	int m_iChildPadding;
	bool m_bAutoDestroy;
	bool m_bDelayedDestroy;
	bool m_bMouseChildEnabled;
	bool m_bScrollProcess; // 防止SetPos循环调用

	CScrollBarUI* m_pVerticalScrollBar;
	CScrollBarUI* m_pHorizontalScrollBar;
};

#endif // ContainerUI_h__
