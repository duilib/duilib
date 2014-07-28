// UIImagePreview.cpp : 实现文件
//

#include "stdafx.h"
#include "DuiDesigner.h"
#include "UIImagePreview.h"


// CUIImagePreview

IMPLEMENT_DYNAMIC(CUIImagePreview, CStatic)

CUIImagePreview::CUIImagePreview()
{

}

CUIImagePreview::~CUIImagePreview()
{
}


BEGIN_MESSAGE_MAP(CUIImagePreview, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CUIImagePreview 消息处理程序

void CUIImagePreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClient;
	CRgn rgnClip;
	this->GetClientRect(&rectClient);
	rgnClip.CreateRectRgnIndirect(&rectClient);
	pDC->SelectClipRgn(&rgnClip);
	pDC->FillRect(&rectClient, &afxGlobalData.brBtnFace);
	if(!m_imgPreview.IsNull())
		m_imgPreview.BitBlt(pDC->GetSafeHdc(), 0, 0);
}

void CUIImagePreview::SetPreviewImage(LPCTSTR pstrImage)
{
	if(!m_imgPreview.IsNull())
		m_imgPreview.Destroy();

	if(*pstrImage != _T('\0'))
		m_imgPreview.Load(pstrImage);
	this->Invalidate(TRUE);
}