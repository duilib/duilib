#include "stdafx.h"
#include "Animation.h"
#include "ActionInterval.h"

namespace DuiLib
{

	AlphaChangeAnimation::AlphaChangeAnimation()
		: m_nStartAlpha(0)
		, m_nEndAlpha(255)
	{
		SetParam(20,8);
	}

	AlphaChangeAnimation::~AlphaChangeAnimation()
	{

	}

	void AlphaChangeAnimation::SetKeyFrameAlpha(int startAlpha, int endAlpha)
	{
		m_nStartAlpha = startAlpha;
		m_nEndAlpha = endAlpha;
	}

	LPCTSTR AlphaChangeAnimation::GetClass()
	{
		return _T("AlphaChangeAnimation");
	}

	void AlphaChangeAnimation::startWithTarget(CObjectUI* pActionTarget)
	{
		if ( pActionTarget )
		{
			// RTTI 类型检测
			CAction::startWithTarget(pActionTarget);
		}
	}

	void AlphaChangeAnimation::Tick(IActionInterval* pActionInterval)
	{
		CObjectUI* pTarget = ActionTarget();
		if ( pTarget )
		{
			CActionInterval::FadeInterval(pTarget,GetCurveObj(),pActionInterval,m_nStartAlpha,m_nEndAlpha);
		}
	}

	void AlphaChangeAnimation::onStop()
	{
		onFinished();
	}

	void AlphaChangeAnimation::onFinished()
	{
		CControlUI* pTarget = (CControlUI*)ActionTarget();
		if ( pTarget )
		{
			DWORD dwAlpha = 0;
			if ( IsReverse() )
			{
				dwAlpha = m_nStartAlpha;
			}
			else
			{
				dwAlpha = m_nEndAlpha;
			}

			pTarget->SetAlpha(dwAlpha);
			// ???
			//  ((void (__thiscall *)(_DWORD, _DWORD))pView->vfptr[3].~BaseObject)(pView, 0);
		}
	}

	PosChangeAnimation::PosChangeAnimation()
	{
		SetParam(20,8);
		SetCurveID(_T("curve.sine.out"));
	}

	void PosChangeAnimation::SetKeyFramePos(const RECT* pStartPos, const RECT* pEndPos)
	{
		if ( pStartPos && pEndPos )
		{
			m_startPos = *pStartPos;
			m_stopPos = *pEndPos;
		}
	}

	LPCTSTR PosChangeAnimation::GetClass()
	{
		return _T("PosChangeAnimation");
	}

	void PosChangeAnimation::startWithTarget(CObjectUI* pActionTarget)
	{
		if ( pActionTarget )
		{
			// RTTI 类型检测
			CAction::startWithTarget(pActionTarget);
		}
	}

	void PosChangeAnimation::Tick(IActionInterval* pActionInterval)
	{
		CDuiRect rcParent;
		
		CControlUI* pTarget = static_cast<CControlUI*>(ActionTarget());
		if ( pTarget && pTarget->GetParent() != NULL)
		{
			rcParent = pTarget->GetParent()->GetPos();
		}

		//if ( rcParent.IsRectEmpty())
		//{
			//CDuiRect rcStart;
			//CDuiRect rcStop;
			CActionInterval::PosInterval(pTarget,GetCurveObj(),pActionInterval,m_startPos,m_stopPos);
		//}
	}

	void PosChangeAnimation::onStop()
	{
		onFinished();
	}

	void PosChangeAnimation::onFinished()
	{
		CObjectUI* pTarget = ActionTarget();
		if ( pTarget )
		{
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ImageSequenceAnimation
	ImageSequenceAnimation::ImageSequenceAnimation()
	{

	}

	ImageSequenceAnimation::~ImageSequenceAnimation()
	{

	}

	LPCTSTR ImageSequenceAnimation::GetClass()
	{
		return _T("ImageSequenceAnimation");
	}

	void ImageSequenceAnimation::startWithTarget(CObjectUI* pActionTarget)
	{
		if ( pActionTarget && pActionTarget->GetInterface(_T("AnimationControl")) != NULL )
		{
			CAction::startWithTarget(pActionTarget);
		}
	}

	void ImageSequenceAnimation::Tick(IActionInterval* pActionInterval)
	{
		CObjectUI * pObj =ActionTarget();
		if ( pObj )
		{

		}
	}

	void ImageSequenceAnimation::onStop()
	{

	}

	void ImageSequenceAnimation::onFinished()
	{
		ActionTarget();
	}

	//////////////////////////////////////////////////////////////////////////
	// TransitionAnimation

	TransitionAnimation::TransitionAnimation()
		: m_pOutView(NULL)
		, m_pInView(NULL)
	{

	}

	LPCTSTR TransitionAnimation::GetClass()
	{
		return _T("TransitionAnimation");
	}

	void TransitionAnimation::BindObj(CControlUI* pOut, CControlUI* pIn)
	{
		m_pOutView = pOut;
		m_pInView = pIn;
	}

	CControlUI* TransitionAnimation::GetOut()
	{
		return m_pOutView;
	}

	CControlUI* TransitionAnimation::GetIn()
	{
		return m_pInView;
	}

	void TransitionAnimation::startWithTarget(CObjectUI* pActionTarget)
	{
		CActionManager* pInstance = CActionManager::GetInstance();
		if ( m_pOutView && pInstance )
		{
			pInstance->AddActionPair(this,m_pOutView);
		}
		if ( m_pInView && pInstance )
		{
			pInstance->AddActionPair(this,m_pInView);
		}

		startWithTarget(pActionTarget);
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnTransition
	TurnTransition::TurnTransition()
	{

	}

	LPCTSTR TurnTransition::GetClass()
	{
		return _T("TurnTransition");
	}

	void TurnTransition::Tick(IActionInterval* pActionInterval)
	{

	}

	void TurnTransition::onStop()
	{

	}

	void TurnTransition::onFinished()
	{
		
	}

}