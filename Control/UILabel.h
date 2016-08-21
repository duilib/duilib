#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
	public:
		CLabelUI();
		UI_DECLARE_DYNCREATE();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		void SetFontID(LPCTSTR lpszFontID);
		LPCTSTR GetFontID();
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void EventHandler(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);

	protected:
		DWORD m_dwTextColor;
		DWORD m_dwDisabledTextColor;
		CDuiString m_strFontID;
		int m_iFont;
		UINT m_uTextStyle;
		RECT m_rcTextPadding;
		bool m_bShowHtml;
	};
}

#endif // __UILABEL_H__