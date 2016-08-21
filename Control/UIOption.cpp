#include "stdafx.h"
#include "UIOption.h"

namespace DuiLib
{
UI_IMPLEMENT_DYNCREATE(COptionUI);
COptionUI::COptionUI() : m_bSelected(false), m_dwSelectedTextColor(0)
{
}

COptionUI::~COptionUI()
{
    if(!m_sGroupName.empty() && m_pManager)
    {
        m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
    }
}

LPCTSTR COptionUI::GetClass() const
{
    return _T("OptionUI");
}

LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("Option")) == 0)
    {
        return static_cast<COptionUI *>(this);
    }

    return CButtonUI::GetInterface(pstrName);
}

bool COptionUI::IsClientArea()
{
    return true;
}

void COptionUI::SetManager(CWindowUI *pManager, CControlUI *pParent, bool bInit)
{
    CControlUI::SetManager(pManager, pParent, bInit);

    if(bInit && !m_sGroupName.empty())
    {
        if(m_pManager)
        {
            m_pManager->AddOptionGroup(m_sGroupName.c_str(), this);
        }
    }
}

LPCTSTR COptionUI::GetGroup() const
{
    return m_sGroupName.c_str();
}

void COptionUI::SetGroup(LPCTSTR pStrGroupName)
{
    if(pStrGroupName == NULL)
    {
        if(m_sGroupName.empty())
        {
            return;
        }

        m_sGroupName.clear();
    }
    else
    {
        if(m_sGroupName.compare(pStrGroupName) == 0)
        {
            return;
        }

        if(!m_sGroupName.empty() && m_pManager)
        {
            m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
        }

        m_sGroupName = pStrGroupName;
    }

    if(!m_sGroupName.empty())
    {
        if(m_pManager)
        {
            m_pManager->AddOptionGroup(m_sGroupName.c_str(), this);
        }
    }
    else
    {
        if(m_pManager)
        {
            m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
        }
    }

    Selected(m_bSelected);
}

bool COptionUI::IsSelected() const
{
    return m_bSelected;
}

void COptionUI::Selected(bool bSelected)
{
    if(m_bSelected == bSelected)
    {
        return;
    }

    m_bSelected = bSelected;

    if(m_bSelected)
    {
        m_uButtonState |= UISTATE_Selected;
    }
    else
    {
        m_uButtonState &= ~UISTATE_Selected;
    }

    if(m_pManager != NULL)
    {
        if(!m_sGroupName.empty())
        {
            if(m_bSelected)
            {
                CStdPtrArray *aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName.c_str());

                for(int i = 0; i < aOptionGroup->GetSize(); i++)
                {
                    COptionUI *pControl = static_cast<COptionUI *>(aOptionGroup->GetAt(i));

                    if(pControl != this)
                    {
                        pControl->Selected(false);
                    }
                }

                m_pManager->SendNotify(this, UINOTIFY_SELCHANGE);
            }
        }
        else
        {
            m_pManager->SendNotify(this, UINOTIFY_SELCHANGE);
        }
    }

    Invalidate();
}

bool COptionUI::Activate()
{
    if(!CButtonUI::Activate())
    {
        return false;
    }

    if(!m_sGroupName.empty())
    {
        Selected(true);
    }
    else
    {
        Selected(!m_bSelected);
    }

    return true;
}

void COptionUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if(!IsEnabled())
    {
        if(m_bSelected)
        {
            m_uButtonState = UISTATE_Selected;
        }
        else
        {
            m_uButtonState = 0;
        }
    }
}

LPCTSTR COptionUI::GetSelectedImage()
{
    return m_sSelectedImage.c_str();
}

void COptionUI::SetSelectedImage(LPCTSTR pStrImage)
{
    m_sSelectedImage = pStrImage;
    Invalidate();
}

//************************************
// Method:    GetSelectedHotImage
// FullName:  COptionUI::GetSelectedHotImage
// Access:    public
// Returns:   LPCTSTR
// Qualifier:
// Node:
//************************************
LPCTSTR COptionUI::GetSelectedHotImage()
{
    return m_sSelectedHotImage.c_str();
}
//************************************
// Method:    SetSelectedHotImage
// FullName:  COptionUI::SetSelectedHotImage
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR pStrImage
// Node:
//************************************
void COptionUI::SetSelectedHotImage(LPCTSTR pStrImage)
{
    m_sSelectedHotImage = pStrImage;
    Invalidate();
}

void COptionUI::SetSelectedTextColor(DWORD dwTextColor)
{
    m_dwSelectedTextColor = dwTextColor;
}

