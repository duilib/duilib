#ifndef UIGROUPS_HPP
#define UIGROUPS_HPP

#include "UIListCommonDefine.hpp"

namespace DuiLib
{
struct GroupsListItemInfo
{
	bool folder;
	bool empty;
	CDuiString id;
	CDuiString logo;
	CDuiString nick_name;
	CDuiString description;
};

class CGroupsUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CGroupsUI(CPaintManagerUI& paint_manager);

	~CGroupsUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl, bool bDoNotDestroy=false);

	bool RemoveAt(int iIndex, bool bDoNotDestroy=false);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(const GroupsListItemInfo& item, Node* parent = NULL);

	bool RemoveNode(Node* node);

	void SetChildVisible(Node* node, bool visible);

	bool CanExpand(Node* node) const;

private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CDuiRect	text_padding_;
	int level_text_start_pos_;
	CDuiString level_expand_image_;
	CDuiString level_collapse_image_;
	CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIGROUPS_HPP