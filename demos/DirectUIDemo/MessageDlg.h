#ifndef MessageDlg_h__
#define MessageDlg_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	MessageDlg.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-11-18 12:45:41
// ˵��			:	ģ̬���ڲ���
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
