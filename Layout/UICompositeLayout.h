#ifndef CompositeUI_h__
#define CompositeUI_h__
#pragma once

namespace DuiLib
{

class UILIB_API CCompositeLayoutUI
	: public CChildLayoutUI
	, public INotifyUI
{
public:
	CCompositeLayoutUI(void);
	virtual ~CCompositeLayoutUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual LPCTSTR GetClass() const;

	virtual void AddNotifyFilter(INotifyUI *pFilter);
	virtual void RemoveNotifyFilter(INotifyUI *pFilter);

	virtual bool Notify(TNotifyUI *pMsg);

	virtual void DoInit();

private:
	CStdPtrArray m_arrayNotifyFilters;
};

}

#endif // CompositeUI_h__
