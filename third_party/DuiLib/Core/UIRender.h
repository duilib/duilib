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

	static bool DrawImageString(HDC hDC,		// ��ͼĿ��DC
		CPaintManagerUI* pManager,
		const RECT& rcItem,								// �ؼ�����
		const RECT& rcPaint,								// �ؼ���ˢ�������ص�����
		LPCTSTR pStrImage,								// ��ͼ��ʽ�ַ���
		LPCTSTR pStrModify = NULL);

    static void DrawImage(
		HDC hDC,							// ��ͼĿ��DC
		HBITMAP hBitmap,				// ͼƬ��Դ
		const RECT& rc,					// DC����
		const RECT& rcPaint,			// ��ͼĿ������
        const RECT& rcBmpPart,		// ͼƬ��Դʹ������
		const RECT& rcCorners,		// ͼƬ�ľŹ���
		bool alphaChannel,				// �Ƿ�֧��͸��
		BYTE uFade = 255,				// Alphaֵ
        bool hole = false,				// �Ƿ���ƾŹ�������
		bool xtiled = false,				//	x��ƽ��
		bool ytiled = false);				// y��ƽ��
   
    static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

    // ���º����е���ɫ����alphaֵ��Ч
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
