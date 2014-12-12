#ifndef ControlUI_h__
#define ControlUI_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	ControlUI.h
// 创建人	: 	daviyang35@gmail.com
// 创建时间	:	2014-11-08 15:36:37
// 说明		:	
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

	// 基础属性
	virtual bool IsVisible() const;
	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
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
	virtual int GetToolTipWidth(void) const;		// 多行ToolTip单行最长宽度

	// 控件坐标相关
	virtual RECT GetPosition();						// GetPos
	virtual void SetPosition(LPCRECT rc);		// SetPos
	virtual RECT GetInterRect();					// 用于HostWindow
	void SetInset(LPRECT rc);
	RECT GetInset();
	SIZE EstimateSize(SIZE szAvailable);

	// 文本相关
	virtual LPCTSTR GetText() const;
	virtual void SetText(LPCTSTR pstrText);
	virtual void SetFontName(LPCTSTR lpszFontIndex);
	virtual LPCTSTR GetFontName();
	virtual void SetTextStyle(LPCTSTR lpszStyle);
	virtual LPCTSTR GetTextStyle();
	virtual UINT GetTextStyleByValue();
	virtual void SetTextColor(DWORD dwColor);
	virtual DWORD GetTextColor();

	// 位置相关
	virtual int GetWidth() const;
	virtual int GetHeight() const;
	virtual int GetX() const;
	virtual int GetY() const;
	virtual RECT GetPadding() const;
	virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
	virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
	virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void SetFixedWidth(int cx);      // 预设的参考值
	virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
	virtual void SetFixedHeight(int cy);     // 预设的参考值
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

	// 显示属性
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

	// 快捷键
	virtual TCHAR GetShortcut() const;
	virtual void SetShortcut(TCHAR ch);

	// 菜单
	virtual bool IsContextMenuUsed() const;
	virtual void SetContextMenuUsed(bool bMenuUsed);

	// 控件状态
	virtual DWORD ModifyState(DWORD dwStateAdd = 0, DWORD dwStateRemove = 0);
	virtual DWORD GetState(void);
	virtual bool CheckState(DWORD dwState);

	// 事件过滤
	virtual void SetNotifyFilter(INotifyUI* pNotifyFilter);
	virtual INotifyUI* GetNotifyFilter(void) const;
	void SendNotify(UINOTIFY dwType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

	// 状态
	virtual bool Activate();

	// 绘图状态与属性
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
	virtual bool EventHandler(TEventUI& event);		// 处理CWindowUI转发的消息事件
	virtual void Render(IUIRender* pRender, LPCRECT pRcPaint);		// 控件基于内部状态绘图
	virtual void PostRender(IUIRender* pRender, LPCRECT pRcPaint);		// 控件基于内部状态绘图
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	// 刷新
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
	bool m_bIsSetPos; // 防止SetPos循环调用
	bool m_bIsUpdateNeeded;
	bool m_bIsMenuUsed;
	CEventSource m_OnEvent;
	CEventSource m_OnInit;

protected:
	//文本相关
	CDuiString m_sText;
	CDuiString m_sFontIndex;
	CDuiString m_sTextStyle;
	UINT m_uTextStyle;
	DWORD m_dwTextColor;
	
	// tooltip
	CDuiString m_strToolTip;
	int m_nTooltipWidth;

	// 快捷键
	TCHAR m_chShortcut;

	// 属性
	DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
	SIZE m_cxyBorderRound;
	int m_nBorderSize;

protected:
	CDuiRect m_rcControl;	// 控件绘制区域
	CDuiRect m_rcInset;		// 内部内容区
	CDuiRect m_rcPadding;	// 边距
	CDuiSize m_cXY;
	CDuiSize m_cxyFixed;
	CDuiSize m_cxyMin;
	CDuiSize m_cxyMax;
	DWORD m_dwState;
	int m_iZOrder;
	LPVOID m_pTag;

	CControlUI* m_pParent;              //父容器
	CWindowUI* m_pManager;              //所属窗口
	INotifyUI *m_pNotifyFilter;

	UIStatePropertyMap m_property;

	ImageObject *m_pImageBackground;

	virtual void SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState);
};

#endif // ControlUI_h__
