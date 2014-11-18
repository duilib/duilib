#include "stdafx.h"
#include "ControlUI.h"

UI_IMPLEMENT_DYNCREATE(CControlUI);

CControlUI::CControlUI(void)
	: m_pManager(NULL)
	, m_pParent(NULL)
	, m_bIsVisible(true)
	, m_bIsInternVisible(true)	// Container才处理
	, m_bIsEnabled(true)
	, m_bIsMouseEnabled(true)
	, m_bIsKeyboardEnabled(true)
	, m_bIsFocused(true)
	, m_bIsFloat(true)					// 推荐使用浮动布局
	, m_bIsSetPos(false)
	, m_nTooltipWidth(300)
	, m_dwState(UISTATE_Normal)
	, m_dwBackColor(0xFF000000)
{
}


CControlUI::~CControlUI(void)
{
	 if( m_pManager != NULL )
		 m_pManager->ReapObjects(this);
}

void CControlUI::SetManager(CWindowUI* pManager, CControlUI* pParent)
{
	m_pManager = pManager;
	m_pParent = pParent;
}

CWindowUI* CControlUI::GetManager()
{
	return m_pManager;
}

CControlUI* CControlUI::GetParent()
{
	return m_pParent;
}

void CControlUI::SendNotify(DWORD dwType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	if ( m_pManager )
	{
		m_pManager->SendNotify(this,dwType,wParam,lParam,false);
	}
}

void CControlUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if ( _tcscmp(lpszName, _T("image")) == 0 )
	{
		StringMap attributeMap;
		CDuiStringOperation::parseAttributeString(lpszValue,attributeMap);
		if ( attributeMap.size() > 0)
		{

		}
	}
	else if( _tcscmp(lpszName, _T("pos")) == 0 ) 
	{
		RECT rcPos = { 0 };
		CDuiCodeOperation::StringToRect(lpszValue,&rcPos);
		SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
		SetFixedXY(szXY);
		SetFixedWidth(rcPos.right - rcPos.left);
		SetFixedHeight(rcPos.bottom - rcPos.top);
	}
	else if( _tcscmp(lpszName, _T("relativepos")) == 0 )
	{
		SIZE szMove,szZoom;
		CDuiCodeOperation::StringToRect(lpszValue,&szMove.cx,&szMove.cy,&szZoom.cx,&szZoom.cy);
		//SetRelativePos(szMove,szZoom);
	}
	else if( _tcscmp(lpszName, _T("padding")) == 0 )
	{
		RECT rcPadding = { 0 };
		CDuiCodeOperation::StringToRect(lpszValue,&rcPadding);
		SetPadding(rcPadding);
	}
	else if( _tcscmp(lpszName, _T("bkcolor")) == 0 || _tcscmp(lpszName, _T("bkcolor1")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		SetBkColor(clrColor);
	}
	else if( _tcscmp(lpszName, _T("bkcolor2")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		//SetBkColor2(clrColor);
	}
	else if( _tcscmp(lpszName, _T("bkcolor3")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		//SetBkColor3(clrColor);
	}
	else if( _tcscmp(lpszName, _T("bordercolor")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		//SetBorderColor(clrColor);
	}
	else if( _tcscmp(lpszName, _T("focusbordercolor")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		//SetFocusBorderColor(clrColor);
	}
	//else if( _tcscmp(lpszName, _T("colorhsl")) == 0 )
	//	SetColorHSL(_tcscmp(lpszValue, _T("true")) == 0);
	//else if( _tcscmp(lpszName, _T("bordersize")) == 0 )
	//{
	//	CDuiString nValue = lpszValue;
	//	if(nValue.Find(',') < 0)
	//	{
	//		SetBorderSize(_ttoi(lpszValue));
	//		RECT rcPadding = {0};
	//		SetBorderSize(rcPadding);
	//	}
	//	else
	//	{
	//		RECT rcPadding = { 0 };
	//		LPTSTR pstr = NULL;
	//		rcPadding.left = _tcstol(lpszValue, &pstr, 10);  ASSERT(pstr);
	//		rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
	//		rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
	//		rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
	//		SetBorderSize(rcPadding);
	//	}
	//}
	//else if( _tcscmp(lpszName, _T("leftbordersize")) == 0 ) SetLeftBorderSize(_ttoi(lpszValue));
	//else if( _tcscmp(lpszName, _T("topbordersize")) == 0 ) SetTopBorderSize(_ttoi(lpszValue));
	//else if( _tcscmp(lpszName, _T("rightbordersize")) == 0 ) SetRightBorderSize(_ttoi(lpszValue));
	//else if( _tcscmp(lpszName, _T("bottombordersize")) == 0 ) SetBottomBorderSize(_ttoi(lpszValue));
	//else if( _tcscmp(lpszName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("borderround")) == 0 ) {
		SIZE cxyRound = { 0 };
		CDuiCodeOperation::StringToSize(lpszValue,&cxyRound);
		//SetBorderRound(cxyRound);
	}
	else if( _tcscmp(lpszName, _T("bkimage")) == 0 ) SetBkImage(lpszValue);
	else if( _tcscmp(lpszName, _T("width")) == 0 ) SetFixedWidth(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("height")) == 0 ) SetFixedHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("minwidth")) == 0 ) SetMinWidth(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("minheight")) == 0 ) SetMinHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("name")) == 0 ) SetName(lpszValue);
	else if( _tcscmp(lpszName, _T("text")) == 0 ) SetText(lpszValue);
	else if( _tcscmp(lpszName, _T("tooltip")) == 0 ) SetToolTip(lpszValue);
	//else if( _tcscmp(lpszName, _T("userdata")) == 0 ) SetUserData(lpszValue);
	else if( _tcscmp(lpszName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("mouse")) == 0 ) SetMouseEnabled(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("visible")) == 0 ) SetVisible(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("float")) == 0 ) SetFloat(_tcscmp(lpszValue, _T("true")) == 0);
	//else if( _tcscmp(lpszName, _T("shortcut")) == 0 ) SetShortcut(lpszValue[0]);
	else if( _tcscmp(lpszName, _T("menu")) == 0 ) SetContextMenuUsed(_tcscmp(lpszValue, _T("true")) == 0);
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() )
		return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() )
		return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bIsMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) )
		return NULL;
	return Proc(this, pData);
}

