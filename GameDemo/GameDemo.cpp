// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>

#include "MiniDumper.h"
CMiniDumper g_miniDumper( true );

#include "ControlEx.h"

class CLoginFrameWnd : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
public:
    CLoginFrameWnd() { };
    LPCTSTR GetWindowClassName() const { return _T("UILoginFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; };
    void OnFinalMessage(HWND /*hWnd*/) 
    { 
        m_pm.RemovePreMessageFilter(this);
        delete this; 
    };

    void Init() {
        CComboUI* pAccountCombo = static_cast<CComboUI*>(m_pm.FindControl(_T("accountcombo")));
        CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
        if( pAccountCombo && pAccountEdit ) pAccountEdit->SetText(pAccountCombo->GetText());
        pAccountEdit->SetFocus();
    }

    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("click") ) {
            if( msg.pSender->GetName() == _T("closebtn") ) { PostQuitMessage(0); return; }
            else if( msg.pSender->GetName() == _T("loginBtn") ) { Close(); return; }
        }
        else if( msg.sType == _T("itemselect") ) {
            if( msg.pSender->GetName() == _T("accountcombo") ) {
                CEditUI* pAccountEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
                if( pAccountEdit ) pAccountEdit->SetText(msg.pSender->GetText());
            }
        }
    }

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        m_pm.Init(m_hWnd);
        m_pm.AddPreMessageFilter(this);
        CDialogBuilder builder;
        CDialogBuilderCallbackEx cb;
        CControlUI* pRoot = builder.Create(_T("login.xml"), (UINT)0, &cb, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);

        Init();
        return 0;
    }

    LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( ::IsIconic(*this) ) bHandled = FALSE;
        return (wParam == 0) ? TRUE : FALSE;
    }

    LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(*this, &pt);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);

        RECT rcCaption = m_pm.GetCaptionRect();
        if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
            && pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
                if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 )
                    return HTCAPTION;
        }

        return HTCLIENT;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SIZE szRoundCorner = m_pm.GetRoundCorner();
        if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
            CDuiRect rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            rcWnd.right++; rcWnd.bottom++;
            HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }

        bHandled = FALSE;
        return 0;
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch( uMsg ) {
        case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
        case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
        case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
        case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
        case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
        default:
            bHandled = FALSE;
        }
        if( bHandled ) return lRes;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
    {
        if( uMsg == WM_KEYDOWN ) {
            if( wParam == VK_RETURN ) {
                CEditUI* pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("accountedit")));
                if( pEdit->GetText().IsEmpty() ) pEdit->SetFocus();
                else {
                    pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("pwdedit")));
                    if( pEdit->GetText().IsEmpty() ) pEdit->SetFocus();
                    else Close();
                }
                return true;
            }
            else if( wParam == VK_ESCAPE ) {
                PostQuitMessage(0);
                return true;
            }

        }
        return false;
    }

public:
    CPaintManagerUI m_pm;
};

class CGameFrameWnd : public CWindowWnd, public INotifyUI, public IListCallbackUI
{
public:
    CGameFrameWnd() { };
    LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
    UINT GetClassStyle() const { return CS_DBLCLKS; };
    void OnFinalMessage(HWND /*hWnd*/) { delete this; };

