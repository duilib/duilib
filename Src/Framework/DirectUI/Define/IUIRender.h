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
	int GetWidth();;
	int GetHeight();;

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
	virtual HDC GetPaintDC();

	virtual void SetInvalidateRect(RECT& rect);
	virtual RECT GetInvalidateRect();

public:
	// 绘图引擎应该干的事情：画文字，画图片，画线，画区域

	virtual void DrawImage(ImageObject* pImageObj, int nAlpha, int x, int y, int nIndex = 0, bool bHole =false);
	virtual void DrawImage9Gird(ImageObject* pImageObj,LPCRECT lp9Gird, int nAlpha, int x, int y, int nIndex = 0, bool bHole =false);
	virtual void DrawImage(ImageObject* pImageObj, int nAlpha, RECT& rect, int nIndex = 0, bool bHole =false);
	virtual void DrawImage(ImageObject* pImageObj, RECT& rcSrc,  RECT& rcDest, int nIndex = 0);

	void DrawText();
	void DrawLine();
	void DrawRect();
private:
	CMemDC *m_pMemDC;
	CDuiRect m_rcInvalidate;
};

class DIRECTUI_API ITextRender
{

};

class DIRECTUI_API CRenderClip
{
public:
	~CRenderClip();
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;

	static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);
	static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
	static void UseOldClipBegin(HDC hDC, CRenderClip& clip);
	static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
};

#endif // IDirectUIRender_h__
