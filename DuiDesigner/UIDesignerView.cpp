// UIDesignerView.cpp : CUIDesignerView 类的实现
//

#include "stdafx.h"
#include "DuiDesigner.h"

#include "UIDesignerDoc.h"
#include "UIDesignerView.h"

#include "PropertyTabLayoutUI.h"
#include "DialogSaveAsName.h"
#include "tinyxml.h"
#include "UIUtil.h"
#include <io.h>
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CUIDesignerView

CLIPFORMAT CUIDesignerView::m_cfUI=(CLIPFORMAT)::RegisterClipboardFormat(_T("UIDesigner Clipboard Data Format"));

IMPLEMENT_DYNCREATE(CUIDesignerView, CScrollView)

BEGIN_MESSAGE_MAP(CUIDesignerView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CUIDesignerView::OnFilePrintPreview)
	ON_COMMAND(ID_FORMEDIT_TEST, &CUIDesignerView::OnFormEditTest)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_TEST, &CUIDesignerView::OnUpdateFormEditTest)
	ON_COMMAND(ID_FORMEDIT_ALIGN_LEFT, &CUIDesignerView::OnAlignLeft)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_LEFT, &CUIDesignerView::OnUpdateAlignLeft)
	ON_COMMAND(ID_FORMEDIT_ALIGN_RIGHT, &CUIDesignerView::OnAlignRight)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_RIGHT, &CUIDesignerView::OnUpdateAlignRight)
	ON_COMMAND(ID_FORMEDIT_ALIGN_TOP, &CUIDesignerView::OnAlignTop)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_TOP, &CUIDesignerView::OnUpdateAlignTop)
	ON_COMMAND(ID_FORMEDIT_ALIGN_BOTTOM, &CUIDesignerView::OnAlignBottom)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_BOTTOM, &CUIDesignerView::OnUpdateAlignBottom)
	ON_COMMAND(ID_FORMEDIT_ALIGN_CENTER_VERTICALLY, &CUIDesignerView::OnAlignCenterVertically)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_CENTER_VERTICALLY, &CUIDesignerView::OnUpdateAlignCenterVertically)
	ON_COMMAND(ID_FORMEDIT_ALIGN_CENTER_HORIZONTALLY, &CUIDesignerView::OnAlignCenterHorizontally)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_CENTER_HORIZONTALLY, &CUIDesignerView::OnUpdateAlignCenterHorizontally)
	ON_COMMAND(ID_FORMEDIT_ALIGN_HORIZONTAL, &CUIDesignerView::OnAlignHorizontal)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_HORIZONTAL, &CUIDesignerView::OnUpdateAlignHorizontal)
	ON_COMMAND(ID_FORMEDIT_ALIGN_VERTICAL, &CUIDesignerView::OnAlignVertical)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_VERTICAL, &CUIDesignerView::OnUpdateAlignVertical)
	ON_COMMAND(ID_FORMEDIT_ALIGN_SAME_WIDTH, &CUIDesignerView::OnAlignSameWidth)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_SAME_WIDTH, &CUIDesignerView::OnUpdateAlignSameWidth)
	ON_COMMAND(ID_FORMEDIT_ALIGN_SAME_HEIGHT, &CUIDesignerView::OnAlignSameHeight)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_SAME_HEIGHT, &CUIDesignerView::OnUpdateAlignSameHeight)
	ON_COMMAND(ID_FORMEDIT_ALIGN_SAME_SIZE, &CUIDesignerView::OnAlignSameSize)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_ALIGN_SAME_SIZE, &CUIDesignerView::OnUpdateAlignSameSize)
	ON_COMMAND(ID_FORMEDIT_SHOW_GRID, &CUIDesignerView::OnShowGrid)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_SHOW_GRID, &CUIDesignerView::OnUpdateShowGrid)
	ON_COMMAND(ID_FORMEDIT_SHOW_AUXBORDER, &CUIDesignerView::OnShowAuxBorder)
	ON_UPDATE_COMMAND_UI(ID_FORMEDIT_SHOW_AUXBORDER, &CUIDesignerView::OnUpdateShowAuxBorder)
	ON_COMMAND(ID_EDIT_CUT, &CUIDesignerView::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CUIDesignerView::OnUpdateNeedSel)
	ON_COMMAND(ID_EDIT_COPY, &CUIDesignerView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CUIDesignerView::OnUpdateNeedSel)
	ON_COMMAND(ID_EDIT_PASTE, &CUIDesignerView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CUIDesignerView::OnUpdateNeedClip)
	ON_COMMAND(ID_EDIT_DELETE, &CUIDesignerView::OnDeleteUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CUIDesignerView::OnUpdateNeedSel)
	ON_COMMAND(ID_EDIT_UNDO, &CUIDesignerView::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CUIDesignerView::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CUIDesignerView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CUIDesignerView::OnUpdateEditRedo)
	ON_COMMAND(ID_TEMPLATE_SAVE_AS, &CUIDesignerView::OnTemplateSaveAs)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CUIDesignerView 构造/析构

