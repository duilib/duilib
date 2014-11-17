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
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);
	CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual void Render(IUIRender* pRender,LPCRECT pRcPaint);
	virtual void SetPosition(LPCRECT rc);

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
