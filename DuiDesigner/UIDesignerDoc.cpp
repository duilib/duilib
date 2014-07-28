
// UIDesignerDoc.cpp : CUIDesignerDoc 类的实现
//

#include "stdafx.h"
#include "DuiDesigner.h"

#include "UIDesignerDoc.h"
#include "DialogSkinFileNew.h"
#include "UIUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIDesignerDoc

IMPLEMENT_DYNCREATE(CUIDesignerDoc, CDocument)

BEGIN_MESSAGE_MAP(CUIDesignerDoc, CDocument)
	ON_COMMAND(ID_MDITABS_COPY_FULLPATH, &CUIDesignerDoc::OnMdiCopyFullPath)
	ON_COMMAND(ID_MDITABS_OPEN_FULLPATH, &CUIDesignerDoc::OnMdiOpenFullPath)
END_MESSAGE_MAP()


// CUIDesignerDoc 构造/析构

CUIDesignerDoc::CUIDesignerDoc()
{
	// TODO: 在此添加一次性构造代码

}

CUIDesignerDoc::~CUIDesignerDoc()
{
}

BOOL CUIDesignerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	CDialogSkinFileNew dlg;
	dlg.m_strUITitle = this->GetTitle();
	if(dlg.DoModal() == IDOK)
	{
		CString strFilePath = dlg.GetStyleFilePath();
		if(!strFilePath.IsEmpty())
			this->SetPathName(strFilePath);
		if(!dlg.m_strUITitle.IsEmpty())
			this->SetTitle(dlg.m_strUITitle);
	}
	else
		return FALSE;

	return TRUE;
}




// CUIDesignerDoc 序列化

void CUIDesignerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CUIDesignerDoc 诊断

#ifdef _DEBUG
void CUIDesignerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUIDesignerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUIDesignerDoc 命令

BOOL CUIDesignerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类
	POSITION pos = this->GetFirstViewPosition();
	while(pos)
	{
		CView* pView = this->GetNextView(pos);
		CUIDesignerView* pUIView = DYNAMIC_DOWNCAST(CUIDesignerView, pView);
		ASSERT(pUIView);
		pUIView->SaveSkinFile(lpszPathName);
		this->SetModifiedFlag(FALSE);
	}

	return TRUE;/*CDocument::OnSaveDocument(lpszPathName);*/
}

void CUIDesignerDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(m_strTitle != lpszTitle)
	{
		POSITION pos = this->GetFirstViewPosition();
		while(pos)
		{
			CView* pView = this->GetNextView(pos);
			CUIDesignerView* pUIView = DYNAMIC_DOWNCAST(CUIDesignerView, pView);
			ASSERT(pUIView);
			pUIView->OnTitleChanged(m_strTitle, lpszTitle);
		}
	}

	CDocument::SetTitle(lpszTitle);
}

void CUIDesignerDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(*lpszPathName == _T('\0'))
		m_strPathName.Empty();
	else
		CDocument::SetPathName(lpszPathName, bAddToMRU);
}

void CUIDesignerDoc::OnMdiCopyFullPath()
{
	if(m_strPathName.IsEmpty())
	{
		MessageBox(NULL, _T("请先保存当前文件。"), _T("提示"), MB_ICONINFORMATION);
		return;
	}

	HGLOBAL hClip;
	if(OpenClipboard(NULL))
	{
		EmptyClipboard();
		CStringA strFullPath = StringConvertor::WideToAnsi(m_strPathName);
		hClip = GlobalAlloc(GMEM_MOVEABLE, strFullPath.GetLength() + 1);
		char* pbufCopy;
		pbufCopy = (char*)GlobalLock(hClip);
		strcpy(pbufCopy, strFullPath);
		GlobalUnlock(hClip);
		SetClipboardData(CF_TEXT, hClip);
		CloseClipboard();
	}
}

void CUIDesignerDoc::OnMdiOpenFullPath()
{
	if(m_strPathName.IsEmpty())
	{
		MessageBox(NULL, _T("请先保存当前文件。"), _T("提示"), MB_ICONINFORMATION);
		return;
	}

	int nPos = m_strPathName.ReverseFind(_T('\\'));
	if(nPos == -1)
		return;
	CString strDir = m_strPathName.Left(nPos + 1);
	ShellExecute(NULL, _T("open"), strDir, NULL, NULL, SW_SHOW);
}