CUIDesignerView::CUIDesignerView()
{
	// TODO: 在此处添加构造代码
	m_ptDPtoLP.x=0;
	m_ptDPtoLP.y=0;
	m_bInit=false;
}

CUIDesignerView::~CUIDesignerView()
{
	m_LayoutManager.ReleaseExtendedAttrib(m_LayoutManager.GetForm(), m_LayoutManager.GetManager());
}

BOOL CUIDesignerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CUIDesignerView 绘制

void CUIDesignerView::OnDraw(CDC* pDrawDC)
{
	CUIDesignerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	CMemDC memDC(*pDrawDC, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClient;
	GetClientRect(rectClient);
	CPoint point=GetScrollPosition();
	rectClient.OffsetRect(point);
	pDC->FillSolidRect(rectClient,RGB(255, 255, 255));

	CSize szForm=m_LayoutManager.GetForm()->GetInitSize();
	CSize szFormOffset(FORM_OFFSET_X,FORM_OFFSET_Y);
	CDC hCloneDC;
	HBITMAP hNewBitmap;
	hCloneDC.CreateCompatibleDC(pDC);
	hNewBitmap=::CreateCompatibleBitmap(pDC->GetSafeHdc(),szForm.cx,szForm.cy);
	HBITMAP hOldBitmap=(HBITMAP)hCloneDC.SelectObject(hNewBitmap);

	if (m_brHatch.m_hObject){
		CDC cloneDC;
		cloneDC.Attach(hCloneDC);
		CRect rcTemp = rectClient;
		rcTemp.top= rcTemp.top>=20 ? rcTemp.top-20 : 0;
		rcTemp.left= rcTemp.left>=20 ? rcTemp.left-20 : 0;
		//rcTemp.right  = max(rcTemp.right , m_pScrollHelper->GetDisplaySize().cx);
		//rcTemp.bottom = max(rcTemp.bottom, m_pScrollHelper->GetDisplaySize().cy);
		m_brHatch.UnrealizeObject();
		CPoint pt(0, 0);
		cloneDC.LPtoDP(&pt);
		pt = cloneDC.SetBrushOrg(pt.x % 8, pt.y % 8);
		CBrush* old = cloneDC.SelectObject(&m_brHatch);
		cloneDC.FillRect(&rcTemp, &m_brHatch);
		cloneDC.SelectObject(old);
		cloneDC.Detach();
	}

	m_LayoutManager.Draw(&hCloneDC);
	pDC->BitBlt(szFormOffset.cx,szFormOffset.cy,szForm.cx,szForm.cy,&hCloneDC,0,0,SRCCOPY);
	hCloneDC.SelectObject(hOldBitmap);
	::DeleteDC(hCloneDC);
	::DeleteObject(hNewBitmap);

	m_MultiTracker.Draw(pDC,&szFormOffset);
}


// CUIDesignerView 打印
void CUIDesignerView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CUIDesignerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CUIDesignerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CUIDesignerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CUIDesignerView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUIDesignerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CUIDesignerView 诊断

#ifdef _DEBUG
void CUIDesignerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CUIDesignerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CUIDesignerDoc* CUIDesignerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUIDesignerDoc)));
	return (CUIDesignerDoc*)m_pDocument;
}
#endif //_DEBUG


// CUIDesignerView 消息处理程序

void CUIDesignerView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	m_brHatch.CreateHatchBrush(HS_DIAGCROSS, RGB(191, 191, 191));

	// TODO: 在此添加专用代码和/或调用基类
	CUIDesignerDoc* pDoc=GetDocument();

	m_LayoutManager.Init(this->GetSafeHwnd(),pDoc->GetPathName());
	CWindowUI* pForm=m_LayoutManager.GetForm();

	g_pClassView->InsertUITreeItem(pForm,pDoc->GetTitle());
	g_pResourceView->InsertImageTree(pDoc->GetTitle(), pDoc->GetPathName());
	if(pForm->GetCount()>0)
	{
		InitUI(pForm->GetItemAt(0), 1);
		m_LayoutManager.GetManager()->InitControls(pForm->GetItemAt(0));
	}
	g_pClassView->SelectUITreeItem(pForm);
	m_bInit=true;

	m_MultiTracker.SetHandleSize(TRACKER_HANDLE_SIZE);
	m_MultiTracker.SetStyle(dottedLine|resizeOutside);
	m_MultiTracker.SetFormSize(pForm->GetInitSize());
	m_MultiTracker.Add(CreateTracker(pForm));

	SetScrollSizes(MM_TEXT,CSize(FORM_INIT_WIDTH+80,FORM_INIT_HEIGHT+80));
}

void CUIDesignerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CClientDC dc(this);
	OnPrepareDC(&dc);//Device coordinates to Logical coordinates
	dc.SetWindowOrg(-FORM_OFFSET_X,-FORM_OFFSET_Y);//Logical coordinates to Form coordinates

	CPoint ptLogical=point-m_ptDPtoLP;//Device coordinates to Logical coordinates
	ptLogical.Offset(-FORM_OFFSET_X,-FORM_OFFSET_Y);//Logical coordinates to Form coordinates

	CControlUI* pControl=m_LayoutManager.FindControl(ptLogical);
	CTrackerElement* pTracker=NULL;
	if(pControl==NULL)
		pControl=m_LayoutManager.GetForm();

	int nHit=m_MultiTracker.HitTest(ptLogical);
	int nType=GetControlType(pControl);
	if((nFlags&MK_CONTROL)==0&&nHit==hitNothing)
		m_MultiTracker.RemoveAll();
	if(nHit==hitNothing)
		m_MultiTracker.Add(CreateTracker(pControl));
	else
		m_MultiTracker.SetFocus(ptLogical);

	if(nHit>=0||nType==typeControl)
	{
		m_MultiTracker.Track(this, ptLogical, FALSE,&dc);
	}
	else
	{
		CUITracker tracker;
		int nClass=g_pToolBoxWnd->GetCurSel()->GetClass();
		CRect rect;
		if (tracker.TrackRubberBand(this, point, TRUE))
		{
			rect=tracker.GetRect();
			rect.NormalizeRect();
			rect.OffsetRect(-FORM_OFFSET_X,-FORM_OFFSET_Y);
			if(rect.Width()<10&&rect.Height()<10)
				rect.SetRect(ptLogical.x,ptLogical.y,ptLogical.x+UI_DEFAULT_WIDTH,ptLogical.y+UI_DEFAULT_HEIGHT);
		}
		else
		{
			rect.SetRect(ptLogical.x,ptLogical.y,ptLogical.x+UI_DEFAULT_WIDTH,ptLogical.y+UI_DEFAULT_HEIGHT);
		}

		if(nClass>classPointer)
		{
			CControlUI* pNewControl=m_LayoutManager.NewUI(nClass, rect, pControl, &m_LayoutManager);
			CArray<CControlUI*,CControlUI*> arrSelected;
			arrSelected.Add(pNewControl);
			m_UICommandHistory.Begin(arrSelected, actionAdd);
			m_UICommandHistory.End();
			g_pClassView->InsertUITreeItem(pNewControl);
			g_pToolBoxWnd->SetCurSel(classPointer);

			m_MultiTracker.RemoveAll();
			m_MultiTracker.Add(CreateTracker(pNewControl));
			this->GetDocument()->SetModifiedFlag();
		}
	}

	g_pClassView->SelectUITreeItem(m_MultiTracker.GetFocused());

	if(m_MultiTracker.GetSize()==1)
		g_pPropertiesWnd->ShowProperty(m_MultiTracker.GetFocused());
	else
		g_pPropertiesWnd->ShowProperty(NULL);
	
	this->Invalidate(FALSE);

// 	__super::OnLButtonDown(nFlags, point);
}

BOOL CUIDesignerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPoint point;
	GetCursorPos(&point);
	this->ScreenToClient(&point);
	point-=m_ptDPtoLP;
	point.Offset(-FORM_OFFSET_X,-FORM_OFFSET_Y);

	if (pWnd==this&&m_MultiTracker.SetCursor(point,nHitTest))
		return TRUE;

	return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CUIDesignerView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_bInit==false)
		return;

	SIZE size=m_LayoutManager.GetForm()->GetInitSize();
	SetScrollSizes(MM_TEXT,CSize(size.cx+FORM_OFFSET_X+80,
		size.cy+FORM_OFFSET_Y+80));

	UpDateDPtoLPOffset();
}

void CUIDesignerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpDateDPtoLPOffset();
	this->Invalidate(FALSE);
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CUIDesignerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpDateDPtoLPOffset();
	this->Invalidate(FALSE);
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}