bool CControlUI::EventHandler(TEventUI& event)
{   // 处理CWindowUI转发的窗口消息事件

	switch (event.dwType)
	{
	case UIEVENT_SETCURSOR:
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		}
		break;
	case UIEVENT_SETFOCUS:
		{
			m_bIsFocused = true;
			Invalidate();
		}
		break;
	case UIEVENT_KILLFOCUS:
		{
			m_bIsFocused = false;
			Invalidate();
		}
		break;
	case UIEVENT_TIMER:
		{
			m_pManager->SendNotify(this, UINOTIFY_TIMER, event.wParam, event.lParam);
		}
		break;
	case UIEVENT_CONTEXTMENU:
		{
			if( IsContextMenuUsed() )
			{
				m_pManager->SendNotify(this, UINOTIFY_CONTEXTMENU, event.wParam, event.lParam);
			}
		}
		break;
	}

	// 父控件冒泡
	if( m_pParent != NULL )
	{
		m_pParent->EventHandler(event);
	}

	//this->Invoke(event);
	return true;
}

//void CControlUI::Invoke(TEventUI& event)
//{// 根据事件处理控件内部状态
//
//	if ( m_pParent != NULL )
//	{
//		m_pParent->Invoke(event);
//	}
//}

RECT CControlUI::GetPosition()
{
	return m_rcItem;
}

void CControlUI::SetPosition(LPCRECT rc)
{
	CDuiRect rect(rc);

	if( rect.right < rect.left )
		rect.right = rect.left;
	if( rect.bottom < rect.top )
		rect.bottom = rect.top;

	CDuiRect invalidateRc = m_rcItem;
	if( ::IsRectEmpty(&invalidateRc) )
		invalidateRc = rc;

	m_rcItem = rc;
	if( m_pManager == NULL )
		return;

	if( !m_bIsSetPos )
	{
		m_bIsSetPos = true;
		//if( OnSize )
		//	OnSize(this);
		m_bIsSetPos = false;
	}

	if( m_bIsFloat ) {
		CControlUI* pParent = GetParent();
		if( pParent != NULL )
		{
			RECT rcParentPos = pParent->GetPosition();

			if( m_cXY.cx >= 0 ) 
				m_cXY.cx = m_rcItem.left - rcParentPos.left;
			else 
				m_cXY.cx = m_rcItem.right - rcParentPos.right;

			if( m_cXY.cy >= 0 ) 
				m_cXY.cy = m_rcItem.top - rcParentPos.top;
			else 
				m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;

			m_cxyFixed.cx = m_rcItem.right - m_rcItem.left;
			m_cxyFixed.cy = m_rcItem.bottom - m_rcItem.top;
		}
	}

	m_bIsUpdateNeeded = false;
	invalidateRc.Join(m_rcItem);

	CControlUI* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->GetParent() )
	{
		rcTemp = invalidateRc;
		rcParent = pParent->GetPosition();
		if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}
	m_pManager->Invalidate(invalidateRc);
}

