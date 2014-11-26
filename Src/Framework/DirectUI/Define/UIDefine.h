#ifndef UIDefine_h__
#define UIDefine_h__
/*/////////////////////////////////////////////////////////////
//
// 文件名称	:	UIDefine.h
// 创建人	: 	daviyang35@gmail.com
// 创建时间	:	2014-11-07 17:01:26
// 说明		:	
/////////////////////////////////////////////////////////////*/
#pragma once

// namespace Micro
#if 1
#define DIRECTUI_BEGIN_NAMESPCE namespace DirectUI {
#define DIRECTUI_END_NAMESPCE }
#define DIRECTUI_USING_NAMESPACE using namespace DirectUI;
#else
#define DIRECTUI_BEGIN_NAMESPCE 
#define DIRECTUI_END_NAMESPCE
#define DIRECTUI_USING_NAMESPACE
#endif

// compiler output Micro
#define chSTR(x)	   #x
#define chSTR2(x)	chSTR(x)
#define TODO(desc) message(__FILE__ "(" chSTR2(__LINE__) ") : TODO ==> " #desc)
#define FIXED(desc) message(__FILE__ "(" chSTR2(__LINE__) ") : FIXED ==> " #desc)

//////////////////////////////////////////////////////////////////////////
// 宏定义
#ifndef CDuiString
#ifdef _UNICODE
#define CDuiString std::wstring
#else
#define CDuiString std::string
#endif // _UNICODE
#endif // CDuiString

#define CDuiStringA	std::string
#define CDuiStringW	std::wstring

#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifndef GET_WPARAM
#define GET_WPARAM(wp, lp)                      (wp)
#endif // !GET_WPARAM
#ifndef GET_LPARAM
#define GET_LPARAM(wp, lp)                      (lp)
#endif // !GET_LPARAM
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif // !GET_X_LPARAM
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif // !GET_Y_LPARAM
#ifndef GetAValue
#define GetAValue(rgb)      (LOBYTE((rgb)>>24))
#endif // !GetAValue
#ifndef GetWindowOwner
#define     GetWindowOwner(hwnd)    GetWindow(hwnd, GW_OWNER)
#endif // !GetWindowOwner

//////////////////////////////////////////////////////////////////////////
// 枚举

// INotify接口使用的Type
typedef enum _enUINOTIFY
{
	UINOTIFY__FIRST,

	// 窗口事件
	UINOTIFY_WindowInit,
	UINOTIFY_WindowPaint,

	// 通用事件
	UINOTIFY_LBTNCLICK,
	UINOTIFY_RBTNCLICK,
	UINOTIFY_LDBCLICK,
	UINOTIFY_RDBCLICK,
	UINOTIFY_SELCHANGE,//selchange
	UINOTIFY_SETFOCUS,//setfocus
	UINOTIFY_KILLFOCUS,//killfocus
	UINOTIFY_HOVER,//houver
	UINOTIFY_TIMER,//timer
	UINOTIFY_CONTEXTMENU,
	UINOTIFY_DROPFILES,//dropfiles

	// TabLayout
	UINOTIFY_TAB_SELECTED,
	//热键
	UINOTIFY_HOTKEY, 
	// 窗口还原时的消息

	UINOTIFY__LAST = UINOTIFY__FIRST + 1000,
	UINOTIFY__USER,	/// 其他自定义控件消息的起点.
	UINOTIFY__USER_LAST = UINOTIFY__USER + 1000,
	// 皮肤事件

}UINOTIFY;