CTrackerElement* CUIDesignerView::CreateTracker(CControlUI* pControl)
{
	CTrackerElement* pTracker=NULL;
	int nType=GetControlType(pControl);
	if(nType==typeForm)
		pTracker=new CTrackerElement(pControl,typeForm,this);
	else if(nType==typeContainer)
		pTracker=new CTrackerElement(pControl,typeContainer,this);
	else
		pTracker=new CTrackerElement(pControl,typeControl,this);

	return pTracker;
}

int CUIDesignerView::GetControlType(CControlUI* pControl)
{
	if(pControl->GetInterface(_T("Form")))
		return typeForm;
	else if(pControl->GetInterface(_T("Container")))
		return typeContainer;

	return typeControl;
}

void CUIDesignerView::Notify(TNotifyUI& msg)
{
	CControlUI* pControl=msg.pSender;

	if(msg.sType == _T("PropertyBeginChanged"))
	{
		UIAttribute* pAttrib = (UIAttribute*)msg.lParam;
		m_UICommandHistory.Begin(pControl, pAttrib->pstrName, pAttrib->pstrValue);
	}
	else if(msg.sType == _T("PropertyEndChanged"))
	{
		UIAttribute* pAttrib = (UIAttribute*)msg.lParam;
		m_UICommandHistory.End(pControl, pAttrib->pstrName, pAttrib->pstrValue);
	}
	else if(msg.sType == _T("PosBeginChanged"))
		m_UICommandHistory.Begin(*(CArray<CControlUI*,CControlUI*>*)msg.lParam, actionModify);
	else if(msg.sType == _T("PosEndChanged"))
		m_UICommandHistory.End();
	else if(msg.sType==_T("setpos"))
	{
		if(GetControlType(pControl)==typeForm)
		{
			CWindowUI* pForm=m_LayoutManager.GetForm();
			SIZE size;
			size.cx=pForm->GetFixedWidth();
			size.cy=pForm->GetFixedHeight();
			pForm->SetInitSize(size.cx,size.cy);

			g_pPropertiesWnd->SetPropValue(pControl,tagWindowSize);
			SetScrollSizes(MM_TEXT,CSize(size.cx+FORM_OFFSET_X+80,
				size.cy+FORM_OFFSET_Y+80));
			m_MultiTracker.SetFormSize(size);
		}
		else
		{
			CControlUI* pParent=pControl->GetParent();
			if(pParent)
				pParent->SetPos(pParent->GetPos());

			g_pPropertiesWnd->SetPropValue(pControl,tagPos);
			g_pPropertiesWnd->SetPropValue(pControl,tagSize);
		}
	}
	else if(msg.sType==_T("formsize"))
	{
		if(GetControlType(pControl)!=typeForm)
			return;
		CWindowUI* pForm=m_LayoutManager.GetForm();
		SIZE size=pForm->GetInitSize();

		pForm->SetFixedWidth(size.cx);
		pForm->SetFixedHeight(size.cy);

		SetScrollSizes(MM_TEXT,CSize(size.cx+FORM_OFFSET_X+80,
			size.cy+FORM_OFFSET_Y+80));
		m_MultiTracker.SetFormSize(size);
	}

	this->GetDocument()->SetModifiedFlag();
}

void CUIDesignerView::UpDateDPtoLPOffset()
{
	if(m_nMapMode==0)
		return;

	CPoint point(0,0);
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.LPtoDP(&point);
	m_ptDPtoLP=point;
}
BOOL CUIDesignerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}

void CUIDesignerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpDateDPtoLPOffset();

	__super::OnMouseMove(nFlags, point);
}

void CUIDesignerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_RETURN:
		OnShowPropertyDialog();
		break;
	case VK_DELETE:
		OnDeleteUI();
		break;
	case VK_UP:
		OnMicoMoveUp();
		break;
	case VK_DOWN:
		OnMicoMoveDown();
		break;
	case VK_LEFT:
		OnMicoMoveLeft();
		break;
	case VK_RIGHT:
		OnMicoMoveRight();
		break;
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CUIDesignerView::OnDeleteUI()
{
	CArray<CControlUI*,CControlUI*> arrSelected;
	m_MultiTracker.GetSelected(arrSelected);
	RemoveForm(arrSelected);
	if(arrSelected.IsEmpty())
		return;

	m_UICommandHistory.Begin(arrSelected, actionDelete);
	for(int i=0; i<arrSelected.GetSize(); i++)
	{
		CControlUI* pControl = arrSelected[i];
		CControlUI* pParent=pControl->GetParent();
		HTREEITEM hDelete=(HTREEITEM)(((ExtendedAttributes*)pControl->GetTag())->hItem);
		g_pClassView->RemoveUITreeItem(hDelete);
		DeleteUI(pControl);
		if(pParent)
			pParent->NeedUpdate();
	}
	m_UICommandHistory.End();

	g_pPropertiesWnd->ShowProperty(NULL);
	m_MultiTracker.RemoveAll();
	this->GetDocument()->SetModifiedFlag();
}

