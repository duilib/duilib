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
	LPCTSTR GetImagePath(void);

	int GetWidth();
	int GetHeight();

public:
	ImageObject* Get();
	void Release();

private:
	int m_nWidth;
	int m_nHeight;
	int m_nAlpha;
	int m_nFrame;
	CDuiRect m_rc9Grid;	// 九宫格
	CDuiString m_strFilePath;
	CDuiString m_strKey;
};

#endif // ImageObject_h__
