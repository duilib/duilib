#ifndef UIDefine_h__
#define UIDefine_h__
/*/////////////////////////////////////////////////////////////
//
// �ļ�����	:	UIDefine.h
// ������	: 	daviyang35@gmail.com
// ����ʱ��	:	2014-11-07 17:01:26
// ˵��		:	
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
// �궨��
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
// ö��

// INotify�ӿ�ʹ�õ�Type
typedef enum _enUINOTIFY
{
	UINOTIFY__FIRST,

	// �����¼�
	UINOTIFY_WindowCreate,
	UINOTIFY_WindowResize,
	UINOTIFY_WindowPaint,
	UINOTIFY_WindowPosChanged,
	// ͨ���¼�
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

	// RichEdit
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
	//�ȼ�
	UINOTIFY_HOTKEY, 
	// ���ڻ�ԭʱ����Ϣ

	UINOTIFY__LAST = UINOTIFY__FIRST + 1000,
	UINOTIFY__CUSTOM,	/// �����Զ���ؼ���Ϣ�����.
	UINOTIFY__CUSTOM_LAST = UINOTIFY__CUSTOM + 1000,
	UINOTIFY_USER,
	// Ƥ���¼�

}UINOTIFY;

// Control�ڲ����ݵ�EventHandler
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
	UIProperty_BackColor,			// ����ɫ
	UIProperty_Back_Color1 = UIProperty_BackColor,
	UIProperty_Back_Color2,		// ����ɫ
	UIProperty_Back_Color3,		// ����ɫ
	UIProperty_Back_Image,		// ����ͼ
	UIProperty_Fore_Image,		// ǰ��ͼ
	UIProperty_Text_String,		// ����
	UIProperty_Text_Font,			// ������ʽ
	UIProperty_Text_Color,		// ������ɫ
	UIProperty_Border_Color,	// �߿���ɫ
	UIProperty_Border_Rect,		// �߿����
	UIProperty_Border_Wdith,	// �߿���
	UIProperty_Border_Style,		// �߿���ʽ
}UIProperty;

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
class CEditUI;

//////////////////////////////////////////////////////////////////////////
// �ṹ��

typedef struct _stTFontInfo
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
	CControlUI* pFocus;	// ��ǰ����ؼ�
	CControlUI* pLast;		
	bool bForward;			// true ��һ���ؼ���false ��һ���ؼ�
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
	DWORD dwTimestamp;	// ʱ���
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
// �ӿ�

// Listener interface
class INotifyUI
{
public:
	// ����true�����¼��Ѿ��������ټ���Ͷ��
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
// λ��־�궨��

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
#define UISTATE_Double			0x00000200		// 512 DoubleClick Mask

//////////////////////////////////////////////////////////////////////////
// ����

#define UI_DECLARE_DYNCREATE() \
	static CControlUI* CreateObject()\

#define UI_IMPLEMENT_DYNCREATE(ClassName)\
	CControlUI* ClassName::CreateObject()\
{ return new ClassName; }\

#define UI_REGISTER_DYNCREATE(ControlName, ClassName)\
	CUIEngine::GetInstance()->RegisterControl(ControlName, &ClassName::CreateObject)

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

#endif // UIDefine_h__
