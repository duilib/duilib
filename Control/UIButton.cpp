#include "stdafx.h"
#include "UIButton.h"

namespace DuiLib
{
UI_IMPLEMENT_DYNCREATE(CButtonUI);
CButtonUI::CButtonUI()
    : m_uButtonState(0)
    , m_dwHotTextColor(0)
    , m_dwPushedTextColor(0)
    , m_dwFocusedTextColor(0)
    , m_dwHotBkColor(0)
{
    m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
}

LPCTSTR CButtonUI::GetClass() const
{
    return _T("ButtonUI");
}

LPVOID CButtonUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("Button")) == 0)
    {
        return static_cast<CButtonUI *>(this);
    }

    return CLabelUI::GetInterface(pstrName);
}

bool CButtonUI::IsClientArea()
{
    return true;
}

UINT CButtonUI::GetControlFlags() const
{
    return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
}

void CButtonUI::EventHandler(TEventUI & event)
{
    if(!IsMouseEnabled() && event.dwType > UIEVENT__MOUSEBEGIN && event.dwType < UIEVENT__MOUSEEND)
    {
        if(m_pParent != NULL)
        {
            m_pParent->EventHandler(event);
        }
        else
        {
            CLabelUI::EventHandler(event);
        }

        return;
    }

    if(event.dwType == UIEVENT_SETFOCUS)
    {
        Invalidate();
    }

    if(event.dwType == UIEVENT_KILLFOCUS)
    {
        Invalidate();
    }

    if(event.dwType == UIEVENT_KEYDOWN)
    {
        if(IsKeyboardEnabled())
        {
            if(event.chKey == VK_SPACE || event.chKey == VK_RETURN)
            {
                Activate();
                return;
            }
        }
    }

    if(event.dwType == UIEVENT_LBUTTONDOWN)
    {
        if(::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
        {
            m_uButtonState |= UISTATE_Pushed | UISTATE_Captured;
            Invalidate();
        }

        return;
    }

    if(event.dwType == UIEVENT_LDBLCLICK)
    {
        if(::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
        {
            m_uButtonState |= UISTATE_Pushed | UISTATE_Captured | UISTATE_Double;
            Invalidate();
        }

        return;
    }

    if(event.dwType == UIEVENT_LBUTTONUP)
    {
        if((m_uButtonState & UISTATE_Captured) != 0)
        {
            if(::PtInRect(&m_rcItem, event.ptMouse))
            {
                Activate();
            }

            m_uButtonState &= ~(UISTATE_Pushed | UISTATE_Captured | UISTATE_Double);
            Invalidate();
        }

        return;
    }

    if(event.dwType == UIEVENT_MOUSEMOVE)
    {
        if((m_uButtonState & UISTATE_Captured) != 0)
        {
			UINT State = m_uButtonState;
            if(::PtInRect(&m_rcItem, event.ptMouse))
            {
                m_uButtonState |= UISTATE_Pushed;
            }
            else
            {
                m_uButtonState &= ~UISTATE_Pushed;
            }
			if (State != m_uButtonState)
			{
				DUI__Trace(_T("CButtonUIÒªÇóÖØ»æ"));
				Invalidate();
			}
        }

        return;
    }

    if(event.dwType == UIEVENT_CONTEXTMENU)
    {
        if(IsContextMenuUsed())
        {
            m_pManager->SendNotify(this, UINOTIFY_CONTEXTMENU, event.wParam, event.lParam);
        }

        return;
    }

    if(event.dwType == UIEVENT_MOUSEENTER)
    {
        if(IsEnabled())
        {
            m_uButtonState |= UISTATE_Hover;
            Invalidate();
        }

        // return;
    }

    if(event.dwType == UIEVENT_MOUSELEAVE)
    {
        if(IsEnabled())
        {
            m_uButtonState &= ~UISTATE_Hover;
            Invalidate();
        }

        // return;
    }

    if(event.dwType == UIEVENT_SETCURSOR)
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
        return;
    }

    CLabelUI::EventHandler(event);
}

bool CButtonUI::Activate()
{
    if(!CControlUI::Activate())
    {
        return false;
    }

    if(m_pManager != NULL)
    {
        if((m_uButtonState & UISTATE_Double) != 0)
        {
            m_pManager->SendNotify(this, UINOTIFY_LDBCLICK);
        }
        else
        {
            m_pManager->SendNotify(this, UINOTIFY_LBTNCLICK);
        }
    }

    return true;
}

void CButtonUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if(!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

//************************************
// Method:    SetHotBkColor
// FullName:  CButtonUI::SetHotBkColor
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: DWORD dwColor
// Note:
//************************************
void CButtonUI::SetHotBkColor(DWORD dwColor)
{
    m_dwHotBkColor = dwColor;
}

//************************************
// Method:    GetHotBkColor
// FullName:  CButtonUI::GetHotBkColor
// Access:    public
// Returns:   DWORD
// Qualifier: const
// Note:
//************************************
DWORD CButtonUI::GetHotBkColor() const
{
    return m_dwHotBkColor;
}

void CButtonUI::SetHotTextColor(DWORD dwColor)
{
    m_dwHotTextColor = dwColor;
}

DWORD CButtonUI::GetHotTextColor() const
{
    return m_dwHotTextColor;
}

void CButtonUI::SetPushedTextColor(DWORD dwColor)
{
    m_dwPushedTextColor = dwColor;
}

DWORD CButtonUI::GetPushedTextColor() const
{
    return m_dwPushedTextColor;
}

void CButtonUI::SetFocusedTextColor(DWORD dwColor)
{
    m_dwFocusedTextColor = dwColor;
}

DWORD CButtonUI::GetFocusedTextColor() const
{
    return m_dwFocusedTextColor;
}

LPCTSTR CButtonUI::GetNormalImage()
{
    return m_sNormalImage.c_str();
}

void CButtonUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetHotImage()
{
    return m_sHotImage.c_str();
}

void CButtonUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetPushedImage()
{
    return m_sPushedImage.c_str();
}

void CButtonUI::SetPushedImage(LPCTSTR pStrImage)
{
    m_sPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetFocusedImage()
{
    return m_sFocusedImage.c_str();
}

void CButtonUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetDisabledImage()
{
    return m_sDisabledImage.c_str();
}

void CButtonUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

//************************************
// Method:    GetForeImage
// FullName:  CButtonUI::GetForeImage
// Access:    public
// Returns:   LPCTSTR
// Qualifier:
// Note:
//************************************
LPCTSTR CButtonUI::GetForeImage()
{
    return m_sForeImage.c_str();
}

//************************************
// Method:    SetForeImage
// FullName:  CButtonUI::SetForeImage
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR pStrImage
// Note:
//************************************
void CButtonUI::SetForeImage(LPCTSTR pStrImage)
{
    m_sForeImage = pStrImage;
    Invalidate();
}

//************************************
// Method:    GetHotForeImage
// FullName:  CButtonUI::GetHotForeImage
// Access:    public
// Returns:   LPCTSTR
// Qualifier:
// Note:
//************************************
LPCTSTR CButtonUI::GetHotForeImage()
{
    return m_sHotForeImage.c_str();
}

//************************************
// Method:    SetHotForeImage
// FullName:  CButtonUI::SetHotForeImage
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR pStrImage
// Note:
//************************************
void CButtonUI::SetHotForeImage(LPCTSTR pStrImage)
{
    m_sHotForeImage = pStrImage;
    Invalidate();
}

SIZE CButtonUI::EstimateSize(SIZE szAvailable)
{
    if(m_cxyFixed.cy == 0)
    {
        return CDuiSize(m_cxyFixed.cx, GetResEngine()->GetFont(GetFontID())->GetTextMetric().tmHeight + 8);
    }

    return CControlUI::EstimateSize(szAvailable);
}

void CButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if(_tcscmp(pstrName, _T("normalimage")) == 0)
    {
        SetNormalImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("hotimage")) == 0)
    {
        SetHotImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("pushedimage")) == 0)
    {
        SetPushedImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("focusedimage")) == 0)
    {
        SetFocusedImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("disabledimage")) == 0)
    {
        SetDisabledImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("foreimage")) == 0)
    {
        SetForeImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("hotforeimage")) == 0)
    {
        SetHotForeImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("hotbkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("hottextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotTextColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("pushedtextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetPushedTextColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("focusedtextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetFocusedTextColor(clrColor);
    }
    else
    {
        CLabelUI::SetAttribute(pstrName, pstrValue);
    }
}

void CButtonUI::PaintText(HDC hDC)
{
    if(IsFocused())
    {
        m_uButtonState |= UISTATE_Focused;
    }
    else
    {
        m_uButtonState &= ~ UISTATE_Focused;
    }

    if(!IsEnabled())
    {
        m_uButtonState |= UISTATE_Disabled;
    }
    else
    {
        m_uButtonState &= ~ UISTATE_Disabled;
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
    DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;

    if(((m_uButtonState & UISTATE_Pushed) != 0) && (GetPushedTextColor() != 0))
    {
        clrColor = GetPushedTextColor();
    }
    else if(((m_uButtonState & UISTATE_Hover) != 0) && (GetHotTextColor() != 0))
    {
        clrColor = GetHotTextColor();
    }
    else if(((m_uButtonState & UISTATE_Focused) != 0) && (GetFocusedTextColor() != 0))
    {
        clrColor = GetFocusedTextColor();
    }

    if(m_bShowHtml)
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText.c_str(), clrColor, \
                                    NULL, NULL, nLinks, m_uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText.c_str(), clrColor, \
                                m_strFontID.c_str(), m_uTextStyle);
}

void CButtonUI::PaintStatusImage(HDC hDC)
{
    if(IsFocused())
    {
        m_uButtonState |= UISTATE_Focused;
    }
    else
    {
        m_uButtonState &= ~ UISTATE_Focused;
    }

    if(!IsEnabled())
    {
        m_uButtonState |= UISTATE_Disabled;
    }
    else
    {
        m_uButtonState &= ~ UISTATE_Disabled;
    }

    if((m_uButtonState & UISTATE_Disabled) != 0)
    {
        if(!m_sDisabledImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sDisabledImage.c_str()))
            {
                m_sDisabledImage.clear();
            }
            else
            {
                goto Label_ForeImage;
            }
        }
    }
    else if((m_uButtonState & UISTATE_Pushed) != 0)
    {
        if(!m_sPushedImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sPushedImage.c_str()))
            {
                m_sPushedImage.clear();
            }

            if(!m_sPushedForeImage.empty())
            {
                if(!DrawImage(hDC, (LPCTSTR)m_sPushedForeImage.c_str()))
                {
                    m_sPushedForeImage.clear();
                }

                return;
            }
            else
            {
                goto Label_ForeImage;
            }
        }
    }
    else if((m_uButtonState & UISTATE_Hover) != 0)
    {
        if(!m_sHotImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sHotImage.c_str()))
            {
                m_sHotImage.clear();
            }

            if(!m_sHotForeImage.empty())
            {
                if(!DrawImage(hDC, (LPCTSTR)m_sHotForeImage.c_str()))
                {
                    m_sHotForeImage.clear();
                }

                return;
            }
            else
            {
                goto Label_ForeImage;
            }
        }
        else if(m_dwHotBkColor != 0)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
            return;
        }
    }
    else if((m_uButtonState & UISTATE_Focused) != 0)
    {
        if(!m_sFocusedImage.empty())
        {
            if(!DrawImage(hDC, (LPCTSTR)m_sFocusedImage.c_str()))
            {
                m_sFocusedImage.clear();
            }
            else
            {
                goto Label_ForeImage;
            }
        }
    }

    if(!m_sNormalImage.empty())
    {
        if(!DrawImage(hDC, (LPCTSTR)m_sNormalImage.c_str()))
        {
            m_sNormalImage.clear();
        }
        else
        {
            goto Label_ForeImage;
        }
    }

    if(!m_sForeImage.empty())
    {
        goto Label_ForeImage;
    }

    return;
Label_ForeImage:

    if(!m_sForeImage.empty())
    {
        if(!DrawImage(hDC, (LPCTSTR)m_sForeImage.c_str()))
        {
            m_sForeImage.clear();
        }
    }
}

}