#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib
{
	class DUILIB_API CTextUI : public CLabelUI
	{
	public:
		CTextUI();
		~CTextUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CDuiString* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);

		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		CDuiString m_sLinks[MAX_LINK];
		int m_nHoverLink;
	};

} // namespace DuiLib

#endif //__UITEXT_H__