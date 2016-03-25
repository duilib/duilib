#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderClip
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

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderEngine
{
public:
    static TImageInfo* LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    static void FreeImage(const TImageInfo* bitmap);

	static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);

	static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc);
	static SIZE GetTextSize(HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle);

	static bool DrawImageString(HDC hDC,		// 绘图目标DC
		CPaintManagerUI* pManager,
		const RECT& rcItem,								// 控件区域
		const RECT& rcPaint,								// 控件与刷新区的重叠部分
		LPCTSTR pStrImage,								// 绘图格式字符串
		LPCTSTR pStrModify = NULL);

    static void DrawImage(
		HDC hDC,							// 绘图目标DC
		HBITMAP hBitmap,				// 图片资源
		const RECT& rc,					// DC区域
		const RECT& rcPaint,			// 绘图目标区域
        const RECT& rcBmpPart,		// 图片资源使用区域
		const RECT& rcCorners,		// 图片的九宫格
		bool alphaChannel,				// 是否支持透明
		BYTE uFade = 255,				// Alpha值
        bool hole = false,				// 是否绘制九宫格中心
		bool xtiled = false,				//	x轴平铺
		bool ytiled = false);				// y轴平铺
   
    static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

    // 以下函数中的颜色参数alpha值无效
    static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
    static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
    static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
    static void DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, \
        DWORD dwTextColor, int iFont, UINT uStyle);
    static void DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, 
        DWORD dwTextColor, RECT* pLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle);
    
};

} // namespace DuiLib

#endif // __UIRENDER_H__
