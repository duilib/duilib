#include "stdafx.h"
#include "PropertiesWnd.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CPropertiesWnd

CPropertiesWnd::CPropertiesWnd()
{
	g_pPropertiesWnd = this;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WM_UI_PROPERTY_CHANGED, OnUIPropChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesWnd 消息处理程序

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	m_wndUIProperties.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, ID_UI_PROP_WND);

	AdjustLayout();

	return 0;
}

void CPropertiesWnd::AdjustLayout()
{
	if(GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndUIProperties.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CPropertiesWnd::SetVSDotNetLook(BOOL bSet)
{
	m_wndUIProperties.SetVSDotNetLook(bSet);
}

void CPropertiesWnd::ShowProperty(CControlUI* pControl)
{
	m_wndUIProperties.ShowProperty(pControl);
}

LRESULT CPropertiesWnd::OnUIPropChanged(WPARAM wp, LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty *)lp;
	SetUIValue(pProp, pProp->GetData());

	return TRUE;
}

void CPropertiesWnd::SetPropValue(CControlUI* pControl,int nTag)
{
	CControlUI* pCurControl = m_wndUIProperties.GetCurUI();
	if(pCurControl != pControl)
		return;

	CMFCPropertyGridProperty* pPropUI=NULL;
	CMFCPropertyGridProperty* pValueList=NULL;
	switch(nTag)
	{
	case tagWindowSize:
		{
			pPropUI = m_wndUIProperties.FindPropByData(classWindow,FALSE);

			RECT rect=pControl->GetPos();
			pValueList=pPropUI->GetSubItem(tagWindowSize-tagWindow);
			pValueList->GetSubItem(0)->SetValue((_variant_t)(LONG)(rect.right-rect.left));
			pValueList->GetSubItem(1)->SetValue((_variant_t)(LONG)(rect.bottom-rect.top));
			pValueList->GetSubItem(0)->SetOriginalValue((_variant_t)(LONG)(rect.right-rect.left));
			pValueList->GetSubItem(1)->SetOriginalValue((_variant_t)(LONG)(rect.bottom-rect.top));

			break;
		}
	case tagPos:
		{
			pPropUI = m_wndUIProperties.FindPropByData(classControl,FALSE);

			SIZE szXY=pControl->GetFixedXY();
			int nWidth=pControl->GetFixedWidth();
			int nHeight=pControl->GetFixedHeight();
			pValueList=pPropUI->GetSubItem(tagPos-tagControl);
			pValueList->GetSubItem(0)->SetValue((_variant_t)(LONG)szXY.cx);
			pValueList->GetSubItem(1)->SetValue((_variant_t)(LONG)szXY.cy);
			pValueList->GetSubItem(2)->SetValue((_variant_t)(LONG)(szXY.cx+nWidth));
			pValueList->GetSubItem(3)->SetValue((_variant_t)(LONG)(szXY.cy+nHeight));
			pValueList->GetSubItem(0)->SetOriginalValue((_variant_t)(LONG)szXY.cx);
			pValueList->GetSubItem(1)->SetOriginalValue((_variant_t)(LONG)szXY.cy);
			pValueList->GetSubItem(2)->SetOriginalValue((_variant_t)(LONG)(szXY.cx+nWidth));
			pValueList->GetSubItem(3)->SetOriginalValue((_variant_t)(LONG)(szXY.cy+nHeight));

			break;
		}
	case tagSize:
		{
			pPropUI = m_wndUIProperties.FindPropByData(classControl,FALSE);

			pValueList=pPropUI->GetSubItem(tagSize-tagControl);
			pValueList->GetSubItem(0)->SetValue((_variant_t)(LONG)pControl->GetWidth());
			pValueList->GetSubItem(1)->SetValue((_variant_t)(LONG)pControl->GetHeight());
			pValueList->GetSubItem(0)->SetOriginalValue((_variant_t)(LONG)pControl->GetWidth());
			pValueList->GetSubItem(1)->SetOriginalValue((_variant_t)(LONG)pControl->GetHeight());

			break;
		}
	}
}

void CPropertiesWnd::SetUIValue(CMFCPropertyGridProperty* pProp,int nTag)
{
	CControlUI* pControl = m_wndUIProperties.GetCurUI();
	if(pControl == NULL)
		return;

	int nUpdate=UPDATE_REDRAW_CONTROL;
	CString strName=pProp->GetName();
	strName.MakeLower();
	CString strNewVal;
	CString strOldVal;
	strNewVal = pProp->FormatProperty();
	strOldVal = m_wndUIProperties.FormatOrigProperty(pProp);
	if(nTag!=tagName && nTag!=tagText && strName.Find(_T("image"))==-1)
	{
		strNewVal.MakeLower();
		strOldVal.MakeLower();
	}

	switch(nTag)
	{
	case tagName:
		{
			CPaintManagerUI* pManager = g_pMainFrame->GetActiveUIView()->GetPaintManager();
			if(strNewVal.IsEmpty() || pManager->FindControl(strNewVal))
			{
				if(!strNewVal.IsEmpty())
					MessageBox(strNewVal + _T(" 名称已被其他控件使用！"));
				pProp->SetValue((_variant_t)pControl->GetName());
				return;
			}
			else
				pManager->ReapObjects(pControl);

			g_pClassView->RenameUITreeItem(pControl, strNewVal);
			break;
		}
	case tagPos:
	case tagPadding:
		SetPropValue(pControl,tagSize);

		nUpdate=UPDATE_POS;
		break;
	case tagSize:
	case tagMinSize:
	case tagMaxSize:
	case tagMinMax:
		SetUIValue(pProp->GetSubItem(0),-1);
		SetUIValue(pProp->GetSubItem(1),-1);
		SetPropValue(pControl,tagPos);

		nUpdate=UPDATE_POS;
		break;
	case tagFloat:
	case tagColumns:
		nUpdate=UPDATE_POS;
		break;
	case tagVisible:
		nUpdate=UPDATE_REDRAW_PARENT;
		break;
	case tagListHeader:
		if (strNewVal.Compare(_T("false"))==0)
		{
			strNewVal = _T("hidden");
		}
		break;
	case tagAlpha:
		{
			int alpha=_ttoi(strNewVal);
			if (alpha<0)
			{
				strNewVal=_T("0");
				pProp->SetValue((_variant_t)(LONG)0);
			}
			else if (alpha<=255) 
			{
				pProp->SetValue((_variant_t)(LONG)alpha);
			}
			else
			{
				strNewVal=_T("255");
				pProp->SetValue((_variant_t)(LONG)255);
			}
			break;
		}
	}

	CUIDesignerView* pUIView=g_pMainFrame->GetActiveUIView();
	ASSERT(pUIView);
	TNotifyUI Msg;
	UIAttribute oldAttrib = {strName, strOldVal};
	Msg.pSender=pControl;
	Msg.sType=_T("PropertyBeginChanged");
	Msg.wParam=0;
	Msg.lParam=(LPARAM)&oldAttrib;
	pUIView->Notify(Msg);

	pControl->SetAttribute(strName,strNewVal);
	pProp->SetOriginalValue(pProp->GetValue());

	UIAttribute newAttrib = {strName, strNewVal};
	Msg.sType = _T("PropertyEndChanged");
	Msg.lParam = (LPARAM)&newAttrib;
	pUIView->Notify(Msg);

	if(nTag==tagWindowSize)
	{
		Msg.sType=_T("formsize");
		Msg.lParam = NULL;
		pUIView->Notify(Msg);
	}
	else if(nTag == tagName)
	{
		CPaintManagerUI* pManager = pUIView->GetPaintManager();
		pManager->InitControls(pControl);
	}

	CControlUI* pParent=pControl->GetParent();
	if(pParent==NULL)
		pParent=pControl;
	switch(nUpdate)
	{
	case UPDATE_POS:
		pParent->SetPos(pParent->GetPos());

		break;
	case UPDATE_REDRAW_CONTROL:
		pControl->NeedUpdate();

		break;
	case UPDATE_REDRAW_PARENT:
		pParent->NeedUpdate();
	}
}