    void Init() {
        m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
        m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
        m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
        m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));

        CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("ie")));
        if( pActiveXUI ) {
            IWebBrowser2* pWebBrowser = NULL;
            pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
            if( pWebBrowser != NULL ) {
                pWebBrowser->Navigate(L"https://github.com/duilib/duilib",NULL,NULL,NULL,NULL);  
                //pWebBrowser->Navigate(L"about:blank",NULL,NULL,NULL,NULL); 
                pWebBrowser->Release();
            }
        }
    }

    void OnPrepare() { 
        GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("gamelist")));
        GameListUI::Node* pCategoryNode = NULL;
        GameListUI::Node* pGameNode = NULL;
        GameListUI::Node* pServerNode = NULL;
        GameListUI::Node* pRoomNode = NULL;
        pCategoryNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}推荐游戏"));
        for( int i = 0; i < 4; ++i )
        {
            pGameNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}四人斗地主"), pCategoryNode);
            for( int i = 0; i < 3; ++i )
            {
                pServerNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}测试服务器"), pGameNode);
                for( int i = 0; i < 3; ++i )
                {
                    pRoomNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}测试房间"), pServerNode);
                }
            }
        }
        pCategoryNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}最近玩过的游戏"));
        for( int i = 0; i < 2; ++i )
        {
            pGameList->AddNode(_T("三缺一"), pCategoryNode);
        }
        pCategoryNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}棋牌游戏"));
        for( int i = 0; i < 8; ++i )
        {
            pGameList->AddNode(_T("双扣"), pCategoryNode);
        }
        pCategoryNode = pGameList->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}休闲游戏"));
        for( int i = 0; i < 8; ++i )
        {
            pGameList->AddNode(_T("飞行棋"), pCategoryNode);
        }

        CListUI* pUserList = static_cast<CListUI*>(m_pm.FindControl(_T("userlist")));
        pUserList->SetTextCallback(this);      
        for( int i = 0; i < 400; i++ ) {
            CListTextElementUI* pListElement = new CListTextElementUI;
            pUserList->Add(pListElement);
        }

        CLoginFrameWnd* pLoginFrame = new CLoginFrameWnd();
        if( pLoginFrame == NULL ) { Close(); return; }
        pLoginFrame->Create(m_hWnd, _T(""), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0, NULL);
		pLoginFrame->SetIcon(IDI_ICON_DUILIB);
        pLoginFrame->CenterWindow();
        pLoginFrame->ShowModal();
    }

    void SendChatMessage() {
        CEditUI* pChatEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("chatEdit")));
        if( pChatEdit == NULL ) return;
        pChatEdit->SetFocus();
        if( pChatEdit->GetText().IsEmpty() ) return;

        CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pm.FindControl(_T("chatmsglist")));
        if( pRichEdit == NULL ) return;
        long lSelBegin = 0, lSelEnd = 0;
        CHARFORMAT2 cf;
        ZeroMemory(&cf, sizeof(CHARFORMAT2));
        cf.cbSize = sizeof(cf);
        cf.dwReserved = 0;
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = RGB(220, 0, 0);

        lSelEnd = lSelBegin = pRichEdit->GetTextLength();
        pRichEdit->SetSel(lSelEnd, lSelEnd);
        pRichEdit->ReplaceSel(_T("某人"), false);
        lSelEnd = pRichEdit->GetTextLength();
        pRichEdit->SetSel(lSelBegin, lSelEnd);
        pRichEdit->SetSelectionCharFormat(cf);

        lSelBegin = lSelEnd;
        pRichEdit->SetSel(-1, -1);
        pRichEdit->ReplaceSel(_T("说:"), false);

        pRichEdit->SetSel(-1, -1);
        pRichEdit->ReplaceSel(pChatEdit->GetText(), false);
        pChatEdit->SetText(_T(""));

        pRichEdit->SetSel(-1, -1);
        pRichEdit->ReplaceSel(_T("\n"), false);

        cf.crTextColor = RGB(0, 0, 0);
        lSelEnd = pRichEdit->GetTextLength();
        pRichEdit->SetSel(lSelBegin, lSelEnd);
        pRichEdit->SetSelectionCharFormat(cf);

        pRichEdit->EndDown();
    }

    void Notify(TNotifyUI& msg)
    {
        if( msg.sType == _T("windowinit") ) OnPrepare();
        else if( msg.sType == _T("click") ) {
            if( msg.pSender == m_pCloseBtn ) { 
                COptionUI* pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("hallswitch")));
                if( pControl && pControl->IsSelected() == false ) {
                    CControlUI* pFadeControl = m_pm.FindControl(_T("fadeEffect"));
                    if( pFadeControl ) pFadeControl->SetVisible(true);
                }
                else {
                    /*Close()*/PostQuitMessage(0); // 因为activex的原因，使用close可能会出现错误
                }
                return; 
            }
            else if( msg.pSender == m_pMinBtn ) { SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
            else if( msg.pSender == m_pMaxBtn ) { SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
            else if( msg.pSender == m_pRestoreBtn ) { SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
            CDuiString name = msg.pSender->GetName();
            if( name == _T("quitbtn") ) {
                /*Close()*/PostQuitMessage(0); // 因为activex的原因，使用close可能会出现错误
            }
            else if( name == _T("returnhallbtn") ) {
                CControlUI* pFadeControl = m_pm.FindControl(_T("fadeEffect"));
                if( pFadeControl ) pFadeControl->SetVisible(false); 

                COptionUI* pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("hallswitch")));
                pControl->Activate();
                pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("roomswitch")));
                if( pControl ) pControl->SetVisible(false);
            }
            else if( name == _T("fontswitch") ) {
                TFontInfo* pFontInfo = m_pm.GetDefaultFontInfo();
                if( pFontInfo->iSize < 18 ) {
                    TFontInfo* pFontInfo = m_pm.GetFontInfo(0);
                    if( pFontInfo )m_pm.SetDefaultFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, 
                        pFontInfo->bUnderline, pFontInfo->bItalic);
                }
                else {
                    TFontInfo* pFontInfo = m_pm.GetFontInfo(1);
                    if( pFontInfo )m_pm.SetDefaultFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, 
                        pFontInfo->bUnderline, pFontInfo->bItalic);
                }
                m_pm.GetRoot()->NeedUpdate();
            }
            else if( name == _T("leaveBtn")  || name == _T("roomclosebtn") ) {
                COptionUI* pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("hallswitch")));
                if( pControl ) {
                    pControl->Activate();
                    pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("roomswitch")));
                    if( pControl ) pControl->SetVisible(false);
                }
            }
            else if( name == _T("sendbtn") ) {
                SendChatMessage();
            }
        }
        else if( msg.sType == _T("selectchanged") ) {
            CDuiString name = msg.pSender->GetName();
            if( name == _T("hallswitch") ) {
                CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
                if( pControl && pControl->GetCurSel() != 0 ) pControl->SelectItem(0);
            }
            else if( name == _T("roomswitch") ) {
                CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
                if( pControl && pControl->GetCurSel() != 1 ) {
                    pControl->SelectItem(1);
                    DeskListUI* pDeskList = static_cast<DeskListUI*>(m_pm.FindControl(_T("destlist")));
                    pDeskList->SetFocus();
                    CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pm.FindControl(_T("chatmsglist")));
                    if( pRichEdit ) {
                        pRichEdit->SetText(_T("欢迎进入XXX游戏，祝游戏愉快！\n\n"));
                        long lSelBegin = 0, lSelEnd = 0;
                        CHARFORMAT2 cf;
                        ZeroMemory(&cf, sizeof(CHARFORMAT2));
                        cf.cbSize = sizeof(cf);
                        cf.dwReserved = 0;
                        cf.dwMask = CFM_COLOR;
                        cf.crTextColor = RGB(255, 0, 0);
                        lSelEnd = pRichEdit->GetTextLength();
                        pRichEdit->SetSel(lSelBegin, lSelEnd);
                        pRichEdit->SetSelectionCharFormat(cf);
                    }
                }
            }
        }
        else if( msg.sType == _T("itemclick") ) {
            GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("gamelist")));
            if( pGameList->GetItemIndex(msg.pSender) != -1 )
            {
                if( _tcscmp(msg.pSender->GetClass(), DUI_CTR_LISTLABELELEMENT) == 0 ) {
                    GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();

                    POINT pt = { 0 };
                    ::GetCursorPos(&pt);
                    ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
                    pt.x -= msg.pSender->GetX();
                    SIZE sz = pGameList->GetExpanderSizeX(node);
                    if( pt.x >= sz.cx && pt.x < sz.cy )                     
                        pGameList->ExpandNode(node, !node->data()._expand);
                }
            }
        }
        else if( msg.sType == _T("itemactivate") ) {
            GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("gamelist")));
            if( pGameList->GetItemIndex(msg.pSender) != -1 )
            {
                if( _tcscmp(msg.pSender->GetClass(), DUI_CTR_LISTLABELELEMENT) == 0 ) {
                    GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();
                    pGameList->ExpandNode(node, !node->data()._expand);
                    if( node->data()._level == 3 ) {
                        COptionUI* pControl = static_cast<COptionUI*>(m_pm.FindControl(_T("roomswitch")));
                        if( pControl ) {
                            pControl->SetVisible(true);
                            pControl->SetText(node->parent()->parent()->data()._text);
                            pControl->Activate();

                        }
                    }
                }
            }
        }
        else if( msg.sType == _T("itemselect") ) {
            if( msg.pSender->GetName() == _T("chatCombo") ) {
                CEditUI* pChatEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("chatEdit")));
                if( pChatEdit ) pChatEdit->SetText(msg.pSender->GetText());
                static_cast<CComboUI*>(msg.pSender)->SelectItem(-1);
            }
        }
        else if( msg.sType == _T("return") ) {
            if( msg.pSender->GetName() == _T("chatEdit") ) {
                SendChatMessage();
            }
        }
    }

    LPCTSTR GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
    {
        if( pControl->GetParent()->GetParent()->GetName() == _T("userlist") ) {
            if( iSubItem == 0 ) return _T("<i vip.png>");
            if( iSubItem == 1 ) return _T("<i vip.png>");
            if( iSubItem == 2 ) return _T("此人昵称");
            if( iSubItem == 3 ) return _T("5");
            if( iSubItem == 4 ) return _T("50%");
            if( iSubItem == 5 ) return _T("0%");
            if( iSubItem == 6 ) return _T("100");
        }

        return _T("");
    }

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        m_pm.Init(m_hWnd);
        CDialogBuilder builder;
        CDialogBuilderCallbackEx cb;
        CControlUI* pRoot = builder.Create(_T("hall.xml"), (UINT)0, &cb, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);

        Init();
        return 0;
    }

    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ::PostQuitMessage(0L);

        bHandled = FALSE;
        return 0;
    }

    LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if( ::IsIconic(*this) ) bHandled = FALSE;
        return (wParam == 0) ? TRUE : FALSE;
    }

    LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(*this, &pt);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);

        if( !::IsZoomed(*this) ) {
            RECT rcSizeBox = m_pm.GetSizeBox();
            if( pt.y < rcClient.top + rcSizeBox.top ) {
                if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
                if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
                return HTTOP;
            }
            else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
                if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
                if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
                return HTBOTTOM;
            }
            if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
            if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
        }

        RECT rcCaption = m_pm.GetCaptionRect();
        if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
            && pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
                if( pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 && 
                    _tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
                    _tcscmp(pControl->GetClass(), DUI_CTR_TEXT) != 0 )
                    return HTCAPTION;
        }

        return HTCLIENT;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SIZE szRoundCorner = m_pm.GetRoundCorner();
        if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
            CDuiRect rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            rcWnd.right++; rcWnd.bottom++;
            HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }

        bHandled = FALSE;
        return 0;
    }

    LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        int primaryMonitorWidth = ::GetSystemMetrics(SM_CXSCREEN);
        int primaryMonitorHeight = ::GetSystemMetrics(SM_CYSCREEN);
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        CDuiRect rcWork = oMonitor.rcWork;
        rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);
        if (rcWork.right > primaryMonitorWidth) rcWork.right = primaryMonitorWidth;
        if (rcWork.bottom > primaryMonitorHeight) rcWork.right = primaryMonitorHeight;
        LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
        lpMMI->ptMaxPosition.x = rcWork.left;
        lpMMI->ptMaxPosition.y = rcWork.top;
        lpMMI->ptMaxSize.x = rcWork.right;
        lpMMI->ptMaxSize.y = rcWork.bottom;
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        // 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
        if( wParam == SC_CLOSE ) {
            ::PostQuitMessage(0L);
            bHandled = TRUE;
            return 0;
        }
        BOOL bZoomed = ::IsZoomed(*this);
        LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        if( ::IsZoomed(*this) != bZoomed ) {
            if( !bZoomed ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
                if( pControl ) pControl->SetVisible(false);
                pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
                if( pControl ) pControl->SetVisible(true);
            }
            else {
                CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
                if( pControl ) pControl->SetVisible(true);
                pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
                if( pControl ) pControl->SetVisible(false);
            }
        }
        return lRes;
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch( uMsg ) {
        case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
        case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
        case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
        case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
        case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
        case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
        case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
        case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
        case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
        default:
            bHandled = FALSE;
        }
        if( bHandled ) return lRes;
        if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }

public:
    CPaintManagerUI m_pm;

private:
    CButtonUI* m_pCloseBtn;
    CButtonUI* m_pMaxBtn;
    CButtonUI* m_pRestoreBtn;
    CButtonUI* m_pMinBtn;
    //...
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
#if 0
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
    CPaintManagerUI::SetResourceZip(_T("GameRes.zip"));
#else
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin/GameRes"));
#endif

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CGameFrameWnd* pFrame = new CGameFrameWnd();
    if( pFrame == NULL ) return 0;
	pFrame->SetIcon(IDI_ICON_DUILIB);
    pFrame->Create(NULL, _T("游戏中心"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 1024, 738);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOWMAXIMIZED);

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}
