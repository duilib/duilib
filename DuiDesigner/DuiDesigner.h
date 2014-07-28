
// UIDesigner.h : UIDesigner 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CUIDesignerApp:
// 有关此类的实现，请参阅 UIDesigner.cpp
//

class CUIDesignerApp : public CWinAppEx
{
public:
	CUIDesignerApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CMultiDocTemplate* m_pUIDocTemplate;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	void RemoveLastFromMRU();
	CMultiDocTemplate* GetUIDocTemplate() const { return m_pUIDocTemplate; }

	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CUIDesignerApp theApp;
