#ifndef UIPaint_h__
#define UIPaint_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	UIPaint.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-19 14:58:33
// 说明			:	控件绘图辅助
作用是分离不同种类基础控件的绘制过程，复用绘制逻辑代码
/////////////////////////////////////////////////////////////*/
#pragma once

class CUIPaint
{
public:
	static CUIPaint* GetInstance();
	static void ReleaseInstance();

public:
	void DrawBackColor(IUIRender *pRender,DWORD dwBackColor,DWORD dwBackColor2,DWORD dwBackColor3,LPCRECT pPaint,LPCRECT pOverlap,LPCRECT pControl);
	void DrawBackImage();
	void DrawForeImage();
	void DrawTextString();
	void DrawBorder();

	void DrawControl(IUIRender *pRender,CControlUI* pControl,LPCRECT rcUpdate);
	void DrawLabel(IUIRender *pRender,CControlUI *pControl);
	void DrawButton(IUIRender *pRender,CButtonUI *pControl,LPCRECT rcUpdate);
	void DrawRadioButton();
	void DrawCheckBox();
	void DrawEdit(IUIRender *pRender,CEditUI *pControl,LPCRECT rcUpdate);
	void DrawProgress();
	void DrawScrollBar(IUIRender *pRender,CScrollBarUI *pControl,LPCRECT rcUpdate);

	//void DrawControlBackColor(IUIRender *pRender,CControlUI *pControl,LPCRECT pRcUpdate,LPCRECT pRcOverlop);
	//void DrawControlBackImage();
	//void DrawControlStateImage();
	//void DrawControlText();
	//void DrawControlBorder();

private:
	CUIPaint(void);
	~CUIPaint(void);
	static CUIPaint* m_pInstance;
};

#endif // UIPaint_h__