RECT CControlUI::GetInterRect()
{
	return m_rcInset;
}

void CControlUI::SetInset(LPRECT rc)
{
	if ( rc != NULL)
		m_rcInset = *rc;
}

RECT CControlUI::GetInset()
{
	return m_rcInset;
}

bool CControlUI::IsVisible() const
{   // 对于普通控件，m_bIsInternVisible始终是true
	// Layout控件会根据情况设置m_bIsInternVisible状态
	return m_bIsVisible && m_bIsInternVisible;
}

void CControlUI::SetVisible(bool bVisible /*= true*/)
{
	if ( m_bIsVisible == bVisible)
		return;

	bool visible = IsVisible();
	m_bIsVisible = visible;
	if ( m_bIsFocused )
		m_bIsFocused = false;
	if ( bVisible && m_pManager && m_pManager->GetFocus() == this )
	{
		m_pManager->SetFocus(NULL);
	}
	if ( IsVisible() != visible )
	{
		NeedParentUpdate();
	}
	m_bIsVisible = bVisible;
}

void CControlUI::NeedParentUpdate()
{
	if ( GetParent())
	{
		GetParent()->NeedUpdate();
		GetParent()->Invalidate();
	}
	else
	{
		this->NeedUpdate();
	}

	if ( m_pManager != NULL )
		m_pManager->NeedUpdate();
}

void CControlUI::Invalidate()
{
	if( !IsVisible() )
		return;

	RECT invalidateRect = m_rcItem;

	CControlUI* pParent = this;
	RECT rcTemp;
	RECT rcParent;
	while( pParent = pParent->GetParent() )
	{
		rcTemp = invalidateRect;
		rcParent = pParent->GetPosition();
		if( !::IntersectRect(&invalidateRect, &rcTemp, &rcParent) ) 
		{
			return;
		}
	}

	if( m_pManager != NULL )
		m_pManager->Invalidate(invalidateRect);
}

void CControlUI::NeedUpdate()
{
	if ( ! IsVisible() )
		return;	// 不可见控件，不重绘
	m_bIsUpdateNeeded = true;
	Invalidate();
	if ( m_pManager != NULL)
		m_pManager->NeedUpdate();
}

bool CControlUI::IsUpdateNeeded() const
{
	return m_bIsUpdateNeeded;
}

UINT CControlUI::GetControlFlags() const
{
	return 0;
}

void CControlUI::SetInternVisible(bool bVisible /*= true*/)
{   // 普通控件不会操作，Layout才会处理
	m_bIsInternVisible = bVisible;
	if ( !bVisible && m_pManager && m_pManager->GetFocus() == this )
	{
		m_pManager->SetFocus(NULL);
	}
}

bool CControlUI::IsEnabled() const
{
	return m_bIsEnabled;
}

void CControlUI::SetEnabled(bool bEnable /*= true*/)
{
	if ( m_bIsEnabled == bEnable)
		return;

	m_bIsEnabled = bEnable;
	Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
	return m_bIsMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnable /*= true*/)
{
	m_bIsMouseEnabled = bEnable;
}

bool CControlUI::IsKeyboardEnabled() const
{
	return m_bIsKeyboardEnabled;
}

void CControlUI::SetKeyboardEnabled(bool bEnable /*= true*/)
{
	m_bIsKeyboardEnabled = bEnable;
}

bool CControlUI::IsFocused() const
{
	return m_bIsFocused;
}

void CControlUI::SetFocus()
{
	if ( m_pManager != NULL)
	{
		m_pManager->SetFocus(this);
	}
}

bool CControlUI::IsFloat() const
{
	return m_bIsFloat;
}

void CControlUI::SetFloat(bool bFloat /*= true*/)
{
	if ( m_bIsFloat == bFloat)
		return;

	m_bIsFloat = bFloat;
	NeedParentUpdate();
}

LPCTSTR CControlUI::GetClass() const
{
	return _T("Control");
}

LPVOID CControlUI::GetInterface(LPCTSTR lpszClass)
{
	if( _tcscmp(lpszClass, _T("Control")) == 0 )
		return this;
	else
		return CObjectUI::GetInterface(lpszClass);
}

LPCTSTR CControlUI::GetToolTip() const
{
	return m_strToolTip.c_str();
}

void CControlUI::SetToolTip(LPCTSTR lpszText)
{
	m_strToolTip = lpszText;
}

int CControlUI::GetToolTipWidth(void) const
{
	return m_nTooltipWidth;
}

void CControlUI::SetToolTipWidth(int nWidth)
{
	m_nTooltipWidth = nWidth;
}