CControlUI* CUIDesignerView::NewUI(int nClass,CRect& rect,CControlUI* pParent)
{
	CControlUI* pControl = m_LayoutManager.NewUI(nClass, rect, pParent, &m_LayoutManager);
	if(pControl != NULL)
		this->GetDocument()->SetModifiedFlag();

	return pControl;
}

BOOL CUIDesignerView::DeleteUI(CControlUI* pControl)
{
	m_MultiTracker.RemoveAll();
	BOOL bRet = m_LayoutManager.DeleteUI(pControl);
	if(bRet)
		this->GetDocument()->SetModifiedFlag();

	return bRet;
}

void CUIDesignerView::SelectUI(CControlUI* pControl)
{
	if(pControl==NULL||pControl==m_MultiTracker.GetFocused())
		return;

	CControlUI* pParent=pControl;
	CControlUI* pForm=pControl;
	while(pParent=pParent->GetParent())
		pForm=pParent;
	if(pForm!=m_LayoutManager.GetForm())
		return;

	m_MultiTracker.RemoveAll();
	if(pControl->IsVisible())
		m_MultiTracker.Add(CreateTracker(pControl));
	g_pPropertiesWnd->ShowProperty(pControl);

	this->Invalidate(FALSE);
}

void CUIDesignerView::OnActivated()
{
	g_pPropertiesWnd->ShowProperty(m_MultiTracker.GetFocused());
	g_HookAPI.SetSkinDir(m_LayoutManager.GetSkinDir());
}

void CUIDesignerView::InitUI(CControlUI* pControl, int depth)
{
	if(pControl==NULL)
		return;

	ExtendedAttributes* pExtended=new ExtendedAttributes;
	ZeroMemory(pExtended,sizeof(ExtendedAttributes));
	pControl->SetTag((UINT_PTR)pExtended);

	pExtended->nClass=gGetUIClass(pControl);
	m_LayoutManager.SetDefaultUIName(pControl);
	g_pClassView->InsertUITreeItem(pControl);
	pExtended->nDepth = depth;

	CContainerUI* pContainer=static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	if(pContainer==NULL)
		return;
	pContainer->SetDelayedDestroy(false);
	for (int i=0;i<pContainer->GetCount();i++)
	{
		InitUI(pContainer->GetItemAt(i), ++depth);
	}
}

void CUIDesignerView::OnFormEditTest()
{
	TCHAR szFileName[MAX_PATH];
	CString strFilePath=GetDocument()->GetPathName();
	strFilePath=strFilePath.Mid(0,strFilePath.ReverseFind(_T('\\'))+1);
	// 在原XML文件目录中创建临时文件用于预览
	::GetTempFileName(strFilePath, _T("Dui"), 0, szFileName);

	this->SaveSkinFile(szFileName);
	
	m_LayoutManager.TestForm(szFileName);
}

