#include "StdAfx.h"

namespace DuiLib {

CControlUI::CControlUI() : 
m_pManager(NULL), 
m_pParent(NULL), 
m_pCover(NULL),
m_bUpdateNeeded(true),
m_bMenuUsed(false),
m_bAsyncNotify(false),
m_bVisible(true), 
m_bInternVisible(true),
m_bFocused(false),
m_bEnabled(true),
m_bMouseEnabled(true),
m_bKeyboardEnabled(true),
m_bFloat(false),
m_bSetPos(false),
m_chShortcut('\0'),
m_pTag(NULL),
m_dwBackColor(0),
m_dwBackColor2(0),
m_dwBackColor3(0),
m_dwBorderColor(0),
m_dwFocusBorderColor(0),
m_bColorHSL(false),
m_nBorderStyle(PS_SOLID),
m_nTooltipWidth(300)
{
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 9999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcPadding, sizeof(RECT));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_rcBorderSize,sizeof(RECT));
	m_piFloatPercent.left = m_piFloatPercent.top = m_piFloatPercent.right = m_piFloatPercent.bottom = 0.0f;
}

CControlUI::~CControlUI()
{
    if( m_pCover != NULL ) {
        m_pCover->Delete();
        m_pCover = NULL;
    }

	RemoveAllCustomAttribute();
    if( OnDestroy ) OnDestroy(this);
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

void CControlUI::Delete()
{
    if (m_pManager) m_pManager->RemoveMouseLeaveNeeded(this);
    delete this;
}

CDuiString CControlUI::GetName() const
{
    return m_sName;
}

void CControlUI::SetName(LPCTSTR pstrName)
{
	if (m_sName != pstrName) {
		m_sName = pstrName;
		if (m_pManager != NULL) m_pManager->RenameControl(this, pstrName);
	}
}

LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_CONTROL) == 0 ) return this;
    return NULL;
}

LPCTSTR CControlUI::GetClass() const
{
    return DUI_CTR_CONTROL;
}

UINT CControlUI::GetControlFlags() const
{
    return 0;
}

HWND CControlUI::GetNativeWindow() const
{
	return NULL;
}

bool CControlUI::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

CPaintManagerUI* CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
{
    if( m_pCover != NULL ) m_pCover->SetManager(pManager, this, bInit);
    m_pManager = pManager;
    m_pParent = pParent;
    if( bInit && m_pParent ) Init();
}

CControlUI* CControlUI::GetParent() const
{
    return m_pParent;
}

CControlUI* CControlUI::GetCover() const
{
    return m_pCover;
}

void CControlUI::SetCover(CControlUI *pControl)
{
    if( m_pCover == pControl ) return;
    if( m_pCover != NULL ) m_pCover->Delete();
    m_pCover = pControl;
    if( m_pCover != NULL ) {
        m_pManager->InitControls(m_pCover, this);
        if( IsVisible() ) NeedUpdate();
        else pControl->SetInternVisible(false);
    }
}

CDuiString CControlUI::GetText() const
{
    return m_sText;
}

