#include "stdafx.h"
#include "WindowAnimation.h"
#include "Action.h"
#include "ActionInterval.h"

namespace DuiLib
{

CWindowAnimation::CWindowAnimation(void)
	: m_AnimationState(WindowAnimationState_Show)
	, m_hWnd(NULL)
	, m_pWindow(NULL)
	, m_bLayerWindow(false)
	//, m_pDibObj(NULL)
{
	m_BlendFun.BlendOp = 0;
	m_BlendFun.BlendFlags = 0;
	m_BlendFun.AlphaFormat = 1;
	m_BlendFun.SourceConstantAlpha = -56;

	m_szAnimationDC.cx = 0;
	m_szAnimationDC.cy = 0;

	SetParam(30,8);
}


CWindowAnimation::~CWindowAnimation(void)
{
	if ( m_hWnd != NULL)
	{
		m_hWnd = NULL;
	}
	if ( m_pWindow != NULL)
	{
		m_pWindow = NULL;
	}
	//if ( m_pDibObj != NULL )
	//{
	//	delete m_pDibObj;
	//	m_pDibObj = NULL;
	//}
}

void CWindowAnimation::Start(CWindowUI *pWindow)
{
	CActionManager::GetInstance()->AddAction(this,pWindow);
}

void CWindowAnimation::SetAnimationState(WindowAnimationState state)
{
	m_AnimationState =state;
}

LPCTSTR CWindowAnimation::GetClass()
{
	return _T("WindowAnimation");
}

void CWindowAnimation::startWithTarget(CObjectUI* pActionTarget)
{
	
	if ( pActionTarget != NULL )
	{
		CWindowUI* pWindow = static_cast<CWindowUI*>(pActionTarget->GetInterface(_T("Window")));
		 if ( pWindow == NULL)
		 {
			 DUI__Trace(_T("CWindowAnimation：只能绑定窗口！"));
			 return;
		 }
		m_pWindow = pWindow;
		CAction::startWithTarget(pActionTarget);
	}
}

void CWindowAnimation::onFinished()
{
	if ( m_pWindow )
	{
		m_pWindow->SetAnimating(false);
		switch (m_AnimationState)
		{
		case WindowAnimationState_Show:
			{
				m_pWindow->SetLayerWindow(m_bLayerWindow,false);
				m_pWindow->ShowWindow(SW_SHOW);
				m_pWindow->GetRoot();
				// 对根控件做了啥事？
				//v1 = CWindowUI::GetRoot(thisa->m_pWindow);
				//((void (__thiscall *)(_DWORD))v1->vfptr[1].SetAttribute)(v1);
				//v1 = CWindowUI::GetRoot(thisa->m_pWindow);
				//((void (__thiscall *)(_DWORD, _DWORD))v1->vfptr[3].~BaseObject)(v1, 0);
			}
			break;
		case WindowAnimationState_Hide:
			{
				m_pWindow->ShowWindow(SW_HIDE);
				m_pWindow->SetLayerWindow(m_bLayerWindow,false);
			}
			break;
		case WindowAnimationState_Close:
			{
				m_pWindow->CloseWindow(2);
			}
			break;
		case WindowAnimationState_None:
			{
				// 对根控件做了啥事？
				//v1 = CWindowUI::GetRoot(thisa->m_pWindow);
				//((void (__thiscall *)(_DWORD))v1->vfptr[1].SetAttribute)(v1);
				//v1 = CWindowUI::GetRoot(thisa->m_pWindow);
				//((void (__thiscall *)(_DWORD, _DWORD))v1->vfptr[3].~BaseObject)(v1, 0);
			}
			break;
		}
	}
}

}
