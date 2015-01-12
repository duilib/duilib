#ifndef MainWnd_h__
#define MainWnd_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	MainWnd.h
// 创建人	: 	daviyang35@QQ.com
// 创建时间	:	2014-11-08 17:27:41
// 说明		:	DirectUI Window测速窗口
/////////////////////////////////////////////////////////////*/
#pragma once

class CMainWnd
	: public CWindowUI
{
public:
	CMainWnd(void);
	virtual ~CMainWnd(void);

	void Init();

	virtual void OnFinalMessage(HWND hWnd);

	virtual bool Notify(TNotifyUI *pMsg);

};

#endif // MainWnd_h__