void CUIDesignerView::OnAlignLeft()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignLeft(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnAlignRight()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignRight(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnAlignTop()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignTop(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnAlignBottom()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignBottom(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnAlignCenterVertically()
{
	CArray<CControlUI*,CControlUI*> arrSelected;
	m_MultiTracker.GetSelected(arrSelected);
	RemoveForm(arrSelected);
	if(arrSelected.IsEmpty())
		return;

	m_UICommandHistory.Begin(arrSelected, actionModify);
	m_LayoutManager.AlignCenterVertically(m_MultiTracker.GetFocused(),arrSelected);
	m_UICommandHistory.End();
	this->GetDocument()->SetModifiedFlag();
}

void CUIDesignerView::OnAlignCenterHorizontally()
{
	CArray<CControlUI*,CControlUI*> arrSelected;
	m_MultiTracker.GetSelected(arrSelected);
	RemoveForm(arrSelected);
	if(arrSelected.IsEmpty())
		return;

	m_UICommandHistory.Begin(arrSelected, actionModify);
	m_LayoutManager.AlignCenterHorizontally(m_MultiTracker.GetFocused(),arrSelected);
	m_UICommandHistory.End();
	this->GetDocument()->SetModifiedFlag();
}

void CUIDesignerView::OnAlignHorizontal()
{
	CArray<CControlUI*,CControlUI*> arrSelected;
	m_MultiTracker.GetSelected(arrSelected);
	RemoveForm(arrSelected);
	if(arrSelected.IsEmpty())
		return;

	m_UICommandHistory.Begin(arrSelected, actionModify);
	m_LayoutManager.AlignHorizontal(m_MultiTracker.GetFocused(),arrSelected);
	m_UICommandHistory.End();
	this->GetDocument()->SetModifiedFlag();

}

void CUIDesignerView::OnAlignVertical()
{
	CArray<CControlUI*,CControlUI*> arrSelected;
	m_MultiTracker.GetSelected(arrSelected);
	RemoveForm(arrSelected);
	if(arrSelected.IsEmpty())
		return;

	m_UICommandHistory.Begin(arrSelected, actionModify);
	m_LayoutManager.AlignVertical(m_MultiTracker.GetFocused(),arrSelected);
	m_UICommandHistory.End();
	this->GetDocument()->SetModifiedFlag();
}

void CUIDesignerView::OnAlignSameWidth()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignSameWidth(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnAlignSameHeight()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignSameHeight(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnAlignSameSize()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.AlignSameSize(m_MultiTracker.GetFocused(),arrSelected);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnShowGrid()
{
	m_LayoutManager.ShowGrid(!m_LayoutManager.IsShowGrid());

	m_LayoutManager.GetForm()->NeedUpdate();
}

void CUIDesignerView::OnShowAuxBorder()
{
	m_LayoutManager.ShowAuxBorder(!m_LayoutManager.IsShowAuxBorder());

	m_LayoutManager.GetForm()->NeedUpdate();
}

void CUIDesignerView::OnUpdateFormEditTest(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_LayoutManager.IsEmptyForm());
}

void CUIDesignerView::OnUpdateAlignLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignTop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignBottom(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignCenterVertically(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_MultiTracker.IsEmpty());
}

void CUIDesignerView::OnUpdateAlignCenterHorizontally(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_MultiTracker.IsEmpty());
}

void CUIDesignerView::OnUpdateAlignHorizontal(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>2?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignVertical(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>2?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignSameWidth(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignSameHeight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateAlignSameSize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_MultiTracker.GetSize()>1?TRUE:FALSE);
}

void CUIDesignerView::OnUpdateShowGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_LayoutManager.IsShowGrid());
}

void CUIDesignerView::OnUpdateShowAuxBorder(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_LayoutManager.IsShowAuxBorder());
}

void CUIDesignerView::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	g_pClassView->RemoveUITreeItem(m_LayoutManager.GetForm());
	g_pResourceView->RemoveImageTree(this->GetDocument()->GetTitle());
	g_pPropertiesWnd->ShowProperty(NULL);
}

void CUIDesignerView::OnEditCut()
{
	OnEditCopy();
	OnDeleteUI();
}

void CUIDesignerView::OnEditCopy()
{
	ASSERT(m_cfUI != NULL);

	TiXmlDocument xmlDoc;
	TiXmlDeclaration Declaration("1.0","utf-8","yes");
	xmlDoc.InsertEndChild(Declaration);
	TiXmlElement* pCopyElm = new TiXmlElement("UICopy");
	CopyUI(pCopyElm);
	xmlDoc.InsertEndChild(*pCopyElm);
	TiXmlPrinter printer;
	xmlDoc.Accept(&printer);
	delete pCopyElm;
	CSharedFile file(GMEM_MOVEABLE, printer.Size() + 1);
	file.Write(printer.CStr(), printer.Size());
	file.Write("\0", 1);
	COleDataSource* pDataSource = NULL;
	TRY
	{
		pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData(m_cfUI, file.Detach());
		pDataSource->SetClipboard();
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CUIDesignerView::CopyUI(TiXmlElement* pParentNode)
{
	ASSERT(pParentNode);
	CArray<CControlUI*,CControlUI*> arrSelected;
	TiXmlElement* pContainerNode = new TiXmlElement("Container");

	m_MultiTracker.GetSelected(arrSelected);

	TiXmlNode* pNode;
	for(int i=0; i<arrSelected.GetSize(); i++)
	{
		m_LayoutManager.SaveProperties(arrSelected[i], pContainerNode);

		if(i == 0)
			pNode = pContainerNode->FirstChild();
		else
			pNode = pNode->NextSibling();
		pNode->ToElement()->RemoveAttribute("name");
	}

	pParentNode->InsertEndChild(*pContainerNode);
	delete pContainerNode;
}

void CUIDesignerView::OnEditPaste()
{
	COleDataObject dataObject;
	dataObject.AttachClipboard();

	if(dataObject.IsDataAvailable(m_cfUI))
	{
		CFile* pFile = dataObject.GetFileData(m_cfUI);
		if (pFile == NULL)
			return;

		int len = (int)pFile->GetLength();
		char* pstrXML  = new char[len];
		pFile->Read(pstrXML, len);

		PasteUI(StringConvertor::Utf8ToWide(pstrXML));

		delete[] pstrXML;
		delete pFile;
	}
}

void CUIDesignerView::PasteUI(LPCTSTR xml)
{
	CDialogBuilder builder;
	CControlUI* pRoot=builder.Create(xml, (UINT)0, NULL, m_LayoutManager.GetManager());
	if(pRoot)
	{
		CControlUI* pParent = m_MultiTracker.GetFocused();
		if(pParent->GetInterface(_T("Container")) == NULL)
			pParent = pParent->GetParent();
		if(pParent == NULL)
			pParent = m_LayoutManager.GetForm();

		m_MultiTracker.RemoveAll();
		CContainerUI* pContainer = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
		CContainerUI* pRootContainer = static_cast<CContainerUI*>(pRoot->GetInterface(_T("Container")));
		ExtendedAttributes* pExtended = (ExtendedAttributes*)pContainer->GetTag();
		for(int i=0; i<pRootContainer->GetCount(); i++)
		{
			CControlUI* pControl = pRootContainer->GetItemAt(i);
			if(pControl->IsFloat())
			{
				SIZE sz = pControl->GetFixedXY();
				sz.cx += COPY_OFFSET_XY;
				sz.cy += COPY_OFFSET_XY;
				pControl->SetFixedXY(sz);
			}
			pContainer->Add(pControl);
			m_MultiTracker.Add(CreateTracker(pControl));
			InitUI(pControl, pExtended->nDepth + 1);
		}
		CArray<CControlUI*,CControlUI*> arrSelected;
		m_MultiTracker.GetSelected(arrSelected);
		m_UICommandHistory.Begin(arrSelected, actionAdd);
		m_UICommandHistory.End();

		pContainer->SetPos(pContainer->GetPos());

		pRootContainer->SetAutoDestroy(false);
		delete pRootContainer;
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnEditUndo()
{
	m_UICommandHistory.Undo();
}

void CUIDesignerView::OnEditRedo()
{
	m_UICommandHistory.Redo();
}

void CUIDesignerView::OnUpdateNeedSel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_MultiTracker.IsEmpty());
}

void CUIDesignerView::OnUpdateNeedClip(CCmdUI* pCmdUI)
{
	BOOL bOn = FALSE;
	if(CountClipboardFormats()!=0 && IsClipboardFormatAvailable(m_cfUI))
		bOn = TRUE;

	pCmdUI->Enable(bOn);
}

void CUIDesignerView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_UICommandHistory.CanUndo());
}

void CUIDesignerView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_UICommandHistory.CanRedo());
}