DWORD COptionUI::GetSelectedTextColor()
{
    if(m_dwSelectedTextColor == 0)
    {
        m_dwSelectedTextColor = GetResEngine()->GetDefaultColor(_T("FontColor"));
    }

    return m_dwSelectedTextColor;
}

//************************************
// Method:    SetSelectedBkColor
// FullName:  COptionUI::SetSelectedBkColor
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: DWORD dwBkColor
// Note:
//************************************
void COptionUI::SetSelectedBkColor(DWORD dwBkColor)
{
    m_dwSelectedBkColor = dwBkColor;
}

//************************************
// Method:    GetSelectBkColor
// FullName:  COptionUI::GetSelectBkColor
// Access:    public
// Returns:   DWORD
// Qualifier:
// Note:
//************************************
DWORD COptionUI::GetSelectBkColor()
{
    return m_dwSelectedBkColor;
}

LPCTSTR COptionUI::GetForeImage()
{
    return m_sForeImage.c_str();
}

void COptionUI::SetForeImage(LPCTSTR pStrImage)
{
    m_sForeImage = pStrImage;
    Invalidate();
}

SIZE COptionUI::EstimateSize(SIZE szAvailable)
{
    if(m_cxyFixed.cy == 0)
    {
        return CDuiSize(m_cxyFixed.cx, GetResEngine()->GetFont(GetFontID())->GetTextMetric().tmHeight + 8);
    }

    return CControlUI::EstimateSize(szAvailable);
}

void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if(_tcscmp(pstrName, _T("group")) == 0)
    {
        SetGroup(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("selected")) == 0)
    {
        Selected(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if(_tcscmp(pstrName, _T("selectedimage")) == 0)
    {
        SetSelectedImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("selectedhotimage")) == 0)
    {
        SetSelectedHotImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("foreimage")) == 0)
    {
        SetForeImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("selectedbkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("selectedtextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedTextColor(clrColor);
    }
    else
    {
        CButtonUI::SetAttribute(pstrName, pstrValue);
    }
}

void COptionUI::PaintStatusImage(HDC hDC)
{
    if((m_uButtonState & UISTATE_Hover) != 0 && IsSelected() && !m_sSelectedHotImage.empty())
    {
        if(!DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage.c_str()))
        {
            m_sSelectedHotImage.clear();
        }
        else
        {
            goto Label_ForeImage;
        }
    }
    else if((m_uButtonState & UISTATE_Selected) != 0)
    {
        if(!m_sSelectedImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sSelectedImage.c_str()))
            {
                m_sSelectedImage.clear();
            }
            else
            {
                goto Label_ForeImage;
            }
        }
        else if(m_dwSelectedBkColor != 0)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
            return;
        }
    }

	UINT uSavedState = m_uButtonState;
	m_uButtonState &= ~UISTATE_Pushed;
    CButtonUI::PaintStatusImage(hDC);
	m_uButtonState = uSavedState;

Label_ForeImage:

    if(!m_sForeImage.empty())
    {
        if(!DrawImage(hDC, (LPCTSTR)m_sForeImage.c_str()))
        {
            m_sForeImage.clear();
        }
    }
}

void COptionUI::PaintText(HDC hDC)
{
    if((m_uButtonState & UISTATE_Selected) != 0)
    {
        DWORD oldTextColor = m_dwTextColor;

        if(m_dwSelectedTextColor != 0)
        {
            m_dwTextColor = m_dwSelectedTextColor;
        }

        if(m_dwTextColor == 0)
        {
            m_dwTextColor = GetResEngine()->GetDefaultColor(_T("FontColor"));
        }

        if(m_dwDisabledTextColor == 0)
        {
            m_dwDisabledTextColor = GetResEngine()->GetDefaultColor(_T("DisableColor"));
        }

        if(m_sText.empty())
        {
            return;
        }

        int nLinks = 0;
        RECT rc = m_rcItem;
        rc.left += m_rcTextPadding.left;
        rc.right -= m_rcTextPadding.right;
        rc.top += m_rcTextPadding.top;
        rc.bottom -= m_rcTextPadding.bottom;

        if(m_bShowHtml)
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor, \
                                        NULL, NULL, nLinks, m_uTextStyle);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText.c_str(), IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor, \
                                    m_strFontID.c_str(), m_uTextStyle);

        m_dwTextColor = oldTextColor;
		return;
    }
    else
    {
		UINT uSavedState = m_uButtonState;
		m_uButtonState &= ~UISTATE_Pushed;
		CButtonUI::PaintText(hDC);
		m_uButtonState = uSavedState;
    }
}
}