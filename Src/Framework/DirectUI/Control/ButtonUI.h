#ifndef ButtonUI_h__
#define ButtonUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ButtonUI.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-16 23:46:28
// 说明			:	Button
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
	UINT GetControlFlags() const;

	bool Activate();
	void SetEnabled(bool bEnable = true);

	virtual bool EventHandler(TEventUI& event);
	virtual void Render(IUIRender* pRender,LPCRECT pRcPaint);

	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

public:
	void SetNormalImage(LPCTSTR lpszImageString);

private:
	// 属性字符串
	CDuiString m_strNormalImage;
	CDuiString m_strHoverImage;
	CDuiString m_strPushedImage;
	CDuiString m_strFocusedImage;
	CDuiString m_strDisabledImage;

	CDuiString m_strForeHoverImage;
	CDuiString m_strForePushedImage;

	// 解析后的图片对象
	ImageObject *m_pNormalImage;
	ImageObject *m_pHoverImage;
	ImageObject *m_pPushedImage;
	ImageObject *m_pFocusedImage;
	ImageObject *m_pDisabledImage;

	ImageObject *m_pForeHoverImage;
	ImageObject *m_pForePushedImage;
};

#endif // ButtonUI_h__
