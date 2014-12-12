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
	, m_bIsFocused(false)
	, m_bIsFloat(false)					// 推荐使用浮动布局
	, m_bIsSetPos(false)
	, m_nTooltipWidth(300)
	, m_dwState(UISTATE_Normal)
	, m_iZOrder(0)
	, m_pTag(NULL)
	, m_pImageBackground(NULL)
	, m_pNotifyFilter(NULL)
	, m_dwTextColor(0)
	, m_sFontIndex(L"")
	, m_sTextStyle(L"noprefix")
	, m_uTextStyle(DT_NOPREFIX)
	, m_dwBorderColor(0)
	, m_dwFocusBorderColor(0)
	, m_nBorderSize(0)
	, m_chShortcut('\0')
{
	m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;
	m_cxyMax.cx = m_cxyMax.cy = 7680*4;
}


CControlUI::~CControlUI(void)
{
	 if( m_pManager != NULL )
		 m_pManager->ReapObjects(this);

	 if ( m_pImageBackground != NULL)
		delete m_pImageBackground;
}

void CControlUI::SetManager(CWindowUI* pManager, CControlUI* pParent,bool bInit /*= false*/)
{
	m_pManager = pManager;
	m_pParent = pParent;
	 if( bInit && m_pParent )
		 Init();
}

CWindowUI* CControlUI::GetManager()
{
	return m_pManager;
}

CControlUI* CControlUI::GetParent()
{
	return m_pParent;
}

void CControlUI::SendNotify(UINOTIFY dwType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, bool bAsync /*= false*/)
{
	if ( m_pManager )
	{
		m_pManager->SendNotify(this,dwType,wParam,lParam,bAsync);
	}
}

void CControlUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if ( lpszValue== NULL || lpszValue[0] == NULL)
		return;
	
	if ( _tcsicmp(lpszName,_T("bkimage")) ==0 )
		SetPropertyForState(lpszValue,UIProperty_Back_Image);
	else if (_tcsicmp(lpszName, _T("foreimage")) == 0) 
		SetPropertyForState(lpszValue, UIProperty_Fore_Image);
	else if (_tcsicmp(lpszName, L"bkcolor1") == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Color1);
	else if (_tcsicmp(lpszName, L"bkcolor2") == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Color2);
	else if (_tcsicmp(lpszName, L"bkcolor3") == 0)
		SetPropertyForState(lpszValue, UIProperty_Back_Color3);
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
	else if( _tcscmp(lpszName, _T("bordercolor")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		SetPropertyForState(lpszValue, UIProperty_Border_Color);
		SetBorderColor(clrColor);
	}
	else if( _tcscmp(lpszName, _T("focusbordercolor")) == 0 )
	{
		DWORD clrColor = 0;
		clrColor = CDuiCodeOperation::StringToColor(lpszValue);
		SetFocusBorderColor(clrColor);
	}
	else if( _tcscmp(lpszName, _T("borderround")) == 0 ) 
	{
		SIZE cxyRound = { 0 };
		CDuiCodeOperation::StringToSize(lpszValue,&cxyRound);
		SetBorderRound(cxyRound);
	}
	else if (_tcscmp(lpszName, L"bordersize") == 0)
	{
		int nSize = CDuiCodeOperation::StringToInt(lpszValue);
		SetPropertyForState(lpszValue, UIProperty_Border_Wdith);
		SetBorderSize(nSize);
	}
	else if( _tcscmp(lpszName, _T("width")) == 0 ) 
		SetFixedWidth(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("height")) == 0 ) 
		SetFixedHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("minwidth")) == 0 ) 
		SetMinWidth(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("minheight")) == 0 ) 
		SetMinHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("maxwidth")) == 0 ) 
		SetMaxWidth(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("maxheight")) == 0 ) 
		SetMaxHeight(_ttoi(lpszValue));
	else if( _tcscmp(lpszName, _T("name")) == 0 ) 
		SetName(lpszValue);
	else if( _tcscmp(lpszName, _T("text")) == 0 ) 
		SetText(lpszValue);
	else if (_tcscmp(lpszName, _T("fontindex")) == 0)
		SetFontName(lpszValue);
	else if (_tcscmp(lpszName, _T("textcolor")) == 0)
		SetTextColor(CDuiCodeOperation::StringToColor(lpszValue));
	else if (_tcscmp(lpszName, _T("textstyle")) == 0)
		SetTextStyle(lpszValue);
	else if( _tcscmp(lpszName, _T("tooltip")) == 0 ) 
		SetToolTip(lpszValue);
	else if( _tcscmp(lpszName, _T("enabled")) == 0 ) 
		SetEnabled(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("mouse")) == 0 ) 
		SetMouseEnabled(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("keyboard")) == 0 ) 
		SetKeyboardEnabled(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("visible")) == 0 ) 
		SetVisible(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("float")) == 0 ) 
		SetFloat(_tcscmp(lpszValue, _T("true")) == 0);
	else if( _tcscmp(lpszName, _T("menu")) == 0 ) 
		SetContextMenuUsed(_tcscmp(lpszValue, _T("true")) == 0);
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() )
		return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() )
		return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bIsMouseEnabled || !m_rcControl.PtInRect(* static_cast<LPPOINT>(pData)) ) )
		return NULL;
	return Proc(this, pData);
}

