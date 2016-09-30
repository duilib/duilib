#include "StdAfx.h"
#include "ButtonUIEx.h"

CButtonUIEx::CButtonUIEx(void)
{
	m_nAnimationState = 0;
}

CButtonUIEx::~CButtonUIEx(void)
{
	m_pManager->KillTimer(this);
}

LPCTSTR CButtonUIEx::GetClass() const
{
	return _T("CButtonUIEx");
}

LPVOID CButtonUIEx::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ButtonUIEx")) == 0 ) return static_cast<CButtonUIEx*>(this);
	return CButtonUI::GetInterface(pstrName);
}

UINT CButtonUIEx::GetControlFlags() const
{
	return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
}

void CButtonUIEx::DoEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() )
		{
			m_uButtonState |= UISTATE_HOT;

			if (m_nAnimationState == 0 || m_nAnimationState==3)
			{
				m_nAnimationState=1;
				m_pManager->SetTimer(this,0x1000+1,1);
			}
		}
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( IsEnabled() )
		{
			m_uButtonState &= ~UISTATE_HOT;
			if (m_nAnimationState==1 || m_nAnimationState==2)
			{
				m_nAnimationState=3;
				m_pManager->SetTimer(this,0x1000+1,1);
			}
		}
	}
	if( event.Type == UIEVENT_SETCURSOR )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}
	CButtonUI::DoEvent(event);
}

void CButtonUIEx::OnTimer(const int& nTimerId)
{
	if (nTimerId == 0x1000+1)
	{
		if (m_nAnimationState==1)
		{//鼠标进入,正在变大
			
			/*测试效果用的代码*/
			RECT rcCur = GetPos();
			if (rcCur.right-rcCur.left==80)
			{
				m_pManager->KillTimer(this,0x1000+1);
				m_nAnimationState=2;
			}
			else
			{
				RECT rcNew = {rcCur.left-1,rcCur.top-1,rcCur.right+1,rcCur.bottom+1};
				SetPos(rcNew);
				//Invalidate();
			}
		}
		else if (m_nAnimationState==3)
		{//鼠标离开,正在变小

			/*测试效果用的代码*/
			RECT rcCur = GetPos();
			if (rcCur.right-rcCur.left==48)
			{
				m_pManager->KillTimer(this,0x1000+1);
				m_nAnimationState=0;
			}
			else
			{
				RECT rcNew = {rcCur.left+1,rcCur.top+1,rcCur.right-1,rcCur.bottom-1};
				SetPos(rcNew);
				//Invalidate();
			}
		}
	}
}