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


class CControlUI;
class IUIRender;
class CObjectUI;
class CImageObject;
class CFontObject;
class TemplateObject;

#define UI_DECLARE_DYNCREATE() \
	static CControlUI* CreateObject()\

#define UI_IMPLEMENT_DYNCREATE(ClassName)\
	CControlUI* ClassName::CreateObject()\
{ return new ClassName; }\

#define UI_REGISTER_DYNCREATE(ControlName, ClassName)\
	CUIEngine::GetInstance()->RegisterControl(ControlName, &ClassName::CreateObject)\

typedef CControlUI* (_cdecl *PROCCONTROLCREATE)();
typedef std::map<CDuiString,CDuiString> StringMap;
typedef StringMap AttributeMap;
typedef std::map<CDuiString,TemplateObject*> TemplateMap;
typedef std::map<CDuiString,CImageObject*>	ImagePoolMap;
typedef std::vector<CFontObject*>		FontPoolVector;

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

typedef struct _stTImageInfo
{
	HBITMAP hBitmap;
	int nX;
	int nY;
	int delay;
	bool alphaChannel;
	CDuiString strResType;
	DWORD dwMask;
} TImageInfo;

typedef struct _stFindTabInfo
{
	CControlUI* pFocus;
	CControlUI* pLast;
	bool bForward;
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

// Structure for notifications from the system
// to the control implementation.
typedef struct _stTEventUI
{
	DWORD dwType;
	CControlUI* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	TCHAR chKey;
	WORD wKeyState;
	WPARAM wParam;
	LPARAM lParam;

	_stTEventUI()
		: dwType(0)
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
	DWORD dwType;
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

// Listener interface
class INotifyUI
{
public:
	virtual void Notify(TNotifyUI *pMsg) = 0;
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
};

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
	// 编辑框事件
	UINOTIFY_EN_CHANGE,//change
	UINOTIFY_EN_IMESTART,//imestart
	UINOTIFY_EN_IMEEND,//imeend
	UINOTIFY_EN_IMEING,//imeing
	UINOTIFY_EN_LIMIT,//limit
	UINOTIFY_EN_KEYDOWN,  // keydouwn add by zhangxin 2011.9.7
	UINOTIFY_EN_LISTCLICK,//listclick
	UINOTIFY_EN_LISTRETURN,//listreturn
	// 树形控件事件
	UINOTIFY_TVN_ITEMEXPENDED,
	UINOTIFY_TVN_ITEMEXPENDING,
	UINOTIFY_TVN_BEGINDRAG,
	UINOTIFY_TVN_ENDDRAG,
	UINOTIFY_TVN_CHECK,
	// Slider事件poschange   selchange beginchange endchange
	UINOTIFY_TRBN_POSCHANGE,
	UINOTIFY_TRBN_SELCHANGE,
	UINOTIFY_TRBN_BEGINCHANGE,
	UINOTIFY_TRBN_ENDCHANGE,
	// richedit控件事件 
	UINOTIFY_RE_DBCLICK_IMAGE,
	UINOTIFY_RE_CLICK_IMAGE_THUMBNAIL_BTN,
	UINOTIFY_RE_CLICK_LINK,
	// ActiveX控件
	UINOTIFY_ACTIVEX_SHOW,
	UINOTIFY_ACTIVEX_NAVIGATE_BEFORE,
	UINOTIFY_ACTIVEX_NAVIGATE_AFTER,
	UINOTIFY_ACTIVEX_LODING,
	UINOTIFY_ACTIVEX_LODE_DONE,
	UINOTIFY_ACTIVEX_LOAD_ERROR,
	UINOTIFY_ACTIVEX_CLOSE,
	// Animation
	UINOTIFY_ANIMATION_END,
	// ComboBox
	UINOTIFY_CBN_BEFOREDROPDOWN,
	UINOTIFY_CBN_DROPDOWN,
	UINOTIFY_CBN_CHECK,
	// Tab
	UINOTIFY_TAB_ADD,
	UINOTIFY_TAB_CLOSE,
	UINOTIFY_TAB_Ext,
	UINOTIFY_TAB_DBCLICK,
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

// Flags used for controlling the paint
typedef enum _enUISTATE
{
	// 普通控件
	UISTATE_Normal =0,		// Normal
	UISTATE_Hover,				// Hover
	UISTATE_Pushed,				// Pushed
	UISTATE_Focused,			// Focused
	UISTATE_Selected,			// Selected
	UISTATE_Disabled,			// Disabled
	UISTATE_Control = UISTATE_Disabled,

	// Check&Radio 选中时
	//UISTATE_CheckNormal,
	//UISTATE_CheckHover,
	//UISTATE_CheckPushed,
	//UISTATE_CheckFocused,
	//UISTATE_CheckDisabled,
	//UISTATE_CheckRadio = UISTATE_CheckDisabled,
	UISTATE_Checked,			// 选中标志位

	UISTATE_ReadOnly,
	UISTATE_Captured,

	UISTATE_ALL,					// 特殊状态，表示设置属性到所有状态
}UISTATE;

#endif // UIDefine_h__