// Control内部传递的EventHandler
typedef enum _UIEVENT
{
	UIEVENT__FIRST = 0,
	UIEVENT__KEYBEGIN,			// Keyboard Begin
	UIEVENT_KEYDOWN,			// KeyDown
	UIEVENT_KEYUP,					// KeyUp
	UIEVENT_CHAR,					// Char
	UIEVENT_IME_CHAR,			// IME Char
	UIEVENT_IME_START,
	UIEVENT_IME_END,
	UIEVENT_IME_ING,
	UIEVENT_SYSKEY,				// SystemKey
	UIEVENT__KEYEND,				// Keyboard End

	UIEVENT__MOUSEBEGIN,	// Mouse Begin
	UIEVENT_MOUSEMOVE,		// MouseMove
	UIEVENT_MOUSELEAVE,		// MouseLeave
	UIEVENT_MOUSEENTER,		// MouseEnter
	UIEVENT_MOUSEHOVER,	// MouseHover
	UIEVENT_LBUTTONDOWN,// MouseLeftButtonDown
	UIEVENT_LBUTTONUP,		// MouseLeftButtonUp
	UIEVENT_LDBLCLICK,			// MouseLeftButtonDoubleClick
	UIEVENT_RBUTTONDOWN,// MouseRightButtonDown
	UIEVENT_RBUTTONUP,		// MouseRightButtonup
	UIEVENT_RDBLCLICK,			// MouseRightButtonDoubleClick
	UIEVENT_SCROLLWHEEL,	// MouseMiddleScrollWheel
	UIEVENT__MOUSEEND,		// Mouse End

	UIEVENT_CONTEXTMENU,
	UIEVENT_KILLFOCUS,
	UIEVENT_SETFOCUS,
	UIEVENT_VSCROLL,
	UIEVENT_HSCROLL,
	UIEVENT_WINDOWSIZE,
	UIEVENT_SETCURSOR,
	UIEVENT_TIMER,
	UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	UIEVENT_DROPFILES,
	UIEVENT_MENUITEM_CLICK,
	UIEVENT__LAST,
}UIEVENT;

typedef enum _enUIProperty
{
	UIProperty_BackColor,			// 背景色
	UIProperty_Back_Color1 = UIProperty_BackColor,
	UIProperty_Back_Color2,		// 渐变色
	UIProperty_Back_Color3,		// 渐变色
	UIProperty_Back_Image,		// 背景图
	UIProperty_Fore_Image,		// 前景图
	UIProperty_Text_String,		// 文字
	UIProperty_Text_Font,			// 文字样式
	UIProperty_Text_Color,		// 文字颜色
	UIProperty_Border_Color,	// 边框颜色
	UIProperty_Border_Rect,		// 边框矩形
	UIProperty_Border_Wdith,	// 边框宽度
	UIProperty_Border_Style,		// 边框样式
}UIProperty;

class CControlUI;
class CButtonUI;
class IUIRender;
class CObjectUI;
class ImageObject;
typedef struct _stFontObject FontObject;
class TemplateObject;
class CScrollBarUI;
class CButtonUI;
class CContainerUI;

//////////////////////////////////////////////////////////////////////////
// 结构体

typedef struct _stTFontInfo
{
	HFONT hFont;
	CDuiString strFaceName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;
	TEXTMETRIC tm;
} TFontInfo;

typedef struct _stTImageData
{
	HBITMAP hBitmap;
	int nX;
	int nY;
	int delay;
	bool alphaChannel;
	CDuiString strResType;
	DWORD dwMask;
	UINT nRefCount;
} TImageData;

typedef struct _stFindTabInfo
{
	CControlUI* pFocus;	// 当前焦点控件
	CControlUI* pLast;		
	bool bForward;			// true 下一个控件，false 上一个控件
	bool bNextIsIt;
} FindTabInfo;

typedef struct _stFindShortCut
{
	TCHAR ch;
	bool bPickNext;
} FindShortCut;

typedef struct _stTimerInfo
{
	CControlUI* pSender;
	UINT nLocalID;
	HWND hWnd;
	UINT uWinTimer;
	bool bKilled;
} TimerInfo;

typedef struct _stProperty
{
	UIProperty property;
	CDuiString strValue;
}Property;

// Structure for notifications from the system
// to the control implementation.
typedef struct _stTEventUI
{
	UIEVENT dwType;
	CControlUI* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	TCHAR chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;

	_stTEventUI()
		: dwType(UIEVENT__FIRST)
		, pSender(NULL)
		, dwTimestamp(0)
		, chKey(0)
		, wKeyState(0)
		, wParam(0)
		, lParam(0)
	{
		ptMouse.x = 0;
		ptMouse.y = 0;
	}
} TEventUI;

