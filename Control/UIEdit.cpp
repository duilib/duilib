#include "stdafx.h"
#include "UIEdit.h"
#include <windowsx.h>

namespace DuiLib
{
UI_IMPLEMENT_DYNCREATE(CEditUI);

class CEditWnd : public CWindowWnd
{
public:
    CEditWnd();

    void Init(CEditUI *pOwner);
    RECT CalPos();

    LPCTSTR GetWindowClassName() const;
    LPCTSTR GetSuperClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);


protected:
    CEditUI *m_pOwner;
    HBRUSH m_hBkBrush;
    bool m_bInit;
};


CEditWnd::CEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
{
}

void CEditWnd::Init(CEditUI *pOwner)
{
    m_pOwner = pOwner;
    CDuiRect rcPos = CalPos();
    UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;

    if(m_pOwner->IsPasswordMode())
    {
        uStyle |= ES_PASSWORD;
    }

    Create(m_pOwner->GetManager()->GetHWND(), NULL, uStyle, 0, rcPos.left, rcPos.top, rcPos.GetWidth(), rcPos.GetHeight());
    HFONT hFont = NULL;
    int iFontIndex = m_pOwner->GetFont();

    if(iFontIndex != -1)
    {
        hFont = GetResEngine()->GetFont(m_pOwner->GetFontID())->GetFont();
    }

    if(hFont == NULL)
    {
        hFont = GetResEngine()->GetDefaultFont()->GetFont();
    }

    SetWindowFont(m_hWnd, hFont, TRUE);
    Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());

    if(m_pOwner->IsPasswordMode())
    {
        Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
    }

    ::SetWindowText(m_hWnd, m_pOwner->GetText());
    //Edit_SetText(m_hWnd, m_pOwner->GetText());
    Edit_SetModify(m_hWnd, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
    Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
    Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
    //Styls
    LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
    styleValue |= pOwner->GetWindowStyls();
    ::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);
    m_bInit = true;
}

RECT CEditWnd::CalPos()
{
    CDuiRect rcPos = m_pOwner->GetPos();
    RECT rcInset = m_pOwner->GetTextPadding();
    rcPos.left += rcInset.left;
    rcPos.top += rcInset.top;
    rcPos.right -= rcInset.right;
    rcPos.bottom -= rcInset.bottom;
    LONG lEditHeight = 0;
    lEditHeight = GetResEngine()->GetFont(m_pOwner->GetFontID())->GetTextMetric().tmHeight;

    if(lEditHeight < rcPos.GetHeight())
    {
        rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
        rcPos.bottom = rcPos.top + lEditHeight;
    }

    return rcPos;
}

LPCTSTR CEditWnd::GetWindowClassName() const
{
    return _T("EditWnd");
}

LPCTSTR CEditWnd::GetSuperClassName() const
{
    return WC_EDIT;
}

void CEditWnd::OnFinalMessage(HWND /*hWnd*/)
{
    m_pOwner->Invalidate();

    // Clear reference and die
    if(m_hBkBrush != NULL)
    {
        ::DeleteObject(m_hBkBrush);
    }

    m_pOwner->m_pWindow = NULL;
    delete this;
}

LRESULT CEditWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    if(uMsg == WM_KILLFOCUS)
    {
        lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
    }
    else if(uMsg == WM_SETFOCUS)
    {
        lRes = OnSetFocus(uMsg, wParam, lParam, bHandled);
    }
    else if(uMsg == OCM_COMMAND)
    {
        if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
        {
            lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
        }
        else if(GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
        {
            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::InvalidateRect(m_hWnd, &rcClient, FALSE);
        }
    }
    else if(uMsg == WM_KEYDOWN)
    {
        if(TCHAR(wParam) == VK_RETURN)
        {
            bHandled = TRUE;
            m_pOwner->GetManager()->SendNotify(m_pOwner, UINOTIFY_EDIT_RETURN);
        }
        else
        {
            bHandled = FALSE;
        }
    }
    else if(uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC)
    {
        if(m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF)
        {
            return NULL;
        }

        ::SetBkMode((HDC)wParam, TRANSPARENT);
        DWORD dwTextColor = m_pOwner->GetTextColor();
        ::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

        if(m_hBkBrush == NULL)
        {
            DWORD clrColor = m_pOwner->GetNativeEditBkColor();
            m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
        }

        return (LRESULT)m_hBkBrush;
    }
    else
    {
        bHandled = FALSE;
    }

    if(!bHandled)
    {
        return CWindowWnd::WindowProc(uMsg, wParam, lParam);
    }

    return lRes;
}



LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

    if(m_pOwner->m_sText.empty() && m_pOwner->m_sDefaultText.empty() == FALSE)
    {
        m_pOwner->ShowDefaultText(TRUE);
    }

    PostMessage(WM_CLOSE);
    return lRes;
}

LRESULT CEditWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    if(m_pOwner->m_bShowDefault)
    {
        m_pOwner->ShowDefaultText(FALSE);
    }

    bHandled = FALSE;
    //LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    return 0;
}

LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    if(!m_bInit)
    {
        return 0;
    }

    if(m_pOwner == NULL)
    {
        return 0;
    }

    // Copy text back
    int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
    LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
    ASSERT(pstr);

    if(pstr == NULL)
    {
        return 0;
    }

    ::GetWindowText(m_hWnd, pstr, cchLen);
    m_pOwner->m_sText = pstr;
    m_pOwner->GetManager()->SendNotify(m_pOwner, UINOTIFY_EDIT_CHANGE);
    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CEditUI::CEditUI() : m_pWindow(NULL), m_uMaxChar(255), m_bReadOnly(false),
    m_bPasswordMode(false), m_cPasswordChar(_T('*')), m_uButtonState(0),
    m_dwEditbkColor(0xFFFFFFFF), m_iWindowStyls(0)
    , m_bShowDefault(FALSE), m_clrDefaultText(0), m_backupPasswordChar(0), m_backupTextClr(0), m_backupPasswordMode(false)
    , m_bInnerSetText(FALSE), m_bInitFinish(FALSE)
{
    SetTextPadding(CDuiRect(4, 3, 4, 3));
    SetBkColor(0xFFFFFFFF);
}

LPCTSTR CEditUI::GetClass() const
{
    return _T("EditUI");
}

LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
{
    if(_tcscmp(pstrName, _T("Edit")) == 0)
    {
        return static_cast<CEditUI *>(this);
    }

    return CLabelUI::GetInterface(pstrName);
}

bool CEditUI::IsClientArea()
{
    return true;
}

UINT CEditUI::GetControlFlags() const
{
    if(!IsEnabled())
    {
        return CControlUI::GetControlFlags();
    }

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

void CEditUI::EventHandler(TEventUI & event)
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

    if(event.dwType == UIEVENT_SETCURSOR && IsEnabled())
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
        return;
    }

    if(event.dwType == UIEVENT_WINDOWSIZE)
    {
        if(m_pWindow != NULL)
        {
            m_pManager->SetFocusNeeded(this);
        }
    }

    if(event.dwType == UIEVENT_SCROLLWHEEL)
    {
        if(m_pWindow != NULL)
        {
            return;
        }
    }

    if(event.dwType == UIEVENT_SETFOCUS)
    {
        if(!IsEnabled())
        {
            return;
        }

        if(m_pWindow)
        {
            return;
        }

        m_pWindow = new CEditWnd();
        ASSERT(m_pWindow);
        m_pWindow->Init(this);
        Invalidate();
    }

    if(event.dwType == UIEVENT_KILLFOCUS && IsEnabled())
    {
        Invalidate();
    }

    if(event.dwType == UIEVENT_LBUTTONDOWN || event.dwType == UIEVENT_LDBLCLICK || event.dwType == UIEVENT_RBUTTONDOWN)
    {
        if(IsEnabled())
        {
            GetManager()->ReleaseCapture();

            if(IsFocused() && m_pWindow == NULL)
            {
                m_pWindow = new CEditWnd();
                ASSERT(m_pWindow);
                m_pWindow->Init(this);

                if(PtInRect(&m_rcItem, event.ptMouse))
                {
                    int nSize = GetWindowTextLength(*m_pWindow);

                    if(nSize == 0)
                    {
                        nSize = 1;
                    }

                    Edit_SetSel(*m_pWindow, 0, nSize);
                }
            }
            else if(m_pWindow != NULL)
            {
#if 1
                int nSize = GetWindowTextLength(*m_pWindow);

                if(nSize == 0)
                {
                    nSize = 1;
                }

                Edit_SetSel(*m_pWindow, 0, nSize);
#else
                POINT pt = event.ptMouse;
                pt.x -= m_rcItem.left + m_rcTextPadding.left;
                pt.y -= m_rcItem.top + m_rcTextPadding.top;
                ::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
            }
        }

        return;
    }

    if(event.dwType == UIEVENT_MOUSEMOVE)
    {
        return;
    }

    if(event.dwType == UIEVENT_LBUTTONUP)
    {
        return;
    }

    if(event.dwType == UIEVENT_CONTEXTMENU)
    {
        return;
    }

    if(event.dwType == UIEVENT_MOUSEENTER)
    {
        if(IsEnabled())
        {
            m_uButtonState |= UISTATE_Hover;
            Invalidate();
        }

        return;
    }

    if(event.dwType == UIEVENT_MOUSELEAVE)
    {
        if(IsEnabled())
        {
            m_uButtonState &= ~UISTATE_Hover;
            Invalidate();
        }

        return;
    }

    CLabelUI::EventHandler(event);
}

void CEditUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if(!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

void CEditUI::SetText(LPCTSTR pstrText)
{
    if(m_bInnerSetText == FALSE)
    {
        if(_tcslen(pstrText) == 0)
        {
            if(m_bShowDefault)
            {
                return ;
            }
            else if(m_bFocused == FALSE)//?T?1¦Ì?, ?¨°??¨º???¨¨???¡À?
            {
                m_sText = pstrText;

                if(m_pWindow != NULL)
                {
                    Edit_SetText(*m_pWindow, m_sText.c_str());    //¨¦¨¨???a??, ¡À¨¹?a?¨²2??D??3¡è?¨¨¡ä¨ª?¨®
                }

                ShowDefaultText(TRUE);
                return ;
            }
        }

        ShowDefaultText(FALSE);
    }

    m_sText = pstrText;

    if(m_pWindow != NULL)
    {
        Edit_SetText(*m_pWindow, m_sText.c_str());
    }

    Invalidate();
}

void CEditUI::SetMaxChar(UINT uMax)
{
    m_uMaxChar = uMax;

    if(m_pWindow != NULL)
    {
        Edit_LimitText(*m_pWindow, m_uMaxChar);
    }
}

UINT CEditUI::GetMaxChar()
{
    return m_uMaxChar;
}

void CEditUI::SetReadOnly(bool bReadOnly)
{
    if(m_bReadOnly == bReadOnly)
    {
        return;
    }

    m_bReadOnly = bReadOnly;

    if(m_pWindow != NULL)
    {
        Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
    }

    Invalidate();
}

bool CEditUI::IsReadOnly() const
{
    return m_bReadOnly;
}

void CEditUI::SetNumberOnly(bool bNumberOnly)
{
    if(bNumberOnly)
    {
        m_iWindowStyls |= ES_NUMBER;
    }
    else
    {
        m_iWindowStyls &= ~ES_NUMBER;
    }
}

bool CEditUI::IsNumberOnly() const
{
    return m_iWindowStyls & ES_NUMBER ? true : false;
}

int CEditUI::GetWindowStyls() const
{
    return m_iWindowStyls;
}

void CEditUI::SetPasswordMode(bool bPasswordMode)
{
    if(m_bPasswordMode == bPasswordMode)
    {
        return;
    }

    m_bPasswordMode = bPasswordMode;
    Invalidate();
}

bool CEditUI::IsPasswordMode() const
{
    return m_bPasswordMode;
}

void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
{
    if(m_cPasswordChar == cPasswordChar)
    {
        return;
    }

    m_cPasswordChar = cPasswordChar;

    if(m_pWindow != NULL)
    {
        Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
    }

    Invalidate();
}

TCHAR CEditUI::GetPasswordChar() const
{
    return m_cPasswordChar;
}

LPCTSTR CEditUI::GetNormalImage()
{
    return m_sNormalImage.c_str();
}

void CEditUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetHotImage()
{
    return m_sHotImage.c_str();
}

void CEditUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetFocusedImage()
{
    return m_sFocusedImage.c_str();
}

void CEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetDisabledImage()
{
    return m_sDisabledImage.c_str();
}

void CEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

void CEditUI::SetNativeEditBkColor(DWORD dwBkColor)
{
    m_dwEditbkColor = dwBkColor;
}

DWORD CEditUI::GetNativeEditBkColor() const
{
    return m_dwEditbkColor;
}

void CEditUI::SetSel(long nStartChar, long nEndChar)
{
    if(m_pWindow != NULL)
    {
        Edit_SetSel(*m_pWindow, nStartChar, nEndChar);
    }
}

void CEditUI::SetSelAll()
{
    SetSel(0, -1);
}

void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
{
    if(m_pWindow != NULL)
    {
        Edit_ReplaceSel(*m_pWindow, lpszReplace);
    }
}

void CEditUI::SetPos(RECT rc, bool bNeedInvalidate /*= true*/)
{
    CControlUI::SetPos(rc, bNeedInvalidate);

    if(m_pWindow != NULL)
    {
        RECT rcPos = m_pWindow->CalPos();
        ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                       rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void CEditUI::Move(SIZE szOffset, bool bNeedInvalidate /*= true*/)
{
    CControlUI::Move(szOffset, bNeedInvalidate);

    if(m_pWindow != NULL)
    {
        RECT rcPos = m_pWindow->CalPos();
        ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                       rcPos.bottom - rcPos.top, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void CEditUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);

    if(!IsVisible() && m_pWindow != NULL)
    {
        m_pManager->SetFocus(NULL);
    }
}

void CEditUI::SetInternVisible(bool bVisible)
{
    if(!IsVisible() && m_pWindow != NULL)
    {
        m_pManager->SetFocus(NULL);
    }
}

SIZE CEditUI::EstimateSize(SIZE szAvailable)
{
    if(m_cxyFixed.cy == 0)
    {
        return CDuiSize(m_cxyFixed.cx, GetResEngine()->GetFont(GetFontID())->GetTextMetric().tmHeight + 6);
    }

    return CControlUI::EstimateSize(szAvailable);
}

void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if(_tcscmp(pstrName, _T("readonly")) == 0)
    {
        SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if(_tcscmp(pstrName, _T("numberonly")) == 0)
    {
        SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if(_tcscmp(pstrName, _T("password")) == 0)
    {
        SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if(_tcscmp(pstrName, _T("maxchar")) == 0)
    {
        SetMaxChar(_ttoi(pstrValue));
    }
    else if(_tcscmp(pstrName, _T("normalimage")) == 0)
    {
        SetNormalImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("hotimage")) == 0)
    {
        SetHotImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("focusedimage")) == 0)
    {
        SetFocusedImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("disabledimage")) == 0)
    {
        SetDisabledImage(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("nativebkcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetNativeEditBkColor(clrColor);
    }
    else if(_tcscmp(pstrName, _T("defaulttext")) == 0)
    {
        SetDefaultText(pstrValue);
    }
    else if(_tcscmp(pstrName, _T("defaulttextcolor")) == 0)
    {
        if(*pstrValue == _T('#'))
        {
            pstrValue = ::CharNext(pstrValue);
        }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetDefaultTextColor(clrColor);
    }
    else
    {
        CLabelUI::SetAttribute(pstrName, pstrValue);
    }
}



void CEditUI::PaintStatusImage(HDC hDC)
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
                return;
            }
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
                return;
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
            else
            {
                return;
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
            return;
        }
    }
}

void CEditUI::PaintText(HDC hDC)
{
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

    CDuiString sText = m_sText;

    if(m_bPasswordMode && m_bShowDefault == FALSE)
    {
        sText.clear();
        LPCTSTR p = m_sText.c_str();

        while(*p != _T('\0'))
        {
            sText += m_cPasswordChar;
            p = ::CharNext(p);
        }
    }

    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

    if(IsEnabled())
    {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText.c_str(), m_dwTextColor, \
                                m_strFontID.c_str(), DT_SINGLELINE | m_uTextStyle);
    }
    else
    {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText.c_str(), m_dwDisabledTextColor, \
                                m_strFontID.c_str(), DT_SINGLELINE | m_uTextStyle);
    }
}

