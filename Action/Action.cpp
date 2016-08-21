#include "stdafx.h"
#include "Action.h"
#include "ActionInterval.h"

namespace DuiLib
{
	CAction::CAction()
		: m_pActionInterval(NULL)
		, m_pActionListener(NULL)
		, m_pActionTarget(NULL)
		,m_bFinishedOfRelease(true)
	{
		SetState(ActionState_Ready);
		m_pActionInterval = new CActionInterval;
		m_pActionInterval->SetOwner(this);
		SetCurveID(_T("curve.liner"));
	}

	CAction::~CAction()
	{
		if ( m_pActionInterval )
		{
			delete m_pActionInterval;
			m_pActionInterval = NULL;
		}
		CActionManager::GetInstance()->RemoveAction(this);
	}

	void CAction::SetParam(UINT uInterval, UINT uTotalFrame, bool bLoop /*= false*/, bool bRebound /*= false*/, bool bByStep /*= true*/)
	{
		if ( m_pActionInterval )
		{
			m_pActionInterval->SetParam(uInterval,uTotalFrame,bLoop,bRebound,bByStep);
		}
	}

	void CAction::SetCurFrame(DWORD dwCurFrame, bool bReverse /*= false*/)
	{
		if ( m_pActionInterval )
		{
			m_pActionInterval->SetCurFrame(dwCurFrame,bReverse);
		}
	}

	DWORD CAction::GetCurFrame()
	{
		if ( m_pActionInterval )
		{
			return m_pActionInterval->GetCurFrame();
		}
		
		return 0;
	}

	DWORD CAction::GetTotalFrame()
	{
		if ( m_pActionInterval )
		{
			return m_pActionInterval->GetTotalFrame();
		}

		return 0;
	}

	bool CAction::IsLoop() const
	{
		if ( m_pActionInterval )
		{
			return m_pActionInterval->IsLoop();
		}
		return false;
	}

	bool CAction::IsReverse() const
	{
		if ( m_pActionInterval )
		{
			return m_pActionInterval->IsReverse();
		}

		return false;
	}

	void CAction::FinishedOfRelease(bool val)
	{
		m_bFinishedOfRelease = val;
	}

	LPCTSTR CAction::GetClass()
	{
		return _T("Action");
	}

	void CAction::onFinished()
	{

	}

	void CAction::SetState(CAction::ActionState state)
	{
		m_stateAction = state;
		bool bFinished = false;
		if ( m_pActionInterval)
		{
			switch (m_stateAction)
			{
			case ActionState_Ready:
				m_pActionInterval->Stop();
				break;
			case ActionState_Running:
				if ( m_pActionInterval->IsRunning())
					m_pActionInterval->Resume();
				else
					m_pActionInterval->Start();
				break;
			case ActionState_Paused:
				m_pActionInterval->Paused();
				break;
			case ActionState_Stop:
				m_pActionInterval->Stop();
				break;
			case ActionState_Finished:
				m_pActionInterval->Stop();
				bFinished = true;
				break;
			}

			if ( m_pActionListener )
			{
				m_pActionListener->OnActionStateChanged(this);
			}

			if ( bFinished && m_bFinishedOfRelease )
			{
				delete this;
			}
		}
	}

	void CAction::startWithTarget(CObjectUI* pActionTarget)
	{
		ActionTarget(pActionTarget);
		SetState(ActionState_Running);
	}

	void CAction::onStop()
	{

	}

	void CAction::Paused()
	{
		SetState( ActionState_Paused);
	}

	void CAction::Resume()
	{
		SetState(ActionState_Running);
	}

	CAction::ActionState CAction::GetState() const
	{
		return m_stateAction;
	}

	void CAction::SetActionListener(IActionListener* pActionListener)
	{
		m_pActionListener = pActionListener;
	}

	CObjectUI* CAction::ActionTarget() const
	{
		return m_pActionTarget;
	}

	void CAction::ActionTarget(CObjectUI* pTarget)
	{
		m_pActionTarget = pTarget;
	}

	void CAction::Tick(IActionInterval* pActionInterval)
	{
		//if ( GetState() == ActionState_Running && m_pActionListener)
		//{
		//	m_pActionListener->OnActionStep(this,pActionInterval);
		//}
		//if ( pActionInterval->IsDone() )
		//{
		//	SetState(ActionState_Finished);
		//}
	}

	void CAction::Update(IActionInterval* pActionInterval)
	{
		if ( GetState() == ActionState_Running )
		{
			this->Tick(pActionInterval);
			if (m_pActionListener )
				m_pActionListener->OnActionStep(this,pActionInterval);
		}
		if ( pActionInterval->IsDone() )
		{
			SetState(ActionState_Finished);
		}
	}

	void CAction::SetCurveID(LPCTSTR lpszID)
	{
		m_pCurveObj = GetResEngine()->GetCurveObj(lpszID);
	}



	CCurveObject* CAction::GetCurveObj()
	{
		return m_pCurveObj;
	}

}