#ifndef ControlUI_h__
#define ControlUI_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	ControlUI.h
// ������	: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-08 15:36:37
// ˵��		:	
/////////////////////////////////////////////////////////////*/
#pragma once

typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class DIRECTUI_API CControlUI
	: public CObjectUI
{
	friend class CWindowUI;
public:
	CControlUI(void);
	virtual ~CControlUI(void);
	UI_DECLARE_DYNCREATE();

	virtual UINT GetControlFlags() const;
	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR lpszClass);

	virtual void Init();
	virtual void DoInit();

	// ��������
	virtual bool IsVisible() const;
	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true); // �����ڲ����ã���ЩUIӵ�д��ھ������Ҫ��д�˺���
	virtual bool IsEnabled() const;
	virtual void SetEnabled(bool bEnable = true);
	virtual bool IsMouseEnabled() const;
	virtual void SetMouseEnabled(bool bEnable = true);
	virtual bool IsKeyboardEnabled() const;
	virtual void SetKeyboardEnabled(bool bEnable = true);
	virtual bool IsFocused() const;
	virtual void SetFocus();
	virtual bool IsFloat() const;
	virtual void SetFloat(bool bFloat = true);

	// Tooltip
	virtual LPCTSTR GetToolTip() const;
	virtual void SetToolTip(LPCTSTR lpszText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int GetToolTipWidth(void) const;		// ����ToolTip��������

	// �ؼ��������
	virtual RECT GetPosition();						// GetPos
	virtual void SetPosition(LPCRECT rc);		// SetPos
	virtual RECT GetInterRect();					// ����HostWindow
	void SetInset(LPRECT rc);
	RECT GetInset();
	SIZE EstimateSize(SIZE szAvailable);

	// �ı����
	virtual LPCTSTR GetText() const;
	virtual void SetText(LPCTSTR pstrText);
	virtual void SetFontName(LPCTSTR lpszFontIndex);
	virtual LPCTSTR GetFontName();
	virtual void SetTextStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR GetTextStyle();
	virtual UINT GetTextStyleByValue();
	virtual void SetTextColor(DWORD dwColor);
	virtual DWORD GetTextColor();

	// λ�����
	virtual int GetWidth() const;
	virtual int GetHeight() const;
	virtual int GetX() const;
	virtual int GetY() const;
	virtual RECT GetPadding() const;
	virtual void SetPadding(RECT rcPadding); // ������߾࣬���ϲ㴰�ڻ���
	virtual SIZE GetFixedXY() const;         // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
	virtual void SetFixedXY(SIZE szXY);      // ��floatΪtrueʱ��Ч
	virtual int GetFixedWidth() const;       // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
	virtual void SetFixedWidth(int cx);      // Ԥ��Ĳο�ֵ
	virtual int GetFixedHeight() const;      // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
	virtual void SetFixedHeight(int cy);     // Ԥ��Ĳο�ֵ
	virtual int GetMinWidth() const;
	virtual void SetMinWidth(int cx);
	virtual int GetMaxWidth() const;
	virtual void SetMaxWidth(int cx);
	virtual int GetMinHeight() const;
	virtual void SetMinHeight(int cy);
	virtual int GetMaxHeight() const;
	virtual void SetMaxHeight(int cy);
	virtual void SetZOrder(int iZOrder);
	virtual int GetZOrder() const;

	// ��ʾ����
	virtual void SetBorderColor(DWORD dwColor);
	virtual DWORD GetBorderColor();
	virtual void SetFocusBorderColor(DWORD dwColor);
	virtual DWORD GetFocusBorderColor();
	virtual void SetBorderRound(SIZE cxySize);
	virtual SIZE GetBorderRound();
	virtual void SetBorderSize(int nSize);
	virtual int GetBorderSize();

	// tag
	virtual void SetTag(LPVOID pTag);
	virtual LPVOID GetTag() const;

	// ��ݼ�
	virtual TCHAR GetShortcut() const;
	virtual void SetShortcut(TCHAR ch);

	// �˵�
	virtual bool IsContextMenuUsed() const;
	virtual void SetContextMenuUsed(bool bMenuUsed);

	// �ؼ�״̬
	virtual DWORD ModifyState(DWORD dwStateAdd = 0, DWORD dwStateRemove = 0);
	virtual DWORD GetState(void);
	virtual bool CheckState(DWORD dwState);

	// �¼�����
	virtual void SetNotifyFilter(INotifyUI* pNotifyFilter);
	virtual INotifyUI* GetNotifyFilter(void) const;
	void SendNotify(UINOTIFY dwType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	// ״̬
	virtual bool Activate();

	// ��ͼ״̬������
	virtual void SetPropertyForState(LPCTSTR lpszValue, UIProperty propType, DWORD dwState = UISTATE_Normal);
	virtual LPCTSTR GetPropertyForState(UIProperty propType, DWORD dwState = UISTATE_Normal);
	virtual RECT GetRectProperty(UIProperty propType, DWORD dwState = UISTATE_Normal);
	virtual DWORD GetColorProperty(UIProperty propType, DWORD dwState = UISTATE_Normal);
	virtual int GetIntProperty(UIProperty propType, DWORD dwState = UISTATE_Normal);
	virtual ImageObject* GetImageProperty(UIProperty propType, DWORD dwState = UISTATE_Normal);
	virtual LPCTSTR GetTextProperty(UIProperty propType, DWORD dwState = UISTATE_Normal);

	// Manager
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent,bool bInit = false);
	CWindowUI* GetManager();
	CControlUI* GetParent();

	// Control
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual bool EventHandler(TEventUI& event);		// ����CWindowUIת������Ϣ�¼�
	virtual void Render(IUIRender* pRender, LPCRECT pRcPaint);		// �ؼ������ڲ�״̬��ͼ
	virtual void PostRender(IUIRender* pRender, LPCRECT pRcPaint);		// �ؼ������ڲ�״̬��ͼ
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	// ˢ��
	void Invalidate();
	bool IsUpdateNeeded() const;
	void NeedUpdate();
	void NeedParentUpdate();
	void SetTimer(UINT nTimerID, UINT uElapse);
	void KillTimer(UINT nTimer);

protected:
	bool m_bIsVisible;
	bool m_bIsInternVisible;
	bool m_bIsEnabled;
	bool m_bIsMouseEnabled;
	bool m_bIsKeyboardEnabled ;
	bool m_bIsFocused;
	bool m_bIsFloat;
	bool m_bIsSetPos; // ��ֹSetPosѭ������
	bool m_bIsUpdateNeeded;
	bool m_bIsMenuUsed;
	CEventSource m_OnEvent;
	CEventSource m_OnInit;

protected:
	//�ı����
	CDuiString m_sText;
	CDuiString m_sFontIndex;
	CDuiString m_sTextStyle;
	UINT m_uTextStyle;
	DWORD m_dwTextColor;
	
	// tooltip
	CDuiString m_strToolTip;
	int m_nTooltipWidth;

	// ��ݼ�
	TCHAR m_chShortcut;

	// ����
	DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
	SIZE m_cxyBorderRound;
	int m_nBorderSize;

protected:
	CDuiRect m_rcControl;	// �ؼ���������
	CDuiRect m_rcInset;		// �ڲ�������
	CDuiRect m_rcPadding;	// �߾�
	CDuiSize m_cXY;
	CDuiSize m_cxyFixed;
	CDuiSize m_cxyMin;
	CDuiSize m_cxyMax;
	DWORD m_dwState;
	int m_iZOrder;
	LPVOID m_pTag;

	CControlUI* m_pParent;              //������
	CWindowUI* m_pManager;              //��������
	INotifyUI *m_pNotifyFilter;

	UIStatePropertyMap m_property;

	ImageObject *m_pImageBackground;

	virtual void SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState);
};

#endif // ControlUI_h__
