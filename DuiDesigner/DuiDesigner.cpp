// UIDesigner.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "DuiDesigner.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "UIDesignerDoc.h"
#include "UIDesignerView.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIDesignerApp

BEGIN_MESSAGE_MAP(CUIDesignerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CUIDesignerApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CUIDesignerApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CUIDesignerApp 构造

CUIDesignerApp::CUIDesignerApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_pUIDocTemplate = NULL;
}

// 唯一的一个 CUIDesignerApp 对象

CUIDesignerApp theApp;


// CUIDesignerApp 初始化

BOOL CUIDesignerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T(""));
	LoadStdProfileSettings();  // 加载标准 INI 文件选项(包括 MRU)
	SetRegistryBase(_T("Settings"));
	CPaintManagerUI::LoadPlugin(_T("mgyUI_Plugin.dll"));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接

	m_pUIDocTemplate = new CMultiDocTemplate(IDR_UIDESIGNER,
		RUNTIME_CLASS(CUIDesignerDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CUIDesignerView));
	if (!m_pUIDocTemplate)
		return FALSE;
	AddDocTemplate(m_pUIDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->SetWindowText(_T("Dui Designer"));
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CMFCLinkCtrl m_btnMyBlogURL;
	CMFCLinkCtrl m_btnDuiLibURL;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MY_BLOG_LINK, m_btnMyBlogURL);
	DDX_Control(pDX, IDC_BUTTON_DUILIB_LINK, m_btnDuiLibURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	this->GetDlgItem(IDC_STATIC_CURRENT_VERSION)->SetWindowText(UIDESIGNER_VERSION);
	m_btnMyBlogURL.SetURL(_T("http://www.taxue.org/"));
	m_btnMyBlogURL.SetTooltip(_T("踏雪流云的博客"));
	m_btnMyBlogURL.SizeToContent();
	m_btnDuiLibURL.SetURL(_T("http://code.google.com/p/duilib/"));
	m_btnDuiLibURL.SetTooltip(_T("DuiLib开源项目"));
	m_btnDuiLibURL.SizeToContent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CUIDesignerApp::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码
	ASSERT(m_pUIDocTemplate);

	CDocument* pDoc = m_pUIDocTemplate->OpenDocumentFile(NULL);
	if(pDoc == NULL)
		return;

	CString strPathName = pDoc->GetPathName();
	if(!strPathName.IsEmpty())
	{
		pDoc->SetPathName(_T(""));
		RemoveLastFromMRU();
	}
}

// 用于运行对话框的应用程序命令
void CUIDesignerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CUIDesignerApp 自定义加载/保存方法

void CUIDesignerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
	bNameValid = strName.LoadString(IDS_RESOURCE);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_RESOURCE);
}

void CUIDesignerApp::LoadCustomState()
{
}

void CUIDesignerApp::SaveCustomState()
{
}

//////////////////////////////////////////////////////////////////////////
// CUIDesignerApp 消息处理程序

int CUIDesignerApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CWinAppEx::ExitInstance();
}

void CUIDesignerApp::RemoveLastFromMRU()
{
	m_pRecentFileList->Remove(0);
}
