// ToolBoxWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "ToolBoxWnd.h"

//////////////////////////////////////////////////////////////////////////
// CToolBoxWnd

IMPLEMENT_DYNAMIC(CToolBoxWnd, CDockablePane)

CToolBoxWnd::CToolBoxWnd()
{
	g_pToolBoxWnd=this;
}

CToolBoxWnd::~CToolBoxWnd()
{
}


BEGIN_MESSAGE_MAP(CToolBoxWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CToolBoxWnd 消息处理程序

int CToolBoxWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (!m_ctlToolList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 12))
	{
		TRACE0("Failed to create ToolBox\n");
		return -1;      // fail to create
	}
	InitToolList();

	return 0;
}

void CToolBoxWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CToolBoxWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ctlToolList.SetWindowPos(NULL, rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CToolBoxWnd::InitToolList()
{
	CToolElement* pControlTab=new CToolElement(_T("控件"));
	pControlTab->AddSubTool(new CToolElement(_T("指针"),classPointer,IDI_TOOLBOX_POINTER));
	pControlTab->AddSubTool(new CToolElement(_T("Control"),classControl,IDI_TOOLBOX_CONTROL));
	pControlTab->AddSubTool(new CToolElement(_T("Label"),classLabel,IDI_TOOLBOX_LABEL));
	pControlTab->AddSubTool(new CToolElement(_T("Text"),classText,IDI_TOOLBOX_TEXT));
	pControlTab->AddSubTool(new CToolElement(_T("Edit"),classEdit,IDI_TOOLBOX_EDIT));
	pControlTab->AddSubTool(new CToolElement(_T("Option"),classOption,IDI_TOOLBOX_OPTION));
	pControlTab->AddSubTool(new CToolElement(_T("Combo"),classCombo,IDI_TOOLBOX_COMBO));
	pControlTab->AddSubTool(new CToolElement(_T("List"),classList,IDI_TOOLBOX_COMBO));
	pControlTab->AddSubTool(new CToolElement(_T("Button"),classButton,IDI_TOOLBOX_BUTTON));
	pControlTab->AddSubTool(new CToolElement(_T("Progress"),classProgress,IDI_TOOLBOX_PROGRESS));
	pControlTab->AddSubTool(new CToolElement(_T("Slider"),classSlider,IDI_TOOLBOX_SLIDER));
	pControlTab->AddSubTool(new CToolElement(_T("ActiveX"),classActiveX,IDI_TOOLBOX_ACTIVE));
	m_ctlToolList.AddToolTab(pControlTab);

	CToolElement* pLayoutTab=new CToolElement(_T("布局"));
	pLayoutTab->AddSubTool(new CToolElement(_T("Container"),classContainer,IDI_TOOLBOX_CONTAINER));
	pLayoutTab->AddSubTool(new CToolElement(_T("VerticalLayout"),classVerticalLayout,IDI_TOOLBOX_VERTICAL_LAYOUT));
	pLayoutTab->AddSubTool(new CToolElement(_T("HorizontalLayout"),classHorizontalLayout,IDI_TOOLBOX_HORIZONTAL_LAYOUT));
	pLayoutTab->AddSubTool(new CToolElement(_T("TileLayout"),classTileLayout,IDI_TOOLBOX_TILE_LAYOUT));
	pLayoutTab->AddSubTool(new CToolElement(_T("TabLayout"),classTabLayout,IDI_TOOLBOX_TAB_LAYOUT));
	m_ctlToolList.AddToolTab(pLayoutTab);

	m_ctlToolList.SetCurSel(classPointer);
}