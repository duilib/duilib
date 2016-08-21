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
#include "Utils/BufferPtr.h"

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

namespace DuiLib
{

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

	// 常用 Buffer 类型的 typedef
	typedef CBufferPtrT<char>						CCharBufferPtr;
	typedef CBufferPtrT<wchar_t>				CWCharBufferPtr;
	typedef CBufferPtrT<unsigned char>		CByteBufferPtr;
	typedef CByteBufferPtr							CBufferPtr;

#ifdef _UNICODE
	typedef CWCharBufferPtr						CTCharBufferPtr;
#else
	typedef CCharBufferPtr							CTCharBufferPtr;
#endif

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
#ifndef ARGB
	typedef DWORD ARGB;
#endif // !ARGB
#ifndef COLOR_ARGB
#define COLOR_ARGB(a,r,g,b) \
	((ARGB)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#endif // !COLOR_ARGB
#ifndef COLOR_RGBA
#define COLOR_RGBA(r,g,b,a) ARGB(a,r,g,b)
#endif // !COLOR_RGBA
#ifndef COLOR_XRGB
#define COLOR_XRGB(r,g,b)   ARGB(0xff,r,g,b)
#endif // !COLOR_XRGB

#ifndef GetWindowOwner
#define     GetWindowOwner(hwnd)    GetWindow(hwnd, GW_OWNER)
#endif // !GetWindowOwner
#ifndef GetWindowInstance
#define     GetWindowInstance(hwnd) ((HMODULE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE))
#endif // !GetWindowInstance
	#ifndef GetWindowStyle
#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#endif // !GetWindowStyle
#ifndef GetWindowExStyle
#define     GetWindowExStyle(hwnd)  ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))
#endif // !GetWindowExStyle
#ifndef SubclassWindow
#define     SubclassWindow(hwnd, lpfn)       \
	((WNDPROC)SetWindowLongPtr((hwnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(lpfn)))
#endif // !SubclassWindow

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)

//////////////////////////////////////////////////////////////////////////
// 枚举

	enum UILIB_RESOURCETYPE
	{
		UILIB_FILE=1,
		UILIB_ZIP,
		UILIB_RESOURCE,		// 不再支持
		UILIB_ZIPRESOURCE,
	};


// INotify接口使用的Type
typedef enum _enUINOTIFY
{
	UINOTIFY__FIRST,

	// 窗口事件
	UINOTIFY_WindowCreate,
	UINOTIFY_WindowResize,
	UINOTIFY_WindowPaint,
	UINOTIFY_WindowPosChanging,
	UINOTIFY_WindowPosChanged,
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
	UINOTIFY_VALUECHANGED,
	UINOTIFY_SCROLL,
	UINOTIFY_DROPFILES,//dropfiles
	UINOTIFY_ITEM_SELECTED,
	UINOTIFY_DROPDOWN,
	UINOTIFY_EDIT_CHANGE,
	UINOTIFY_EDIT_RETURN,
	UINOTIFY_RICHEDIT_RETURN,
	UINOTIFY_HEADER_CLICK,
	UINOTIFY_ITEM_ACTIVATE,
	UINOTIFY_ITEM_LCLICK,
	UINOTIFY_ITEM_RCLICK,
	UINOTIFY_ITEM_LDBCLICK,
	UINOTIFY_ITEM_RDBCLICK,
	UINOTIFY_ITEM_Enter,
	UINOTIFY_ITEM_Leave,

	//
	UINOTIFY_ACTIVEX_SHOW,
	UINOTIFY_ACTIVEX_LINK,
	// Animation
	UINOTIFY_ANIMATION_END,
	// ComboBox
	UINOTIFY_CBN_BEFOREDROPDOWN,
	UINOTIFY_CBN_DROPDOWN,
	UINOTIFY_CBN_CHECK,
	// Tab
	UINOTIFY_TAB_SELECTED,
	UINOTIFY_TAB_ADD,
	UINOTIFY_TAB_CLOSE,
	UINOTIFY_TAB_Ext,
	UINOTIFY_TAB_DBCLICK,
	//热键
	UINOTIFY_HOTKEY, 
	// 窗口还原时的消息

	UINOTIFY__LAST = UINOTIFY__FIRST + 1000,
	UINOTIFY__CUSTOM,	/// 其他自定义控件消息的起点.
    UINOTIFY__CUSTOM_UpdateDeviceList,  //并机检测更新列表
    UINOTIFY__CUSTOM_UpdateWirelessList,  //无线网络配置页面初始化完成后加载数据消息
	UINOTIFY__CUSTOM_LAST = UINOTIFY__CUSTOM + 1000,
	UINOTIFY_USER,
	// 皮肤事件

}UINOTIFY;

// Control内部传递的EventHandler
typedef enum _UIEVENT
{
	UIEVENT__FIRST = 0,
	UIEVENT_CREATE,
	UIEVENT_INIT,
	UIEVENT_DESTROY,
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

//typedef enum _enFontStyle
//{
//	FontStyleRegular		= 0,		// 常规
//	FontStyleBold			= 1,		// 粗体
//	FontStyleItalic			= 2,		// 斜体
//	FontStyleBoldItalic	= 3,		// 粗体&斜体
//	FontStyleUnderline	= 4,		// 下划线
//	FontStyleStrikeout	= 8,		// 删除线
//}FontStyle;

class CControlUI;
class CButtonUI;
class IUIRender;
class CObjectUI;
class ImageObject;
class FontObject;
class TemplateObject;
class CScrollBarUI;
class CButtonUI;
class CContainerUI;
class CWindowUI;
class CCurveObject;
class Point2D;

//////////////////////////////////////////////////////////////////////////
// 结构体

typedef struct UILIB_API _stTFontInfo
{
	HFONT hFont;
	CDuiString strFaceName;
	int iSize;
	bool bBold;
	bool bUnderline;
	bool bItalic;
	TEXTMETRIC tm;

	_stTFontInfo()
		: hFont(NULL)
		, iSize(0)
		, bBold(false)
		, bUnderline(false)
		, bItalic(false)
	{
		ZeroMemory(&tm,sizeof(TEXTMETRIC));
	}
} TFontInfo;

typedef struct UILIB_API _stTImageData
{
	HBITMAP hBitmap;
	int nX;
	int nY;
	int delay;
	bool alphaChannel;
	CDuiString strResType;
	DWORD dwMask;
	UINT nCounter;
} TImageData;

typedef struct UILIB_API tagTPercentInfo
{
	double left;
	double top;
	double right;
	double bottom;
} TPercentInfo;

typedef struct UILIB_API _stSkinRDB
{
	HANDLE	hZip;
	LPBYTE		lpByte;
	DWORD	dwSize;
	_stSkinRDB()
		: lpByte(NULL)
		, hZip(NULL)
		, dwSize(0)
	{
	}
}SkinRDB,*PSkinRDB;

typedef struct UILIB_API _stFindTabInfo
{
	CControlUI* pFocus;	// 当前焦点控件
	CControlUI* pLast;		
	bool bForward;			// true 下一个控件，false 上一个控件
	bool bNextIsIt;
} FindTabInfo;

typedef struct UILIB_API _stFindShortCut
{
	TCHAR ch;
	bool bPickNext;
} FindShortCut;

typedef struct UILIB_API _stTimerInfo
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
typedef struct UILIB_API _stTEventUI
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
typedef struct UILIB_API _stTNotifyUI 
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

class IResDecoder
{
public:
	virtual void ResDecoder(LPVOID lpData,DWORD dwResLength) =0;
};

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

class IIdleFilter
{
public:
	virtual void OnIdle() =0;
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
#define UIFIND_UPDATETEST 0x00000008
#define UIFIND_TOP_FIRST     0x00000010
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
#define UISTATE_Double			0x00000200		// 512 DoubleClick Mask

//////////////////////////////////////////////////////////////////////////
// Engine
//////////////////////////////////////////////////////////////////////////
// 2015-4-24 扩展UI资源管理
typedef CControlUI* (_cdecl *PROCCONTROLCREATE)();
typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);

class UILIB_API IUIEngine
{
public:
	virtual DWORD GetEngineVersion() =0;

	// 1、初始化界面库
	virtual void Init() = 0;
	// 2、创建窗口，
	//new CWindowUI()
	// 3、消息循环
	virtual int  MessageLoop() = 0;
	// 4、销毁界面库
	virtual void Uninit() = 0;

	// 设置进程资源句柄
	virtual void SetInstanceHandle(HINSTANCE hInstance) = 0;
	virtual HINSTANCE GetInstanceHandle()= 0;

	// 获取消息泵窗口句柄
	virtual HWND GetPumpWnd() =0;

	// 消息泵接口
	virtual bool TranslateMessage(const LPMSG pMsg) =0;
	virtual bool PumpMessage() =0;

	// 注册/反注册自绘窗口
	virtual void SkinWindow(CWindowUI* pWindow) =0;
	virtual void UnSkinWindow(CWindowUI* pWindow) =0;

	// 空闲处理接口
	virtual void AddIdleFilter(IIdleFilter* pFilter) =0;
	virtual void RemoveIdleFilter(IIdleFilter* pFilter) =0;

	// 自绘窗口查找
	virtual int GetWindowCount() = 0;
	// 根据窗口句柄查找自绘窗口
	virtual CWindowUI* GetWindow(HWND hWnd) =0;
	// 根据窗口名称查找自绘窗口
	virtual CWindowUI* GetWindow(LPCTSTR lpszName) =0;
	// 根据窗口索引查找自绘窗口
	virtual CWindowUI* GetWindow(UINT nIndex) =0;
	// 获取当前焦点窗口
	virtual CWindowUI* GetKeyWindow() =0;
	// 设置焦点窗口
	virtual CWindowUI* SetKeyWindow(CWindowUI* pKeyWindow) =0;

	// 控件注册与创建接口
	virtual void RegisterControl(LPCTSTR lpszType, PROCCONTROLCREATE pFn_ControlCreate) =0;
	virtual void UnregisterControl(LPCTSTR lpszType) =0;
	virtual CControlUI* CreateControl(LPCTSTR lpszType) =0;

	// 辅助函数
	virtual LPCTSTR GetAppDir() =0;
	virtual LPCTSTR GetWorkDir() =0;
	virtual BOOL SetWorkDir(LPCTSTR lpszWorkDir) =0;
};

class UILIB_API IResEngine
{
public:
	// 设置显示语言
	virtual void SetLanguage(LPCTSTR lpszLanguage) =0;
	virtual LPCTSTR GetLanguage() =0;

	// 设置默认字体
	virtual void SetDefaultFontID(LPCTSTR lpszDefaultFontID) =0;

	// 从文件系统、rdb压缩包、PE资源中加载皮肤包
	// Note:皮肤包需要满足特定格式条件
	virtual void LoadResFromPath(LPCTSTR lpszResPath,bool bDefaultSkin= false) =0;
	virtual void LoadResFromRDB(LPCTSTR lpszRDBPath,bool bDefaultSkin= false) =0;
	virtual void LoadResFromRes(UINT nResID,LPCTSTR lpszResType,bool bDefaultSkin= false) =0;

	// 释放模块资源
	// Note:包括已缓存的资源
	virtual void FreeResPackage(LPCTSTR lpszPackageName) =0;

	// 获得指定皮肤包的路径
	virtual LPCTSTR GetResDir(LPCTSTR lpszPackageName) =0;

	// 图片相关
	virtual TImageData* GetImage(LPCTSTR lpszImagePath,DWORD dwMask,bool bCached = true ) =0;
	virtual TImageData* GetImage(LPCTSTR lpszImageName,bool bCached = true)=0;

	// 字体相关
	virtual void  AddFont(FontObject* pFontObject) = 0;
	virtual FontObject* GetFont(LPCTSTR lpszFontID) = 0;
	virtual FontObject* GetDefaultFont() = 0;

	virtual LPCTSTR GetFont(LPCTSTR pstrFace, int nSize,
		bool bBold=false, bool bUnderline=false, bool bItalic=false,bool bStrikeout=false) =0;

	virtual LPCTSTR GetI18NString(LPCTSTR lpszItem) =0;

	// 根据资源URI查找文件并读取
	virtual bool GetRes(CBufferPtr &dataBuffer,LPCTSTR lpszRelative) =0;

	// 默认皮肤模板
	virtual bool ApplyResTemplate(CControlUI* pControl,LPCTSTR lpszTemplateID) =0;

	virtual void GetHSL(short* H, short* S, short* L) =0;
	// H:0~360, S:0~200, L:0~200 
	virtual void SetHSL(bool bUseHSL, short H, short S, short L) =0;

	virtual DWORD GetDefaultColor(LPCTSTR lpszID) =0;

	// 通过ID获取曲线对象
	virtual CCurveObject* GetCurveObj(LPCTSTR lpszID) =0;
};

EXTERN_C
{
	UILIB_API IUIEngine*		GetUIEngine();
	UILIB_API IResEngine*		GetResEngine();
	UILIB_API LPCTSTR			GetI18NString(LPCTSTR lpszItem);
	UILIB_API CControlUI*		CreateControl(LPCTSTR lpszControlName);
};

#define LOAD_STRING GetI18NString

//////////////////////////////////////////////////////////////////////////
// 辅助

#define UI_DECLARE_DYNCREATE() \
	static CControlUI* CreateObject()\

#define UI_IMPLEMENT_DYNCREATE(ClassName)\
	CControlUI* ClassName::CreateObject()\
{ return new ClassName; }\

#define UI_REGISTER_DYNCREATE(ControlName, ClassName)\
	GetUIEngine()->RegisterControl(ControlName, &ClassName::CreateObject)

#define UI_UNREGISTER_DYNCREATE(ControlName)\
	GetUIEngine()->UnregisterControl(ControlName)

typedef std::vector<CDuiString>								VecString;
typedef std::map<CDuiString,CDuiString>					StringMap;
typedef std::map<CDuiString,TemplateObject*>		TemplateMap;
typedef std::map<CDuiString,ImageObject*>			ImagePoolMap;
typedef std::vector<FontObject*>								FontPoolVector;
typedef std::multimap<DWORD,Property>				UIStatePropertyMap;
typedef std::vector<Point2D>									Point2DVector;
typedef StringMap AttributeMap;

typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

#ifdef _DEBUG
static const TCHAR* DirectUIMessage = _T("WM_DIRECTUI_MESSAGE");
#else
static const TCHAR* DirectUIMessage = _T("{87FA3361-8884-4137-83C1-1EA38FCE1838}");
#endif

static DWORD WM_DIRECTUI_MESSAGE	= ::RegisterWindowMessage(DirectUIMessage);

static UINT MapKeyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 )
		uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 )
		uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 )
		uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 )
		uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 )
		uState |= MK_ALT;
	return uState;
}

static bool GetRealWindowRect(HWND hWnd,LPRECT lpRect)
{
	bool result = false;
	WINDOWPLACEMENT winPT;

	if ( ::IsWindowVisible(hWnd) )
	{
		result = ::GetWindowRect(hWnd, lpRect) != 0;
	}
	else
	{
		::GetWindowPlacement(hWnd, &winPT);
		lpRect->top = winPT.rcNormalPosition.top;
		lpRect->right = winPT.rcNormalPosition.right;
		lpRect->left = winPT.rcNormalPosition.left;
		lpRect->bottom = winPT.rcNormalPosition.bottom;
		result = IsRectEmpty(&winPT.rcNormalPosition) != 0;
	}
	return result;
}

}

#endif // UIDefine_h__