// Structure for notifications to the outside world
typedef struct _stTNotifyUI 
{
	UINOTIFY dwType;
	CControlUI* pSender;
	DWORD dwTimestamp;	// 时间戳
	POINT ptMouse;
	WPARAM wParam;
	LPARAM lParam;
	_stTNotifyUI()
		: pSender(NULL)
		, dwTimestamp(0)
		, wParam(0)
		, lParam(0)
	{
		ptMouse.x = 0;
		ptMouse.y = 0;
	}
} TNotifyUI;

//////////////////////////////////////////////////////////////////////////
// 接口

// Listener interface
class INotifyUI
{
public:
	// 返回true表明事件已经处理，不再继续投递
	virtual bool Notify(TNotifyUI *pMsg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
	virtual LRESULT MessageFilter(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};

class IContainerUI
{
public:
	virtual CControlUI* GetItemAt(int iIndex) const = 0;
	virtual int GetItemIndex(CControlUI* pControl) const  = 0;
	virtual bool SetItemIndex(CControlUI* pControl, int iIndex)  = 0;
	virtual int GetCount() const = 0;
	virtual bool Add(CControlUI* pControl) = 0;
	virtual bool AddAt(CControlUI* pControl, int iIndex)  = 0;
	virtual bool Remove(CControlUI* pControl) = 0;
	virtual bool RemoveAt(int iIndex)  = 0;
	virtual void RemoveAll() = 0;
	virtual CControlUI* FindSubControl(LPCTSTR pstrSubControlName) =0;
};

//////////////////////////////////////////////////////////////////////////
// 位标志宏定义

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

#define UISTATE_ALL 0x80000000

// Flags used for controlling the paint
#define UISTATE_Normal		0x00000001		// Normal	1
#define UISTATE_Hover			0x00000002		// Hover		2
#define UISTATE_Pushed		0x00000004		// Pushed	4
#define UISTATE_Focused		0x00000008		// Focused	8
#define UISTATE_Selected		0x00000010		// Selected	16
#define UISTATE_Disabled		0x00000020		// Disabled	32 
#define UISTATE_Checked		0x00000040		// CheckBox RadioButton Selected Flag 64
#define UISTATE_ReadOnly		0x00000080		// 128
#define UISTATE_Captured		0x00000100		// 256

//////////////////////////////////////////////////////////////////////////
// 辅助

#define UI_DECLARE_DYNCREATE() \
	static CControlUI* CreateObject()\

#define UI_IMPLEMENT_DYNCREATE(ClassName)\
	CControlUI* ClassName::CreateObject()\
{ return new ClassName; }\

#define UI_REGISTER_DYNCREATE(ControlName, ClassName,bIsActive)\
	CUIEngine::GetInstance()->RegisterControl(ControlName, &ClassName::CreateObject,bIsActive)

#define UI_UNREGISTER_DYNCREATE(ControlName)\
	CUIEngine::GetInstance()->UnregisterControl(ControlName)

typedef CControlUI* (_cdecl *PROCCONTROLCREATE)();
typedef std::vector<CDuiString>								VecString;
typedef std::map<CDuiString,CDuiString>					StringMap;
typedef std::map<CDuiString,TemplateObject*>		TemplateMap;
typedef std::map<CDuiString,ImageObject*>			ImagePoolMap;
typedef std::vector<FontObject*>								FontPoolVector;
typedef std::multimap<DWORD,Property>				UIStatePropertyMap;

typedef StringMap AttributeMap;

typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

static DWORD WM_DIRECTUI_MESSAGE	= ::RegisterWindowMessage(_T("WM_DIRECTUI_MESSAGE"));

static UINT MapKeyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 )
		uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 )
		uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 )
		uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 )
		uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 )
		uState |= MK_ALT;
	return uState;
}

#endif // UIDefine_h__