void CUIDesignerView::OnMicoMoveUp()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.MicoMoveUp(arrSelected,MICO_MOVE_SPACE);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnMicoMoveDown()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.MicoMoveDown(arrSelected,MICO_MOVE_SPACE);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnMicoMoveLeft()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.MicoMoveLeft(arrSelected,MICO_MOVE_SPACE);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnMicoMoveRight()
{
	CArray<CControlUI*,CControlUI*> arrSelected;

	if(m_MultiTracker.GetSelected(arrSelected))
	{
		m_UICommandHistory.Begin(arrSelected, actionModify);
		m_LayoutManager.MicoMoveRight(arrSelected,MICO_MOVE_SPACE);
		m_UICommandHistory.End();
		this->GetDocument()->SetModifiedFlag();
	}
}

void CUIDesignerView::OnShowPropertyDialog()
{
	if(m_MultiTracker.GetSize()>1)
		return;

	ShowPropertyDialog(m_MultiTracker.GetFocused());
}

void CUIDesignerView::ShowPropertyDialog(CControlUI* pControl)
{
	if(pControl==NULL)
		return;

	CDialog* pPropDlg=NULL;
	ExtendedAttributes* pExtended=(ExtendedAttributes*)pControl->GetTag();
	switch(pExtended->nClass)
	{
	case classWindow:
		break;
	case classControl:
		break;
	case classLabel:
	case classText:
		break;
	case classButton:
		break;
	case classEdit:
		break;
	case classOption:
		break;
	case classProgress:
		break;
	case classSlider:
		break;
	case classCombo: 
		break;
	case classActiveX:
		break;
	case classContainer:
		break;
	case classVerticalLayout:
		break;
	case classTabLayout:
		pPropDlg = new CPropertyTabLayoutUI(pControl);
		break;
	case classHorizontalLayout:
		break;
	case classTileLayout:
		break;
	case classChildLayout:
		break;
	default:
		return;
	}

	if(pPropDlg)
	{
		pPropDlg->DoModal();
		delete pPropDlg;
	}
}

