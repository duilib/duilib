#ifndef GDIRender_h__
#define GDIRender_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	GDIRender.h
// 创建人		: 	daviyang35@gmail.com
// 创建时间	:	2014-11-09 16:49:29
// 说明			:	GDI Render
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Define/IUIRender.h"

class CGDIRender
	: public IUIRender
{
public:
	CGDIRender(void);
	virtual ~CGDIRender(void);

	// 填充颜色
	virtual void DrawColor(LPCRECT rcSrc,DWORD dwColor);
	// 渐变色
	virtual void DrawGradient(LPCRECT rcSrc,DWORD dwStart,DWORD dwEnd,bool bVertical);
	// 圆角矩形边框
	virtual void DrawRoundRectBorder(LPCRECT rcSrc, INT nCornerWidth, INT nCornerHeight, INT nBorderWidth, DWORD dwColor);
	// 矩形边框
	virtual void DrawRectangleBorder(LPCRECT rcSrc,LPCRECT rcBorder,DWORD dwColor);
	// 文字
	virtual void DrawText(LPCRECT rcSrc,LPCTSTR lpszTextString);
	// 画图片
	virtual void DrawImage(ImageObject* pImageObj,LPCRECT rcSrc);
private:
	VOID GradientFill(LPCRECT rcSrc, DWORD dwStart, DWORD dwEnd, bool bVertical);
};

#endif // GDIRender_h__
