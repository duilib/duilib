#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	UI_IMPLEMENT_DYNCREATE(CCheckBoxUI);
	LPCTSTR CCheckBoxUI::GetClass() const
	{
		return _T("CheckBoxUI");
	}

	void CCheckBoxUI::SetCheck(bool bCheck)
	{
		Selected(bCheck);
	}

	bool  CCheckBoxUI::GetCheck() const
	{
		return IsSelected();
	}
}
