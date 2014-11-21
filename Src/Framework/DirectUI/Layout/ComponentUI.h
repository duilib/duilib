#ifndef ComponentUI_h__
#define ComponentUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ComponentUI.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-21 17:08:06
// 说明			:	组合布局，主要用于事件过滤
/////////////////////////////////////////////////////////////*/
#pragma once
class DIRECTUI_API CComponentUI
	: public CContainerUI
	, public INotifyUI
{
public:
	CComponentUI(void);
	virtual ~CComponentUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);

	// INotifyUI
	virtual bool Notify(TNotifyUI *pMsg);

	void AddNotify(INotifyUI *pNotify);
	void RemoveNotify(INotifyUI *pNotify);

private:
	CStdPtrArray m_arrayNotifyFilters;
};

#endif // ComponentUI_h__
