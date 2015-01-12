#ifndef MessageDlg_h__
#define MessageDlg_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	MessageDlg.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-18 12:45:41
// 说明			:	模态窗口测试
/////////////////////////////////////////////////////////////*/
#pragma once
class CMessageDlg
	: public CWindowUI
{
public:
	CMessageDlg(void);
	virtual ~CMessageDlg(void);

	void Init(HWND hWndParent);
	virtual void OnFinalMessage(HWND hWnd);
	virtual bool Notify(TNotifyUI *pMsg);
};

#endif // MessageDlg_h__
