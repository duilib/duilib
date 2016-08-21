#ifndef ImageObject_h__
#define ImageObject_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ImageObject.h
// 创建人		: 	daviyang35@QQ.com
// 创建时间	:	2014-11-09 15:30:54
// 说明			:	绘图资源包装对象	
绘图资源，多个对象可能引用同一个图片对象
/////////////////////////////////////////////////////////////*/
#pragma once


namespace DuiLib
{

class UILIB_API ImageObject
{
public:
	void Release();
	ImageObject* Get();

public:
	ImageObject();
	ImageObject(HBITMAP hBitmap);
	ImageObject(HICON hIcon);
	ImageObject(BYTE* pPixels, int nWidth, int nHeight);
	virtual ~ImageObject(void);

	void SetHBitmap(HBITMAP hBitmap);
	HBITMAP  GetHBitmap();

	void SetHIcon(HICON hIcon);
	HICON  GetHIcon();

	void SetPixels(BYTE* pPixels, int nWidth, int nHeight);
	BYTE*  GetPixels();

public:
	void    Init();
	void    Desotry();

private:
	TImageData* m_pImageData;
	int m_nAlpha;
	DWORD m_dwMask;
	bool m_bHole;
	bool m_bXTiled;
	bool m_bYTiled;
	CDuiRect m_rcDest;
	CDuiRect m_rcSource;
	CDuiRect m_rc9Grid;	// 九宫格
	CDuiString m_strFilePath;
};

}
#endif // ImageObject_h__
