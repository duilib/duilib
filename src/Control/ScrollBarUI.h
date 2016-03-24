#ifndef ScrollBarUI_h__
#define ScrollBarUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	ScrollBarUI.h
// ������		: 	daviyang35@QQ.com
// ����ʱ��	:	2014-11-14 9:20:00
// ˵��			:	������
/////////////////////////////////////////////////////////////*/
#pragma once
class CContainerUI;

class DIRECTUI_API CScrollBarUI
	: public CControlUI
{
	enum
	{ 
		DEFAULT_SCROLLBAR_SIZE = 16,
		DEFAULT_TIMERID = 10,
	};
public:
	CScrollBarUI(void);
	virtual ~CScrollBarUI(void);
	UI_DECLARE_DYNCREATE();

	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);
	virtual bool IsClientArea();
	CContainerUI* GetOwner() const;
	void SetOwner(CContainerUI* pOwner);

	bool IsHorizontal();
	void SetHorizontal(bool bHorizontal = true);
	int GetScrollRange() const;
	void SetScrollRange(int nRange);
	int GetScrollPos() const;
	void SetScrollPos(int nPos);
	int GetLineSize() const;
	void SetLineSize(int nSize);

	//////////////////////////////////////////////////////////////////////////
	// CControlUI
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	virtual void SetVisible(bool bVisible = true);
	virtual void SetEnabled(bool bEnable = true);
	virtual void SetFocus();
	virtual void Render(IUIRender* pRender,LPCRECT pRcPaint);
	virtual void SetPosition(LPCRECT lpRc);

private:
	bool m_bHorizontal;
	int m_nRange;
	int m_nScrollPos;
	int m_nLineSize;
	CContainerUI* m_pOwner;
	POINT m_ptLastMouse;
	int m_nLastScrollPos;
	int m_nLastScrollOffset;
	int m_nScrollRepeatDelay;

	DWORD m_uButton1State;		// 
	DWORD m_uButton2State;
	DWORD m_uThumbState;		// �������м���϶���ť

	CDuiRect m_rcButton1;
	CDuiRect m_rcButton2;
	CDuiRect m_rcThumb;

	bool m_bShowButton1;
	bool m_bShowButton2;

};

#endif // ScrollBarUI_h__