void CUIDesignerView::SaveSkinFile(LPCTSTR pstrPathName)
{
	if(!m_LayoutManager.SaveSkinFile(pstrPathName))
	{
		MessageBox(_T("保存XML文件失败!"),_T("错误"),MB_OK);
	}
	g_pResourceView->CopyImageToSkinDir(m_LayoutManager.GetSkinDir(), this->GetDocument()->GetTitle());
}

void CUIDesignerView::OnTitleChanged(LPCTSTR pstrOldTitle, LPCTSTR pstrNewTitle)
{
	g_pClassView->RenameUITreeItem(m_LayoutManager.GetForm(), pstrNewTitle);
	g_pResourceView->RenameImageTree(pstrOldTitle, pstrNewTitle);
}

void CUIDesignerView::RedoUI(CControlUI* pControl, CControlUI* pParent)
{
	ASSERT(pControl && pParent);
	if(!pControl || !pParent)
		return;

	CContainerUI* pContainer = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
	ExtendedAttributes* pExtended = (ExtendedAttributes*)pContainer->GetTag();
	pContainer->Add(pControl);
 	pControl->GetManager()->ReapObjects(pControl);
	InitUI(pControl, pExtended->nDepth + 1);
	m_MultiTracker.Add(CreateTracker(pControl));
	pContainer->SetPos(pContainer->GetPos());
}

void CUIDesignerView::OnTemplateSaveAs()
{
	// TODO: 在此添加命令处理程序代码
	CDialogSaveAsName dlg;
	if(dlg.DoModal() != IDOK)
		return;

	CString strTemplateName = dlg.GetSaveAsName();
	CString strTemplatesDir = CGlobalVariable::GetTemplatesDir();
	CreateDirectory(strTemplatesDir + strTemplateName, NULL);
	CString strTemplatePathName = strTemplatesDir + strTemplateName
		+ _T("\\template.xml");
	CString strTemplateImage = strTemplatesDir + strTemplateName
		+ _T("\\template.ui");

	SaveSkinImage(strTemplateImage);
	SaveSkinFile(strTemplatePathName);
}

void CUIDesignerView::OnStyleSaveAs()
{
	CDialogSaveAsName dlg;
	if(dlg.DoModal() != IDOK)
		return;

	CString strStyleName = dlg.GetSaveAsName();
	CString strStylesDir = CGlobalVariable::GetStylesDir();
	CreateDirectory(strStylesDir + strStyleName, NULL);
	CString strStylePathName = strStylesDir + strStyleName
		+ _T("\\style.xml");
	CString strStyleImage = strStylesDir + strStyleName
		+ _T("\\style.ui");

	SaveSkinImage(strStyleImage);
	SaveSkinFile(strStylePathName);
}

BOOL CUIDesignerView::SaveSkinImage(LPCTSTR pstrPathName)
{
	CImage image;
	CWindowUI* pForm = m_LayoutManager.GetForm();
	ASSERT(pForm);

	CSize szForm = pForm->GetInitSize();
	CRect rcPaint(0,0,szForm.cx,szForm.cy);
	image.Create(szForm.cx, szForm.cy, 32);
	pForm->DoPaint(image.GetDC(), rcPaint);
	BOOL bRet = image.Save(pstrPathName, Gdiplus::ImageFormatJPEG);

	image.ReleaseDC();
	return bRet;
}

void CUIDesignerView::ReDrawForm()
{
	m_LayoutManager.GetForm()->NeedUpdate();
}

void CUIDesignerView::RemoveForm(CArray<CControlUI*,CControlUI*>& arrSelected)
{
	for (int i=0; i<arrSelected.GetSize(); i++)
	{
		CControlUI* pControl = arrSelected[i];
		if(pControl == m_LayoutManager.GetForm())
		{
			arrSelected.RemoveAt(i);
			break;
		}
	}
}

void CUIDesignerView::SetModifiedFlag(BOOL bModified/* = TRUE*/)
{
	this->GetDocument()->SetModifiedFlag(bModified);
}