bool CControlUI::EventHandler(TEventUI& event)
{   // 处理CWindowUI转发的窗口消息事件

	// 控件默认处于Normal状态，
	// 收到感兴趣的消息后，给控件增加状态，但不清除Normal状态标记
	// 新增加的非Normal状态一般会在某个消息中被清除掉，以使控件能恢复到只有Normal状态
	// 控件绘图处理会按照状态顺序反序绘图，不会因为状态叠加同时画几种状态
	// 可参考CButtonUI::EventHandler处理

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
	
	return true;
}

RECT CControlUI::GetPosition()
{
	return m_rcControl;
}

void CControlUI::SetPosition(LPCRECT rc)
{
	CDuiRect rect(rc);

	if( rect.right < rect.left )
		rect.right = rect.left;
	if( rect.bottom < rect.top )
		rect.bottom = rect.top;

	CDuiRect invalidateRc = m_rcControl;
	if( invalidateRc.IsRectEmpty() )
		invalidateRc = rc;

	m_rcControl = rc;
	if( m_pManager == NULL )
		return;

	if( !m_bIsSetPos )
	{
		m_bIsSetPos = true;
		//if( OnSize )
		//	OnSize(this);
		m_bIsSetPos = false;
	}

	if( m_bIsFloat )
	{
		CControlUI* pParent = GetParent();
		if( pParent != NULL )
		{
			RECT rcParentPos = pParent->GetPosition();

			if( m_cXY.cx >= 0 ) 
				m_cXY.cx = m_rcControl.left - rcParentPos.left;
			else 
				m_cXY.cx = m_rcControl.right - rcParentPos.right;

			if( m_cXY.cy >= 0 ) 
				m_cXY.cy = m_rcControl.top - rcParentPos.top;
			else 
				m_cXY.cy = m_rcControl.bottom - rcParentPos.bottom;

			m_cxyFixed.cx = m_rcControl.right - m_rcControl.left;
			m_cxyFixed.cy = m_rcControl.bottom - m_rcControl.top;
		}
	}

	m_bIsUpdateNeeded = false;
	invalidateRc.Join(m_rcControl);

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

	RECT invalidateRect = m_rcControl;

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
	return m_rcControl.GetWidth();
}

int CControlUI::GetHeight() const
{
	return m_rcControl.GetHeight();
}

int CControlUI::GetX() const
{
	 return m_rcControl.left;
}

int CControlUI::GetY() const
{
	 return m_rcControl.top;
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

	if( cx < 0 )
		return; 
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
	if( !rcPaint.IntersectRect(pRcPaint, &m_rcControl) )
		return;

	// 有叠加部分，重绘
	CUIPaint::GetInstance()->DrawControl(pRender,this,pRcPaint);
}

