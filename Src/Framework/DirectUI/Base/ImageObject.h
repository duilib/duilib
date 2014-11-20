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

class DIRECTUI_API ImageObject
{
public:
	ImageObject(void);
	~ImageObject(void);

	void SetImagePath(LPCTSTR lpszImagePath);
	LPCTSTR GetImagePath(void) const;

	void SetAlpha(int iAlpha);
	int GetAlpha(void) const;

	void SetHole(bool bHole);
	bool GetHole(void) const;
	void SetXTiled(bool bXTiled);
	void SetYTiled(bool bYTiled);
	bool GetXTiled(void) const;
	bool GetYTiled(void) const;

	void SetMaskColor(DWORD dwMask);
	DWORD GetMaskColor(void) const;

	void SetDest(RECT &rc);
	RECT GetDest(void) const;

	void SetSource(RECT &rc);
	RECT GetSource(void) const;

	void Set9Gird(RECT &rc);
	RECT Get9Gird(void) const;

	HBITMAP GetHBitmap();
	int GetImageWidth();
	int GetImageHeight();
	bool IsAlphaImage();

	void Init();

public:
	static ImageObject* CreateFromString(LPCTSTR lpszString);

private:
	TImageData* m_pImageData;
	int m_nAlpha;
	//int m_nFrame;
	DWORD m_dwMask;
	bool m_bHole;
	bool m_bXTiled;
	bool m_bYTiled;
	CDuiRect m_rcDest;
	CDuiRect m_rcSource;
	CDuiRect m_rc9Grid;	// 九宫格
	CDuiString m_strFilePath;
};

#endif // ImageObject_h__
