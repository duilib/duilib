#ifndef ButtonUI_h__
#define ButtonUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	ButtonUI.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-11-16 23:46:28
// ˵��			:	Button
/////////////////////////////////////////////////////////////*/
#pragma once

class DIRECTUI_API CButtonUI
	: public CControlUI
{
public:
	CButtonUI(void);
	virtual ~CButtonUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);
	virtual bool IsClientArea();
	UINT GetControlFlags() const;

	bool Activate();
	void SetEnabled(bool bEnable = true);

	virtual bool EventHandler(TEventUI& event);
	virtual void Render(IUIRender* pRender,LPCRECT pRcPaint);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState);
	virtual ImageObject* GetImageProperty(UIProperty propType,DWORD dwState = UISTATE_Normal);

public:

private:
	// �������ͼƬ����
	//ImageObject *m_pNormalImage;
	ImageObject *m_pHoverImage;
	ImageObject *m_pPushedImage;
	ImageObject *m_pFocusedImage;
	ImageObject *m_pDisabledImage;

	ImageObject *m_pForeHoverImage;
	ImageObject *m_pForePushedImage;
};

#endif // ButtonUI_h__
