#include "stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
#include <shellapi.h>
#endif

#include "UIGroups.hpp"
#include "UIMicroBlog.hpp"
#include "main_frame.hpp"
#include "color_skin.hpp"
#include "chat_dialog.hpp"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

const TCHAR* const kTabControlName = _T("tabs");

const TCHAR* const kFriendButtonControlName = _T("friendbtn");
const TCHAR* const kGroupButtonControlName = _T("groupbtn");
const TCHAR* const kMicroBlogButtonControlName = _T("microblogbtn");

const TCHAR* const kFriendsListControlName = _T("friends");
const TCHAR* const kGroupsListControlName = _T("groups");
const TCHAR* const kMicroBlogListControlName = _T("microblog");

const TCHAR* const kHideLeftMainPannelControlName = _T("btnleft");
const TCHAR* const kShowLeftMainPannelControlName = _T("btnright");
const TCHAR* const kLeftMainPannelControlName = _T("LeftMainPanel");

const TCHAR* const kSignatureTipsControlName = _T("signaturetip");
const TCHAR* const kSignatureControlName = _T("signature");

const TCHAR* const kSearchEditTipControlName = _T("search_tip");
const TCHAR* const kSearchEditControlName = _T("search_edit");

const TCHAR* const kChangeBkSkinControlName = _T("bkskinbtn");
const TCHAR* const kChangeColorSkinControlName = _T("colorskinbtn");

const TCHAR* const kBackgroundControlName = _T("bg");

const int kBackgroundSkinImageCount = 3;

MainFrame::MainFrame()
: bk_image_index_(0)
{}

MainFrame::~MainFrame()
{
	PostQuitMessage(0);
}

LPCTSTR MainFrame::GetWindowClassName() const
{
	return _T("TXGuiFoundation");
}

CControlUI* MainFrame::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, _T("FriendList")) == 0)
	{
		return new CFriendsUI(m_PaintManager);
	}
	else if (_tcsicmp(pstrClass, _T("GroupList")) == 0)
	{
		return new CGroupsUI(m_PaintManager);
	}
	else if (_tcsicmp(pstrClass, _T("MicroBlog")) == 0)
	{
		return new CMicroBlogUI(m_PaintManager);
	}

	return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

CDuiString MainFrame::GetSkinFile()
{
	return _T("main_frame.xml");
}

CDuiString MainFrame::GetSkinFolder()
{
	return  _T("skin\\");
}

LRESULT MainFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{
		if (!bZoomed)
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
		}
		else 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
		}
	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

void MainFrame::OnTimer(TNotifyUI& msg)
{
}

void MainFrame::OnExit(TNotifyUI& msg)
{
	Close();
}

void MainFrame::InitWindow()
{}

DWORD MainFrame::GetBkColor()
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
		return background->GetBkColor();

	return 0;
}

void MainFrame::SetBkColor(DWORD dwBackColor)
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		background->SetBkImage(_T(""));
		background->SetBkColor(dwBackColor);
		background->NeedUpdate();

		SkinChangedParam param;
		param.bkcolor = background->GetBkColor();
		param.bgimage = background->GetBkImage();
		skin_changed_observer_.Broadcast(param);
	}
}