void CControlUI::SetText(LPCTSTR pstrText)
{
    if( m_sText == pstrText ) return;

    m_sText = pstrText;
    Invalidate();
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

DWORD CControlUI::GetBkColor2() const
{
    return m_dwBackColor2;
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
    if( m_dwBackColor2 == dwBackColor ) return;

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
    return m_dwBackColor3;
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
    if( m_dwBackColor3 == dwBackColor ) return;

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}

LPCTSTR CControlUI::GetBkImage()
{
    return m_diBk.sDrawString;
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
	if( m_diBk.sDrawString == pStrImage && m_diBk.pImageInfo != NULL ) return;
	m_diBk.Clear();
	m_diBk.sDrawString = pStrImage;
	DrawImage(NULL, m_diBk);
	if( m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0 && m_diBk.pImageInfo ) {
		m_cxyFixed.cx = m_diBk.pImageInfo->nX;
		m_cxyFixed.cy = m_diBk.pImageInfo->nY;
	}
	Invalidate();
}

DWORD CControlUI::GetBorderColor() const
{
    return m_dwBorderColor;
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
    if( m_dwBorderColor == dwBorderColor ) return;

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
{
    if( m_dwFocusBorderColor == dwBorderColor ) return;

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}

bool CControlUI::IsColorHSL() const
{
    return m_bColorHSL;
}

void CControlUI::SetColorHSL(bool bColorHSL)
{
    if( m_bColorHSL == bColorHSL ) return;

    m_bColorHSL = bColorHSL;
    Invalidate();
}

RECT CControlUI::GetBorderSize() const
{
    return m_rcBorderSize;
}

void CControlUI::SetBorderSize( RECT rc )
{
	m_rcBorderSize = rc;
	Invalidate();
}

void CControlUI::SetBorderSize(int iSize)
{
	m_rcBorderSize.left = m_rcBorderSize.top = m_rcBorderSize.right = m_rcBorderSize.bottom = iSize;
	Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool CControlUI::DrawImage(HDC hDC, TDrawInfo& drawInfo)
{
	return CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, drawInfo);
}

const RECT& CControlUI::GetPos() const
{
    return m_rcItem;
}

RECT CControlUI::GetRelativePos() const
{
	CControlUI* pParent = GetParent();
	if( pParent != NULL ) {
		RECT rcParentPos = pParent->GetPos();
		CDuiRect rcRelativePos(m_rcItem);
		rcRelativePos.Offset(-rcParentPos.left, -rcParentPos.top);
		return rcRelativePos;
	}
	else {
		return CDuiRect(0, 0, 0, 0);
	}
}

RECT CControlUI::GetClientPos() const 
{
	return m_rcItem;
}

void CControlUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;

    CDuiRect invalidateRc = m_rcItem;
    if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

	if( m_bFloat ) {
		CControlUI* pParent = GetParent();
		if( pParent != NULL ) {
			RECT rcParentPos = pParent->GetPos();
			RECT rcCtrl = {rcParentPos.left + rc.left, rcParentPos.top + rc.top, 
				rcParentPos.left + rc.right, rcParentPos.top + rc.bottom};
			m_rcItem = rcCtrl;

			LONG width = rcParentPos.right - rcParentPos.left;
			LONG height = rcParentPos.bottom - rcParentPos.top;
			RECT rcPercent = {(LONG)(width*m_piFloatPercent.left), (LONG)(height*m_piFloatPercent.top),
				(LONG)(width*m_piFloatPercent.right), (LONG)(height*m_piFloatPercent.bottom)};
			m_cXY.cx = rc.left - rcPercent.left;
			m_cXY.cy = rc.top - rcPercent.top;
			m_cxyFixed.cx = rc.right - rcPercent.right - m_cXY.cx;
			m_cxyFixed.cy = rc.bottom - rcPercent.bottom - m_cXY.cy;
		}
	}
	else {
		m_rcItem = rc;
	}
    if( m_pManager == NULL ) return;

    if( !m_bSetPos ) {
        m_bSetPos = true;
        if( OnSize ) OnSize(this);
        m_bSetPos = false;
    }
    
    m_bUpdateNeeded = false;

	if( bNeedInvalidate && IsVisible() ) {
		invalidateRc.Join(m_rcItem);
		CControlUI* pParent = this;
		RECT rcTemp;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) return;
			rcTemp = invalidateRc;
			rcParent = pParent->GetPos();
			if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) return;
		}
		m_pManager->Invalidate(invalidateRc);
	}

    if( m_pCover != NULL && m_pCover->IsVisible() ) {
        if( m_pCover->IsFloat() ) {
            SIZE szXY = m_pCover->GetFixedXY();
            SIZE sz = {m_pCover->GetFixedWidth(), m_pCover->GetFixedHeight()};
            TPercentInfo rcPercent = m_pCover->GetFloatPercent();
            LONG width = m_rcItem.right - m_rcItem.left;
            LONG height = m_rcItem.bottom - m_rcItem.top;
            RECT rcCtrl = { 0 };
            rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx;
            rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy;
            rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx;
            rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy;
            m_pCover->SetPos(rcCtrl, false);
        }
        else {
            SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
            if( sz.cx < m_pCover->GetMinWidth() ) sz.cx = m_pCover->GetMinWidth();
            if( sz.cx > m_pCover->GetMaxWidth() ) sz.cx = m_pCover->GetMaxWidth();
            if( sz.cy < m_pCover->GetMinHeight() ) sz.cy = m_pCover->GetMinHeight();
            if( sz.cy > m_pCover->GetMaxHeight() ) sz.cy = m_pCover->GetMaxHeight();
            RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
            m_pCover->SetPos(rcCtrl, false);
        }
    }
}

void CControlUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
	CDuiRect invalidateRc = m_rcItem;
	m_rcItem.left += szOffset.cx;
	m_rcItem.top += szOffset.cy;
	m_rcItem.right += szOffset.cx;
	m_rcItem.bottom += szOffset.cy;

	if( bNeedInvalidate && m_pManager == NULL && IsVisible() ) {
		invalidateRc.Join(m_rcItem);
		CControlUI* pParent = this;
		RECT rcTemp;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) return;
			rcTemp = invalidateRc;
			rcParent = pParent->GetPos();
			if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) return;
		}
		m_pManager->Invalidate(invalidateRc);
	}

    if( m_pCover != NULL && m_pCover->IsVisible() ) m_pCover->Move(szOffset, false);
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
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
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    NeedParentUpdate();
}

TPercentInfo CControlUI::GetFloatPercent() const
{
	return m_piFloatPercent;
}

void CControlUI::SetFloatPercent(TPercentInfo piFloatPercent)
{
	m_piFloatPercent = piFloatPercent;
	NeedParentUpdate();
}

int CControlUI::GetFixedWidth() const
{
    return m_cxyFixed.cx;
}

void CControlUI::SetFixedWidth(int cx)
{
    if( cx < 0 ) return; 
    m_cxyFixed.cx = cx;
    NeedParentUpdate();
}

int CControlUI::GetFixedHeight() const
{
    return m_cxyFixed.cy;
}

void CControlUI::SetFixedHeight(int cy)
{
    if( cy < 0 ) return; 
    m_cxyFixed.cy = cy;
    NeedParentUpdate();
}

int CControlUI::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    NeedParentUpdate();
}

int CControlUI::GetMaxWidth() const
{
	if (m_cxyMax.cx < m_cxyMin.cx) return m_cxyMin.cx;
    return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    NeedParentUpdate();
}

int CControlUI::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
    if( m_cxyMin.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMin.cy = cy;
    NeedParentUpdate();
}

int CControlUI::GetMaxHeight() const
{
	if (m_cxyMax.cy < m_cxyMin.cy) return m_cxyMin.cy;
    return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
    if( m_cxyMax.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMax.cy = cy;
    NeedParentUpdate();
}

CDuiString CControlUI::GetToolTip() const
{
    return m_sToolTip;
}

void CControlUI::SetToolTip(LPCTSTR pstrText)
{
	CDuiString strTemp(pstrText);
	strTemp.Replace(_T("<n>"),_T("\r\n"));
	m_sToolTip=strTemp;
}

void CControlUI::SetToolTipWidth( int nWidth )
{
	m_nTooltipWidth=nWidth;
}

int CControlUI::GetToolTipWidth( void )
{
	return m_nTooltipWidth;
}

TCHAR CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(TCHAR ch)
{
    m_chShortcut = ch;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const CDuiString& CControlUI::GetUserData()
{
    return m_sUserData;
}

void CControlUI::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR CControlUI::GetTag() const
{
    return m_pTag;
}

void CControlUI::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

bool CControlUI::IsVisible() const
{
    return m_bVisible && m_bInternVisible;
}

void CControlUI::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }

    if( m_pCover != NULL ) m_pCover->SetInternVisible(IsVisible());
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}

    if( m_pCover != NULL ) m_pCover->SetInternVisible(IsVisible());
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this, false);
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

void CControlUI::AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
{
	if( pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0') ) return;
	CDuiString* pCostomAttr = new CDuiString(pstrAttr);
	if (pCostomAttr != NULL) {
		if (m_mCustomAttrHash.Find(pstrName) == NULL)
			m_mCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
		else
			delete pCostomAttr;
	}
}

LPCTSTR CControlUI::GetCustomAttribute(LPCTSTR pstrName) const
{
	if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
	CDuiString* pCostomAttr = static_cast<CDuiString*>(m_mCustomAttrHash.Find(pstrName));
	if( pCostomAttr ) return pCostomAttr->GetData();
	return NULL;
}

bool CControlUI::RemoveCustomAttribute(LPCTSTR pstrName)
{
	if( pstrName == NULL || pstrName[0] == _T('\0') ) return NULL;
	CDuiString* pCostomAttr = static_cast<CDuiString*>(m_mCustomAttrHash.Find(pstrName));
	if( !pCostomAttr ) return false;

	delete pCostomAttr;
	return m_mCustomAttrHash.Remove(pstrName);
}

void CControlUI::RemoveAllCustomAttribute()
{
	CDuiString* pCostomAttr;
	for( int i = 0; i< m_mCustomAttrHash.GetSize(); i++ ) {
		if(LPCTSTR key = m_mCustomAttrHash.GetAt(i)) {
			pCostomAttr = static_cast<CDuiString*>(m_mCustomAttrHash.Find(key));
			delete pCostomAttr;
		}
	}
	m_mCustomAttrHash.Resize();
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
	if( (uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL ) return NULL;
    
    CControlUI* pResult = NULL;
    if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
        if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
    }
    if( pResult == NULL && m_pCover != NULL ) {
        /*if( (uFlags & UIFIND_HITTEST) == 0 || true)*/ pResult = m_pCover->FindControl(Proc, pData, uFlags);
    }
    if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) {
        if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
    }
    return pResult;
}