void CEditUI::SetDefaultText(LPCTSTR strText)
{
    m_sDefaultText = strText;
}

void CEditUI::SetDefaultTextColor(DWORD clr)
{
    m_clrDefaultText = clr;
}

void CEditUI::Init()
{
    CLabelUI::Init();

    if(m_sText.empty())
    {
        ShowDefaultText(TRUE);
    }

    m_bInitFinish = TRUE;
}

void CEditUI::ShowDefaultText(BOOL bShow)
{
    if(bShow)
    {
        m_bShowDefault = TRUE;
        m_backupTextClr = m_dwTextColor;
        m_backupPasswordChar = m_cPasswordChar;
        m_backupPasswordMode = m_bPasswordMode;

        if(m_backupPasswordMode)
        {
            SetPasswordMode(false);
            SetPasswordChar(0);
        }

        SetTextColor(m_clrDefaultText);
        m_bInnerSetText = TRUE;
        SetText(m_sDefaultText.c_str());
        m_bInnerSetText = FALSE;
    }
    else
    {
        if(m_bShowDefault == TRUE)
        {
            if(m_backupPasswordMode == true)
            {
                SetPasswordMode(true);
                SetPasswordChar(m_backupPasswordChar);
            }

            m_bInnerSetText = TRUE;
            SetText(TEXT(""));
            m_bInnerSetText = FALSE;
            m_bShowDefault = FALSE;
            SetTextColor(m_backupTextClr);
        }
    }
}

LPCTSTR CEditUI::GetText() const
{
    if(m_bShowDefault)
    {
        return TEXT("");
    }
    else
    {
        return m_sText.c_str();
    }
}



}