DWORD CControlUI::ModifyState(DWORD dwStateAdd /*= 0*/,DWORD dwStateRemove /*= 0*/)
{
	DWORD dwOldState = m_dwState;
	this->m_dwState = (this->m_dwState & ~dwStateRemove) | dwStateAdd;
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

void CControlUI::SetZOrder(int iZOrder)
{
	if ( m_iZOrder == iZOrder )
		return;

	m_iZOrder = iZOrder;
	if ( IsVisible() )
		NeedParentUpdate();
}

int CControlUI::GetZOrder() const
{
	return m_iZOrder;
}

void CControlUI::SetTag(LPVOID pTag)
{
	m_pTag = pTag;
}

LPVOID CControlUI::GetTag() const
{
	return m_pTag;
}

void CControlUI::SetPropertyForState(LPCTSTR lpszValue,UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	if ( lpszValue[0]==NULL)
		return;

	size_t iCount = m_property.count(dwState);

	do 
	{
		// 循环检查指定状态的指定属性是否已经存在了
		if ( iCount != 0 )
		{
			// 存在值，检查是否需要替换现有属性
			UIStatePropertyMap::iterator iter = m_property.find(dwState);
			for ( size_t i=0;i<iCount;++i,++iter)
			{
				if ( iter->second.property == propType )
				{
					iter->second.strValue = lpszValue;
					break;
				}
			}
		}

		CDuiString strValue(lpszValue);
		Property prop;
		prop.property = propType;
		prop.strValue = lpszValue;
		m_property.insert(UIStatePropertyMap::value_type(dwState,prop));

	} while (false);
	
	if ( propType ==  UIProperty_Back_Image || propType == UIProperty_Fore_Image )
		SetImage(lpszValue,propType,dwState);
}

LPCTSTR CControlUI::GetPropertyForState(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	size_t iCount = m_property.count(dwState);
	if ( iCount != 0 )
	{
		UIStatePropertyMap::iterator iter = m_property.find(dwState);
		for ( size_t i=0;i<iCount;++i,++iter)
		{
			if ( iter->second.property == propType )
			{
				return iter->second.strValue.c_str();
			}
		}
	}

	return NULL;
}

RECT CControlUI::GetRectProperty(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	CDuiRect rc;
	LPCTSTR lpszRect = GetPropertyForState(propType,dwState);
	if ( lpszRect == NULL)
		return rc;

	CDuiCodeOperation::StringToRect(lpszRect,&rc);
	return rc;
}

DWORD CControlUI::GetColorProperty(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	DWORD dwColor =0;
	LPCTSTR lpszColor = GetPropertyForState(propType,dwState);
	if ( lpszColor == 0)
		return 0;

	return CDuiCodeOperation::StringToColor(lpszColor);
}

int CControlUI::GetIntProperty(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	LPCTSTR lpszNum = GetPropertyForState(propType,dwState);
	if ( lpszNum == 0)
		return 0;

	return CDuiCodeOperation::StringToInt(lpszNum);
}

LPCTSTR CControlUI::GetTextProperty(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	LPCTSTR lpszStateText = GetPropertyForState(propType,dwState);
	if ( lpszStateText == NULL )
		lpszStateText =  GetPropertyForState(propType,UISTATE_Normal);

	return lpszStateText;
}

ImageObject* CControlUI::GetImageProperty(UIProperty propType,DWORD dwState /*= UISTATE_Normal*/)
{
	if ( propType == UIProperty_Back_Image && dwState == UISTATE_Normal )
		return m_pImageBackground;

	return NULL;
}

void CControlUI::SetImage(LPCTSTR lpszImageString,UIProperty propType,DWORD dwState)
{
	// 解析字符串，生成ImageObject对象存储
	if ( dwState == UISTATE_Normal && propType == UIProperty_Back_Image)
	{
		if ( m_pImageBackground != NULL)
		{
			delete m_pImageBackground;
		}
		m_pImageBackground = ImageObject::CreateFromString(lpszImageString);
	}
}

void CControlUI::SetNotifyFilter(INotifyUI* pNotifyFilter)
{
	m_pNotifyFilter = pNotifyFilter;
}

INotifyUI* CControlUI::GetNotifyFilter(void) const
{
	return m_pNotifyFilter;
}

void CControlUI::SetFontName(LPCTSTR lpszFontIndex)
{
	ASSERT(lpszFontIndex);
	if (m_sFontIndex == lpszFontIndex)
	{
		return;
	}
	m_sFontIndex = lpszFontIndex;
	Invalidate();
}

LPCTSTR CControlUI::GetFontName()
{
	return m_sFontIndex.c_str();
}

void CControlUI::SetTextStyle(LPCTSTR lpszStyle)
{
	if (m_sTextStyle == lpszStyle)
	{
		return;
	}
	// TODO ==> 这里需要有一个更好的方法优化一下
	m_sTextStyle = lpszStyle;
	VecString vcStyle;
	UINT uStyle = DT_NOPREFIX;
	CDuiStringOperation::splite(lpszStyle, L".", vcStyle);
	VecString::iterator it = vcStyle.begin();
	for (; it != vcStyle.end(); it++)
	{
		if ((*it) == L"center")
		{
			uStyle = uStyle | DT_CENTER;
		}
		else if ((*it) == L"bottom")
		{
			uStyle = uStyle | DT_BOTTOM; // 必须与singleline组合使用
		}
		else if ((*it) == L"top")
		{
			uStyle = uStyle | DT_TOP;
		}
		else if ((*it) == L"left")
		{
			uStyle = uStyle | DT_LEFT;
		}
		else if ((*it) == L"right")
		{
			uStyle = uStyle | DT_RIGHT;
		}
		else if ((*it) == L"vcenter")
		{
			uStyle = uStyle | DT_VCENTER; // 必须与singleline组合使用
		}
		else if ((*it) == L"calcrect")
		{
			uStyle = uStyle | DT_CALCRECT;
		}
		else if ((*it) == L"editcontrol")
		{
			uStyle = uStyle | DT_EDITCONTROL;
		}
		else if ((*it) == L"endellipsis")
		{
			uStyle = uStyle | DT_END_ELLIPSIS;
		}
		else if ((*it) == L"nofullwidthcharbreak")
		{
			uStyle = uStyle | DT_NOFULLWIDTHCHARBREAK;
		}
		else if ((*it) == L"pathellipsis")
		{
			uStyle = uStyle | DT_PATH_ELLIPSIS;
		}
		else if ((*it) == L"singleline")
		{
			uStyle = uStyle | DT_SINGLELINE;
		}
		else if ((*it) == L"wordbreak")
		{
			uStyle = uStyle | DT_WORDBREAK;
		}
		else if ((*it) == L"wordellipsis")
		{
			uStyle = uStyle | DT_WORD_ELLIPSIS;
		}
		else if ((*it) == L"noprefix")
		{
			uStyle = uStyle | DT_NOPREFIX;
		}
	}

	m_uTextStyle = uStyle;
	Invalidate();
}

LPCTSTR CControlUI::GetTextStyle()
{
	return m_sTextStyle.c_str();
}

UINT CControlUI::GetTextStyleByValue()
{
	return m_uTextStyle;
}

void CControlUI::SetTextColor(DWORD dwColor)
{
	if (dwColor == m_dwTextColor)
	{
		return;
	}
	m_dwTextColor = dwColor;
	Invalidate();
}

DWORD CControlUI::GetTextColor()
{
	return m_dwTextColor;
}

void CControlUI::SetBorderColor(DWORD dwColor)
{
	if (m_dwBorderColor == dwColor)
	{
		return;
	}
	m_dwBorderColor = dwColor;
	Invalidate();
}

DWORD CControlUI::GetBorderColor()
{
	return m_dwBorderColor;
}

void CControlUI::SetFocusBorderColor(DWORD dwColor)
{
	if (m_dwFocusBorderColor == dwColor)
	{
		return;
	}
	m_dwFocusBorderColor = dwColor;
	Invalidate();
}

DWORD CControlUI::GetFocusBorderColor()
{
	return m_dwFocusBorderColor;
}

void CControlUI::SetBorderRound(SIZE cxySize)
{
	m_cxyBorderRound = cxySize;
	Invalidate();
}

SIZE CControlUI::GetBorderRound()
{
	return m_cxyBorderRound;
}

void CControlUI::SetBorderSize(int nSize)
{
	if (m_nBorderSize == nSize)
	{
		return;
	}
	m_nBorderSize = nSize;
	Invalidate();
}

int CControlUI::GetBorderSize()
{
	return m_nBorderSize;
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
	return m_cxyFixed;
}

void CControlUI::PostRender(IUIRender* pRender, LPCRECT pRcPaint)
{

}

TCHAR CControlUI::GetShortcut() const
{
	return m_chShortcut;
}

void CControlUI::SetShortcut(TCHAR ch)
{
	m_chShortcut = ch;
}

void CControlUI::Init()
{
	 DoInit();
	 if( m_OnInit )
		 m_OnInit(this);
}

void CControlUI::DoInit()
{

}
