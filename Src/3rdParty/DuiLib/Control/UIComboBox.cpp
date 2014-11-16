#include "stdafx.h"
#include "UIComboBox.h"

namespace DuiLib
{
	CComboBoxUI::CComboBoxUI()
	{
		m_nArrowWidth = 0;
	}

	LPCTSTR CComboBoxUI::GetClass() const
	{
		return _T("ComboBoxUI");
	}

	void CComboBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("arrowimage")) == 0)
			m_sArrowImage = pstrValue;
		else
			CComboUI::SetAttribute(pstrName, pstrValue);
	}

	void CComboBoxUI::PaintStatusImage(HDC hDC)
	{
		if (m_sArrowImage.IsEmpty())
			CComboUI::PaintStatusImage(hDC);
		else
		{
			// get index
			if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
			else m_uButtonState &= ~ UISTATE_FOCUSED;
			if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
			else m_uButtonState &= ~ UISTATE_DISABLED;

			int nIndex = 0;
			if ((m_uButtonState & UISTATE_DISABLED) != 0)
				nIndex = 4;
			else if ((m_uButtonState & UISTATE_PUSHED) != 0)
				nIndex = 2;
			else if ((m_uButtonState & UISTATE_HOT) != 0)
				nIndex = 1;
			else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
				nIndex = 3;

			// make modify string
			CDuiString sModify = m_sArrowImage;

			int nPos1 = sModify.Find(_T("source"));
			int nPos2 = sModify.Find(_T("'"), nPos1 + 7);
			if (nPos2 == -1) return; //first
			int nPos3 = sModify.Find(_T("'"), nPos2 + 1);
			if (nPos3 == -1) return; //second

			CDuiRect rcBmpPart;
			LPTSTR lpszValue = NULL;
			rcBmpPart.left = _tcstol(sModify.GetData() + nPos2 + 1, &lpszValue, 10);  ASSERT(lpszValue);    
			rcBmpPart.top = _tcstol(lpszValue + 1, &lpszValue, 10);    ASSERT(lpszValue);    
			rcBmpPart.right = _tcstol(lpszValue + 1, &lpszValue, 10);  ASSERT(lpszValue);    
			rcBmpPart.bottom = _tcstol(lpszValue + 1, &lpszValue, 10); ASSERT(lpszValue); 

			m_nArrowWidth = rcBmpPart.GetWidth() / 5;
			rcBmpPart.left += nIndex * m_nArrowWidth;
			rcBmpPart.right = rcBmpPart.left + m_nArrowWidth;

			CDuiRect rcDest(0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
			rcDest.Deflate(GetBorderSize(), GetBorderSize());
			rcDest.left = rcDest.right - m_nArrowWidth;

			CDuiString sSource = sModify.Mid(nPos1, nPos3 + 1 - nPos1);
			CDuiString sReplace;
			sReplace.SmallFormat(_T("source='%d,%d,%d,%d' dest='%d,%d,%d,%d'"),
				rcBmpPart.left, rcBmpPart.top, rcBmpPart.right, rcBmpPart.bottom,
				rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);

			sModify.Replace(sSource, sReplace);

			// draw image
			if (!DrawImage(hDC, m_sArrowImage, sModify))
				m_sNormalImage.Empty();
		}
	}

	void CComboBoxUI::PaintText(HDC hDC)
	{
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;

		rcText.right -= m_nArrowWidth; // add this line than CComboUI::PaintText(HDC hDC)

		if( m_iCurSel >= 0 ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
			IListItemUI* pElement = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pElement != NULL ) {
				pElement->DrawItemText(hDC, rcText);
			}
			else {
				RECT rcOldPos = pControl->GetPos();
				pControl->SetPos(rcText);
				pControl->DoPaint(hDC, rcText);
				pControl->SetPos(rcOldPos);
			}
		}
	}
}
