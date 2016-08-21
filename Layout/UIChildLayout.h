#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CChildLayoutUI
		: public CContainerUI
	{
	public:
		CChildLayoutUI();
		UI_DECLARE_DYNCREATE();

		void Init();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetChildLayoutXML(LPCTSTR pXML);
		LPCTSTR GetChildLayoutXML();
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;

		void SetViewName(LPCTSTR lpszViewName);
		LPCTSTR GetViewName();

	protected:
		CDuiString m_pstrXMLFile;
		CDuiString m_strViewName;
	};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
