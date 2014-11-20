#ifndef IDirectUIRender_h__
#define IDirectUIRender_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	IDirectUIRender.h
// 创建人	: 	daviyang35@gmail.com
// 创建时间	:	2014-11-08 21:23:38
// 说明		:	Render接口
/////////////////////////////////////////////////////////////*/
#pragma once
#include "Utils/UIUtils.h"

class CRenderClip;
class ImageObject;
class FontObject;

class DIRECTUI_API CMemDC
{
public:
	CMemDC();
	~CMemDC();

public:
	void Create(HDC hDC,int cx,int cy);
	void Clear();

	BOOL IsValid();
	HDC GetSafeHdc();
	HBITMAP GetHBitmap();
	int GetWidth();
	int GetHeight();

private:
	HDC m_hMemDC;
	HBITMAP m_hMemBmp;
	HBITMAP m_hOldBmp;
	int m_cx;
	int m_cy;
};

class DIRECTUI_API IUIRender
{
public:
	IUIRender();
	virtual ~IUIRender();

public:
	virtual void SetDevice(CMemDC *memDC);
	virtual void SetDevice(HDC hDc);
	virtual HDC GetPaintDC();

	virtual void SetInvalidateRect(RECT& rect);
	virtual RECT GetInvalidateRect();

public:	// 绘图引擎应该干的事情：画文字，画图片，画线，画区域

	// 填充颜色
	virtual void DrawColor(LPCRECT rcSrc,DWORD dwColor);
	// 渐变色
	virtual void DrawGradient(LPCRECT rcSrc,DWORD dwStart,DWORD dwEnd,bool bVertical,int nStep);
	// 圆角矩形边框
	virtual void DrawRoundRectBorder(LPCRECT rcSrc, INT nCornerWidth, INT nCornerHeight, INT nBorderWidth, DWORD dwColor);
	// 矩形边框
	virtual void DrawRectangleBorder(LPCRECT rcSrc,LPCRECT rcBorder,DWORD dwColor);
	// 文字
	virtual void DrawText(FontObject* pFontObj,LPCRECT rcSrc,LPCTSTR lpszTextString,DWORD dwTextColor,DWORD dwStyle);
	// 画图片
	// pRcControl	是控件矩形区
	// pRcPaint		是刷新区
	virtual void DrawImage(ImageObject* pImageObj,LPCRECT pRcControl,LPCRECT pRcPaint);

protected:
	HDC m_hDC;
	CMemDC *m_pMemDC;
	CDuiRect m_rcInvalidate;
};

class DIRECTUI_API ITextRender
{

};

class DIRECTUI_API CRenderClip
{
public:
	CRenderClip();
	~CRenderClip();
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;

	// 创建一个剪切区域，将绘图限制在rc区域内
	static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);

	// 创建一个圆角剪切区域，将绘图限制在rc区域内
	// width、height 横/纵向圆角弧度
	static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);

	// 使用活跃区域作为绘图区
	static void UseOldClipBegin(HDC hDC, CRenderClip& clip);

	// 恢复控件完整活跃区
	static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
};

#endif // IDirectUIRender_h__