void MainFrame::UpdateFriendsList()
{
	CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(m_PaintManager.FindControl(kFriendsListControlName));
	if (pFriendsList != NULL)
	{
		if (!friends_.empty())
			friends_.clear();
		if (pFriendsList->GetCount() > 0)
			pFriendsList->RemoveAll();

		FriendListItemInfo item;

		item.id = _T("0");
		item.folder = true;
		item.empty = false;
		item.nick_name = _T("我的好友");

		Node* root_parent = pFriendsList->AddNode(item, NULL);
		friends_.push_back(item);

		item.id = _T("1");
		item.folder = false;		
		item.logo = _T("man.png");
		item.nick_name = _T("tojen");
		item.description = _T("tojen.me@gmail.com");

		myself_info_ = item;

		pFriendsList->AddNode(item, root_parent);
		friends_.push_back(item);

		item.id = _T("2");
		item.folder = false;
		item.logo = _T("default.png");
		item.nick_name = _T("achellies");
		item.description = _T("achellies@hotmail.com");
		pFriendsList->AddNode(item, root_parent);
		friends_.push_back(item);

        item.id = _T("2");
        item.folder = false;
        item.logo = _T("default.png");
        item.nick_name = _T("wangchyz");
        item.description = _T("wangchyz@gmail.com");
        pFriendsList->AddNode(item, root_parent);
        friends_.push_back(item);

        for( int i = 0; i < 100; ++i )
        {
            item.id = _T("2");
            item.folder = false;
            item.logo = _T("default.png");
            item.nick_name = _T("duilib");
            item.description = _T("www.duilib.com");
            pFriendsList->AddNode(item, root_parent);
            friends_.push_back(item);
        }

		item.id = _T("3");
		item.folder = true;
		item.empty = false;
		item.nick_name = _T("企业好友");
		Node* root_parent2 = pFriendsList->AddNode(item, NULL);
		friends_.push_back(item);

		item.id = _T("4");
		item.folder = false;
		item.logo = _T("icon_home.png");
		item.nick_name = _T("腾讯企业QQ的官方展示号");
		item.description = _T("");
		pFriendsList->AddNode(item, root_parent2);
		friends_.push_back(item);

		item.id = _T("5");
		item.folder = true;
		item.empty = false;
		item.nick_name = _T("陌生人");
		Node* root_parent3 = pFriendsList->AddNode(item, NULL);
		friends_.push_back(item);


		item.id = _T("6");
		item.folder = true;
		item.empty = false;
		item.nick_name = _T("黑名单");
		Node* root_parent4 = pFriendsList->AddNode(item, NULL);
		friends_.push_back(item);
	}
}

void MainFrame::UpdateGroupsList()
{
	CGroupsUI* pGroupsList = static_cast<CGroupsUI*>(m_PaintManager.FindControl(kGroupsListControlName));
	if (pGroupsList != NULL)
	{
		if (pGroupsList->GetCount() > 0)
			pGroupsList->RemoveAll();

		GroupsListItemInfo item;

		item.folder = true;
		item.empty = false;
		item.nick_name = _T("我的QQ群");

		Node* root_parent = pGroupsList->AddNode(item, NULL);

		item.folder = false;
		item.logo = _T("duilib.png");
		item.nick_name = _T("Duilib官方交流群");
		item.description = _T("153787916");
		pGroupsList->AddNode(item, root_parent);

		item.folder = false;
		item.logo = _T("groups.png");
		item.nick_name = _T("Duilib官方交流群2");
		item.description = _T("79145400");
		pGroupsList->AddNode(item, root_parent);
	}
}

void MainFrame::UpdateMicroBlogList()
{
	CMicroBlogUI* pMicroBlogList = static_cast<CMicroBlogUI*>(m_PaintManager.FindControl(kMicroBlogListControlName));
	if (pMicroBlogList != NULL)
	{}
}

void MainFrame::OnPrepare(TNotifyUI& msg)
{
	CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
	if (background != NULL)
	{
		TCHAR szBuf[MAX_PATH] = {0};
#if defined(UNDER_WINCE)
		_stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#else
		_stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#endif
		background->SetBkImage(szBuf);
	}

	UpdateFriendsList();

	UpdateGroupsList();

	UpdateMicroBlogList();
}

