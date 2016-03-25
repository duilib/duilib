#ifndef ImageObject_h__
#define ImageObject_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	ImageObject.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-11-09 15:30:54
// ˵��			:	��ͼ��Դ��װ����	
��ͼ��Դ����������������ͬһ��ͼƬ����
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
	CDuiRect m_rc9Grid;	// �Ź���
	CDuiString m_strFilePath;
};

#endif // ImageObject_h__