void CControlUI::Invalidate()
{
    if( !IsVisible() ) return;

    RECT invalidateRc = m_rcItem;

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }

    if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void CControlUI::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

DWORD CControlUI::GetAdjustColor(DWORD dwColor)
{
    if( !m_bColorHSL ) return dwColor;
    short H, S, L;
    CPaintManagerUI::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void CControlUI::Init()
{
    DoInit();
    if( OnInit ) OnInit(this);
}

void CControlUI::DoInit()
{

}

void CControlUI::Event(TEventUI& event)
{
    if( OnEvent(&event) ) DoEvent(event);
}

void CControlUI::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETCURSOR )
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            return;
        }
    }
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}


void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
{
	m_sVirtualWnd = pstrValue;
	m_pManager->UsedVirtualWnd(true);
}

CDuiString CControlUI::GetVirtualWnd() const
{
	CDuiString str;
	if( !m_sVirtualWnd.IsEmpty() ){
		str = m_sVirtualWnd;
	}
	else{
		CControlUI* pParent = GetParent();
		if( pParent != NULL){
			str = pParent->GetVirtualWnd();
		}
		else{
			str = _T("");
		}
	}
	return str;
}

CDuiString CControlUI::GetAttribute(LPCTSTR pstrName)
{
    return _T("");
}

void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("pos")) == 0 ) {
        RECT rcPos = { 0 };
        LPTSTR pstr = NULL;
        rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SIZE szXY = {rcPos.left, rcPos.top};
        SetFixedXY(szXY);
		//ASSERT(rcPos.right - rcPos.left >= 0);
		//ASSERT(rcPos.bottom - rcPos.top >= 0);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( _tcscmp(pstrName, _T("padding")) == 0 ) {
        RECT rcPadding = { 0 };
        LPTSTR pstr = NULL;
        rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetPadding(rcPadding);
    }
    else if( _tcscmp(pstrName, _T("bkcolor")) == 0 || _tcscmp(pstrName, _T("bkcolor1")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor2")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor2(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bkcolor3")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor3(clrColor);
    }
    else if( _tcscmp(pstrName, _T("bordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("focusbordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetFocusBorderColor(clrColor);
    }
    else if( _tcscmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("bordersize")) == 0 ) {
		CDuiString nValue = pstrValue;
		if(nValue.Find(',') < 0)
		{
			SetBorderSize(_ttoi(pstrValue));
		}
		else
		{
			RECT rcBorder = { 0 };
			LPTSTR pstr = NULL;
			rcBorder.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcBorder.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcBorder.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcBorder.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetBorderSize(rcBorder);
		}
	}
	else if( _tcscmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("borderround")) == 0 ) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
    else if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
    else if( _tcscmp(pstrName, _T("width")) == 0 ) SetFixedWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("height")) == 0 ) SetFixedHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("minwidth")) == 0 ) SetMinWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("minheight")) == 0 ) SetMinHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
    else if( _tcscmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
    else if( _tcscmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
    else if( _tcscmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
    else if( _tcscmp(pstrName, _T("tag")) == 0 ) SetTag(_ttoi(pstrValue));
    else if( _tcscmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
    else if( _tcscmp(pstrName, _T("float")) == 0 ) {
		CDuiString nValue = pstrValue;
		if(nValue.Find(',') < 0) {
			SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else {
			TPercentInfo piFloatPercent = { 0 };
			LPTSTR pstr = NULL;
			piFloatPercent.left = _tcstod(pstrValue, &pstr);  ASSERT(pstr);
			piFloatPercent.top = _tcstod(pstr + 1, &pstr);    ASSERT(pstr);
			piFloatPercent.right = _tcstod(pstr + 1, &pstr);  ASSERT(pstr);
			piFloatPercent.bottom = _tcstod(pstr + 1, &pstr); ASSERT(pstr);
			SetFloatPercent(piFloatPercent);
			SetFloat(true);
		}
	}
    else if( _tcscmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
    else if( _tcscmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcscmp(pstrValue, _T("true")) == 0);
	else if( _tcscmp(pstrName, _T("virtualwnd")) == 0 ) SetVirtualWnd(pstrValue);
	else {
		AddCustomAttribute(pstrName, pstrValue);
	}
}

CDuiString CControlUI::GetAttributeList(bool bIgnoreDefault)
{
	return _T("");
}

void CControlUI::SetAttributeList(LPCTSTR pstrList)
{
    CDuiString sItem;
    CDuiString sValue;
    while( *pstrList != _T('\0') ) {
        sItem.Empty();
        sValue.Empty();
        while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('=') );
        if( *pstrList++ != _T('=') ) return;
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return;
        while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        ASSERT( *pstrList == _T('\"') );
        if( *pstrList++ != _T('\"') ) return;
        SetAttribute(sItem, sValue);
        if( *pstrList++ != _T(' ') ) return;
    }
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

bool CControlUI::Paint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	if (pStopControl == this) return false;
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return true;
	if( OnPaint ) {
		if( !OnPaint(this) ) return true;
	}
	if (!DoPaint(hDC, rcPaint, pStopControl)) return false;
    if( m_pCover != NULL ) return m_pCover->Paint(hDC, rcPaint);
    return true;
}

bool CControlUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
    // 绘制循序：背景颜色->背景图->状态图->文本->边框
    if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
        CRenderClip roundClip;
        CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    return true;
}

void CControlUI::PaintBkColor(HDC hDC)
{
    if( m_dwBackColor != 0 ) {
        if( m_dwBackColor2 != 0 ) {
            if( m_dwBackColor3 != 0 ) {
                RECT rc = m_rcItem;
                rc.bottom = (rc.bottom + rc.top) / 2;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 8);
                rc.top = rc.bottom;
                rc.bottom = m_rcItem.bottom;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3), true, 8);
            }
            else 
                CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2), true, 16);
        }
        else if( m_dwBackColor >= 0xFF000000 ) CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor));
        else CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor));
    }
}

