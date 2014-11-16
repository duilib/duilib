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

	// 一些重要的属性
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

public:
	// Tooltip
	virtual LPCTSTR GetToolTip() const;
	virtual void SetToolTip(LPCTSTR lpszText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int GetToolTipWidth( void ) const;		// 多行ToolTip单行最长宽度
private:
	CDuiString m_sText;
	CDuiString m_strToolTip;
	int m_nTooltipWidth;

public:
	// 控件坐标相关
	virtual RECT GetRect();						// GetPos
	virtual void SetRect(LPCRECT rc);		// SetPos
	virtual RECT GetInterRect();					// 用于HostWindow
	void SetInset(LPRECT rc);
	RECT GetInset();

	// 文本相关
	virtual LPCTSTR GetText() const;
	virtual void SetText(LPCTSTR pstrText);

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
	//virtual void SetRelativePos(SIZE szMove,SIZE szZoom);
	//virtual void SetRelativeParentSize(SIZE sz);
	//virtual TRelativePosUI GetRelativePos() const;
	//virtual bool IsRelativePos() const;

	// 菜单
	virtual bool IsContextMenuUsed() const;
	virtual void SetContextMenuUsed(bool bMenuUsed);

	// 控件状态
	virtual void SetState(UISTATE nState);
	virtual UISTATE GetState(void);

	virtual bool Activate();

	virtual void SetTextForState(LPCTSTR lpszText,UISTATE nState);
	virtual LPCTSTR GetTextForState(UISTATE nState);

	virtual void SetImageForState(LPCTSTR lpszImage,UISTATE nState);
	virtual LPCTSTR GetImageForState(UISTATE nState);

	// 
	DWORD GetBkColor() const;
	void SetBkColor(DWORD dwBackColor);
	LPCTSTR GetBkImage();
	virtual void SetBkImage(LPCTSTR pStrImage);
protected:
	CDuiRect m_rcItem;			// 控件绘制区域
	CDuiRect m_rcInset;		// 内部内容区
	CDuiRect m_rcPadding;	// 边距
	CDuiSize m_cXY;
	CDuiSize m_cxyFixed;
	CDuiSize m_cxyMin;
	CDuiSize m_cxyMax;
	UISTATE m_uState;
	DWORD m_dwBackColor;
	CDuiString m_sBkImage;
public:
	// Manager
	virtual void SetManager(CWindowUI* pManager, CControlUI* pParent);
	CWindowUI* GetManager();
	CControlUI* GetParent();

protected:
	CControlUI* m_pParent;              //父容器
	CWindowUI* m_pManager;              //所属窗口

public:
	// Control
	virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual bool EventHandler(TEventUI& event);		// 处理CWindowUI转发的消息事件
	//virtual void Invoke(TEventUI& event);						// 控件自身业务逻辑处理
	virtual void Render(IUIRender* pRender,LPCRECT pRcPaint);		// 控件基于内部状态绘图
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);

	void Invalidate();
	bool IsUpdateNeeded() const;
	void NeedUpdate();
	void NeedParentUpdate();
	void SetTimer(UINT nTimerID,UINT uElapse);
	void KillTimer(UINT nTimer);

public:
	void	SendNotify(DWORD dwType, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);


private:
	CEventSource m_OnEvent;
	CEventSource OnNotify;
};

#endif // ControlUI_h__