void MainFrame::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("windowinit")) == 0)
	{
		OnPrepare(msg);
	}
	else if (_tcsicmp(msg.sType, _T("killfocus")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kSignatureControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CControlUI* signature_tip = m_PaintManager.FindControl(kSignatureTipsControlName);
			if (signature_tip != NULL)
			{
				CRichEditUI* signature = static_cast<CRichEditUI*>(msg.pSender);
				if (signature != NULL)
					signature_tip->SetText(signature->GetText());
				signature_tip->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSearchEditControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CControlUI* search_tip = static_cast<CRichEditUI*>(m_PaintManager.FindControl(kSearchEditTipControlName));
			if (search_tip != NULL)
			{
				CRichEditUI* search_edit = static_cast<CRichEditUI*>(msg.pSender);
				if (search_edit != NULL)
					search_tip->SetText(search_edit->GetText());
				search_tip->SetVisible(true);
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
		{
			OnExit(msg);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(true);
#else
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
		{
#if defined(UNDER_CE)
			::ShowWindow(m_hWnd, SW_RESTORE);
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
			if( pControl ) pControl->SetVisible(false);
#else
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
		}
		else if (_tcsicmp(msg.pSender->GetName(), kHideLeftMainPannelControlName) == 0)
		{
			CControlUI* left_main_pannel = m_PaintManager.FindControl(kLeftMainPannelControlName);
			CControlUI* hide_left_main_pannel = m_PaintManager.FindControl(kHideLeftMainPannelControlName);
			CControlUI* show_left_main_pannel = m_PaintManager.FindControl(kShowLeftMainPannelControlName);
			if ((left_main_pannel != NULL) && (show_left_main_pannel != NULL) && (hide_left_main_pannel != NULL))
			{
				hide_left_main_pannel->SetVisible(false);
				left_main_pannel->SetVisible(false);
				show_left_main_pannel->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kShowLeftMainPannelControlName) == 0)
		{
			CControlUI* left_main_pannel = m_PaintManager.FindControl(kLeftMainPannelControlName);
			CControlUI* hide_left_main_pannel = m_PaintManager.FindControl(kHideLeftMainPannelControlName);
			CControlUI* show_left_main_pannel = m_PaintManager.FindControl(kShowLeftMainPannelControlName);
			if ((left_main_pannel != NULL) && (show_left_main_pannel != NULL) && (hide_left_main_pannel != NULL))
			{
				hide_left_main_pannel->SetVisible(true);
				left_main_pannel->SetVisible(true);
				show_left_main_pannel->SetVisible(false);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSignatureTipsControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CRichEditUI* signature = static_cast<CRichEditUI*>(m_PaintManager.FindControl(kSignatureControlName));
			if (signature != NULL)
			{
				signature->SetText(msg.pSender->GetText());
				signature->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kSearchEditTipControlName) == 0)
		{
			msg.pSender->SetVisible(false);
			CRichEditUI* search_edit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(kSearchEditControlName));
			if (search_edit != NULL)
			{
				search_edit->SetText(msg.pSender->GetText());
				search_edit->SetVisible(true);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kChangeBkSkinControlName) == 0)
		{
			CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
			if (background != NULL)
			{
				TCHAR szBuf[MAX_PATH] = {0};
				++bk_image_index_;
				if (kBackgroundSkinImageCount < bk_image_index_)
					bk_image_index_ = 0;

#if defined(UNDER_WINCE)
				_stprintf(szBuf, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#else
				_stprintf_s(szBuf, MAX_PATH - 1, _T("file='bg%d.png' corner='600,200,1,1'"), bk_image_index_);
#endif
				background->SetBkImage(szBuf);

				SkinChangedParam param;
				CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
				if (background != NULL)
				{
					param.bkcolor = background->GetBkColor();
					if (_tcslen(background->GetBkImage()) > 0)
					{
#if defined(UNDER_WINCE)
						_stprintf(szBuf, _T("bg%d.png"), bk_image_index_);
#else
						_stprintf_s(szBuf, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);
#endif
					}

					param.bgimage = szBuf;
				}
				skin_changed_observer_.Broadcast(param);
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kChangeColorSkinControlName) == 0)
		{
			CDuiRect rcWindow;
			GetWindowRect(m_hWnd, &rcWindow);
			rcWindow.top = rcWindow.top + msg.pSender->GetPos().bottom;
			new ColorSkinWindow(this, rcWindow);
		}
	}
	else if (_tcsicmp(msg.sType, _T("timer")) == 0)
	{
		return OnTimer(msg);
	}
	else if (_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (_tcsicmp(msg.pSender->GetName(), kFriendButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 0)
			{
				pTabControl->SelectItem(0);
				UpdateFriendsList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kGroupButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 1)
			{
				pTabControl->SelectItem(1);
				UpdateGroupsList();
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), kMicroBlogButtonControlName) == 0)
		{
			if (pTabControl && pTabControl->GetCurSel() != 2)
			{
				pTabControl->SelectItem(2);
				UpdateMicroBlogList();
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemactivate")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (pTabControl != NULL)
		{
			if (pTabControl->GetCurSel() == 0)
			{
				CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(m_PaintManager.FindControl(kFriendsListControlName));
				if ((pFriendsList != NULL) &&  pFriendsList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						CControlUI* background = m_PaintManager.FindControl(kBackgroundControlName);
						if (!pFriendsList->CanExpand(node) && (background != NULL))
						{
							FriendListItemInfo friend_info;

							for (std::vector<FriendListItemInfo>::const_iterator citer = friends_.begin(); citer != friends_.end(); ++citer)
							{
								if (_tcsicmp(citer->id, node->data().value) == 0)
								{
									friend_info = *citer;
									break;
								}
							}
							TCHAR szBuf[MAX_PATH] = {0};
							if (_tcslen(background->GetBkImage()) > 0)
							{
#if defined(UNDER_WINCE)
								_stprintf(szBuf, _T("bg%d.png"), bk_image_index_);
#else
								_stprintf_s(szBuf, MAX_PATH - 1, _T("bg%d.png"), bk_image_index_);
#endif
							}

							ChatDialog* pChatDialog = new ChatDialog(szBuf, background->GetBkColor(), myself_info_, friend_info);
							if( pChatDialog == NULL )
								return;
#if defined(WIN32) && !defined(UNDER_CE)
							pChatDialog->Create(NULL, _T("ChatDialog"), UI_WNDSTYLE_FRAME | WS_POPUP,  NULL, 0, 0, 0, 0);
#else
							pChatDialog->Create(NULL, _T("ChatDialog"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
#endif
							skin_changed_observer_.AddReceiver(pChatDialog);

							pChatDialog->CenterWindow();
							::ShowWindow(*pChatDialog, SW_SHOW);
						}
					}
				}
			}
		}
	}
	else if (_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		CTabLayoutUI* pTabControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(kTabControlName));
		if (pTabControl != NULL)
		{
			if (pTabControl->GetCurSel() == 0)
			{
				CFriendsUI* pFriendsList = static_cast<CFriendsUI*>(m_PaintManager.FindControl(kFriendsListControlName));
				if ((pFriendsList != NULL) &&  pFriendsList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						if (pFriendsList->CanExpand(node))
						{
							pFriendsList->SetChildVisible(node, !node->data().child_visible_);
						}
					}
				}
			}
			else if (pTabControl->GetCurSel() == 1)
			{
				CGroupsUI* pGroupsList = static_cast<CGroupsUI*>(m_PaintManager.FindControl(kGroupsListControlName));
				if ((pGroupsList != NULL) &&  pGroupsList->GetItemIndex(msg.pSender) != -1)
				{
					if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
					{
						Node* node = (Node*)msg.pSender->GetTag();

						if (pGroupsList->CanExpand(node))
						{
							pGroupsList->SetChildVisible(node, !node->data().child_visible_);
						}
					}
				}
			}

		}
	}
}

LRESULT MainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

UILIB_RESOURCETYPE MainFrame::GetResourceType() const
{
	return UILIB_ZIPRESOURCE;
}

LPCTSTR MainFrame::GetResourceID() const
{
	return MAKEINTRESOURCE(101);
}