void CControlUI::PaintBkImage(HDC hDC)
{
	DrawImage(hDC, m_diBk);
}

void CControlUI::PaintStatusImage(HDC hDC)
{
    return;
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
{
	if(m_rcBorderSize.left > 0 && (m_dwBorderColor != 0 || m_dwFocusBorderColor != 0)) {
		if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 )//画圆角边框
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_rcBorderSize.left, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle);
			else
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_rcBorderSize.left, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
		}
		else {
			if (m_rcBorderSize.right == m_rcBorderSize.left && m_rcBorderSize.top == m_rcBorderSize.left && m_rcBorderSize.bottom == m_rcBorderSize.left) {
				if (IsFocused() && m_dwFocusBorderColor != 0)
					CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle);
				else
					CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
			}
			else {
				RECT rcBorder;
				if(m_rcBorderSize.left > 0){
					rcBorder		= m_rcItem;
                    rcBorder.left  += m_rcBorderSize.left / 2;
					rcBorder.right	= rcBorder.left;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.left,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.left,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.top > 0) {
					rcBorder		= m_rcItem;
                    rcBorder.top   += m_rcBorderSize.top / 2;
					rcBorder.bottom	= rcBorder.top;
                    rcBorder.left  += m_rcBorderSize.left;
                    rcBorder.right -= m_rcBorderSize.right;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.top,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.top,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.right > 0) {
					rcBorder		= m_rcItem;
					rcBorder.left	= m_rcItem.right - m_rcBorderSize.right / 2;
                    rcBorder.right  = rcBorder.left;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.right,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.right,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.bottom > 0) {
					rcBorder		= m_rcItem;
					rcBorder.top	= m_rcItem.bottom - m_rcBorderSize.bottom / 2;
                    rcBorder.bottom = rcBorder.top;
                    rcBorder.left  += m_rcBorderSize.left;
                    rcBorder.right -= m_rcBorderSize.right;
					if (IsFocused() && m_dwFocusBorderColor != 0)
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.bottom,GetAdjustColor(m_dwFocusBorderColor),m_nBorderStyle);
					else
						CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.bottom,GetAdjustColor(m_dwBorderColor),m_nBorderStyle);
				}
			}
		}
	}
}

void CControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
	if( OnPostPaint ) OnPostPaint(this);
}

int CControlUI::GetBorderStyle() const
{
	return m_nBorderStyle;
}

void CControlUI::SetBorderStyle( int nStyle )
{
	m_nBorderStyle = nStyle;
	Invalidate();
}

} // namespace DuiLib