int CControlUI::GetWidth() const
{
	return m_rcItem.GetWidth();
}

int CControlUI::GetHeight() const
{
	return m_rcItem.GetHeight();
}

int CControlUI::GetX() const
{
	 return m_rcItem.left;
}

int CControlUI::GetY() const
{
	 return m_rcItem.top;
}

RECT CControlUI::GetPadding() const
{
	return m_rcPadding;
}

void CControlUI::SetPadding(RECT rcPadding)
{
	m_rcPadding = rcPadding;
	NeedParentUpdate();
}

SIZE CControlUI::GetFixedXY() const
{
	return m_cXY;
}

void CControlUI::SetFixedXY(SIZE szXY)
{
	m_cXY = szXY;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

int CControlUI::GetFixedWidth() const
{
	return m_cxyFixed.cx;
}

void CControlUI::SetFixedWidth(int cx)
{

	if( cx < 0 ) return; 
	m_cxyFixed.cx = cx;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

int CControlUI::GetFixedHeight() const
{
	return m_cxyFixed.cy;
}

void CControlUI::SetFixedHeight(int cy)
{
	if( cy < 0 ) return; 
	m_cxyFixed.cy = cy;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

int CControlUI::GetMinWidth() const
{
	 return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
	if( m_cxyMin.cx == cx )
		return;

	if( cx < 0 )
		return; 
	m_cxyMin.cx = cx;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

int CControlUI::GetMaxWidth() const
{
	 return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
	if( m_cxyMax.cx == cx )
		return;

	if( cx < 0 )
		return; 
	m_cxyMax.cx = cx;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

int CControlUI::GetMinHeight() const
{
	 return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
	if( m_cxyMin.cy == cy )
		return;

	if( cy < 0 )
		return; 
	m_cxyMin.cy = cy;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

int CControlUI::GetMaxHeight() const
{
	 return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
	if( m_cxyMax.cy == cy )
		return;

	if( cy < 0 )
		return; 
	m_cxyMax.cy = cy;
	if( !m_bIsFloat )
		NeedParentUpdate();
	else
		NeedUpdate();
}

DWORD CControlUI::GetBkColor() const
{
	 return m_dwBackColor;
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
	if( m_dwBackColor == dwBackColor ) return;

	m_dwBackColor = dwBackColor;
	Invalidate();
}

LPCTSTR CControlUI::GetBkImage()
{
	return m_sBkImage.c_str();
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
	if( m_sBkImage == pStrImage )
		return;

	m_sBkImage = pStrImage;
	Invalidate();
}

LPCTSTR CControlUI::GetText() const
{
	return m_sText.c_str();
}

void CControlUI::SetText(LPCTSTR pstrText)
{
	if( m_sText == pstrText )
		return;

	m_sText = pstrText;
	Invalidate();
}

bool CControlUI::IsContextMenuUsed() const
{
	return m_bIsMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
	m_bIsMenuUsed = bMenuUsed;
}

void CControlUI::Render(IUIRender* pRender,LPCRECT pRcPaint)
{  // 子控件绘图
	// 在1.X版中，对应CControlUI::DoPaint，该函数按顺序调用5个虚函数，分别是
	// PaintBkColor、PaintBkImage、PaintStatusImage、PaintText、PaintBorder
	// 所有控件重写这5个函数实现绘图

	// 刷新区与控件叠加区域
	CDuiRect rcPaint;
	if( !rcPaint.IntersectRect(pRcPaint, &m_rcItem) )
		return;

	// 有叠加部分，重绘
	if (m_dwBackColor != 0xFF000000)
	{
		pRender->DrawColor(&rcPaint,m_dwBackColor);
	}
}

DWORD CControlUI::ModifyState(DWORD dwStateAdd /*= 0*/,DWORD dwStateRemove /*= 0*/)
{
	DWORD dwOldState = m_dwState;

	m_dwState |= dwStateAdd;
	m_dwState &= ~dwStateRemove;

	return dwOldState;
}

DWORD CControlUI::GetState(void)
{
	return m_dwState;
}

bool CControlUI::CheckState(DWORD dwState)
{
	return (m_dwState & dwState) != 0;
}

bool CControlUI::Activate()
{
	if( !IsVisible() )
		return false;
	if( !IsEnabled() )
		return false;
	return true;
}

void CControlUI::SetTextForState(LPCTSTR lpszText,DWORD dwState)
{

}

LPCTSTR CControlUI::GetTextForState(DWORD dwState)
{
	return NULL;
}

void CControlUI::SetImageForState(LPCTSTR lpszImage,DWORD dwState)
{

}

LPCTSTR CControlUI::GetImageForState(DWORD dwState)
{
	return NULL;
}

