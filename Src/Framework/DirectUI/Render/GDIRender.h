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

	virtual void DrawImage(ImageObject* pImageObj, int nAlpha, int x, int y, int nIndex = 0, bool bHole =false);
	virtual void DrawImage(ImageObject* pImageObj, int nAlpha, RECT& rect, int nIndex = 0, bool bHole =false);
	virtual void DrawImage(ImageObject* pImageObj, RECT& rcSrc, RECT& rcDest, int nIndex = 0);

	virtual void DrawColor(LPCRECT rcSrc,DWORD dwColor);

};

#endif // GDIRender_h__
