#ifndef ChildLayoutUI_h__
#define ChildLayoutUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ChildLayoutUI.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-26 16:15:15
// 说明			:	子页面布局
/////////////////////////////////////////////////////////////*/
#pragma once
class DIRECTUI_API CChildLayoutUI
	: public CContainerUI
	, public INotifyUI
{
public:
	CChildLayoutUI(void);
	virtual ~CChildLayoutUI(void);
	UI_DECLARE_DYNCREATE();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	void SetChildLayoutXML(LPCTSTR lpFilePath);
	LPCTSTR GetChildLayoutXML();

	virtual bool Notify(TNotifyUI *pMsg);

	void AddNotify(INotifyUI *pNotify);
	void RemoveNotify(INotifyUI *pNotify);

	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);

private:
	CDuiString m_strXmlPath;
	CStdPtrArray m_arrayNotifyFilters;
	CControlUI *m_pRootControl;
};

#endif // ChildLayoutUI_h__
